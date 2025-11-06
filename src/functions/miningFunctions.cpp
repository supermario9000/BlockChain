#include "../headers/helperFunctionPrototypes.hpp"
#include "../headers/hash_function.hpp"
#include <thread>
#include <atomic>
#include <random>
#include <limits>
#include <fstream>

const int requiredTxAmount = 100;

int getDifficultyForBlock(const vector<Block>& blocks) {
    const int baseDifficulty = 3;
    size_t minedCount = 0;
    for (const auto &b : blocks) if (b.mined) ++minedCount;
    return baseDifficulty + static_cast<int>(minedCount / 10);
}

// Mine a single block from the front of the mempool and append it to the chain.
bool mineBlocks(vector<Block>& blocks, vector<Transaction>& mempool, vector<User>& users, const std::chrono::steady_clock::time_point &deadline, bool randomNonce, const string &minerLabel) {
    if (mempool.empty()) return false;

    // Select up to requiredTxAmount valid transactions, simulating UTXO changes on a copy.
    vector<User> temp_users = users; // copy to simulate spends
    vector<Transaction> includedTxs;
    vector<char> usedFlag(mempool.size(), 0);

    for (size_t i = 0; i < mempool.size() && includedTxs.size() < static_cast<size_t>(requiredTxAmount); ++i) {
        const Transaction &tx = mempool[i];
        auto s_it = std::find_if(temp_users.begin(), temp_users.end(), [&](const User &u){ return u.getPublicKey() == tx.getSenderKey(); });
        if (s_it == temp_users.end()) continue; // unknown sender
        bool spent = s_it->spendUTXO(tx.getAmount());
        if (!spent) continue; // insufficient funds
        auto r_it = std::find_if(temp_users.begin(), temp_users.end(), [&](const User &u){ return u.getPublicKey() == tx.getReceiverKey(); });
        if (r_it != temp_users.end()) r_it->addUTXO(tx.getAmount());
        includedTxs.push_back(tx);
        usedFlag[i] = 1;
    }

    if (includedTxs.empty()) return false;

    Block newBlock;
    for (const auto &t : includedTxs) newBlock.addTransaction(t);
    newBlock.setPreviousHash(blocks.empty() ? "" : blocks.back().getBlockHash());
    newBlock.setMerkleRoot(calculateMerkleRoot(newBlock.transactions));
    newBlock.setVersion("1.0");
    newBlock.setDifficultyTarget(getDifficultyForBlock(blocks));
    newBlock.setTimestamp(std::time(0));
    newBlock.setNonce(0);

    const int difficulty = getDifficultyForBlock(blocks);
    const string targetPrefix(difficulty, '0');

    // Prepare RNG for random-nonce mode (per-call seed to avoid cross-thread correlation)
    std::mt19937_64 rng;
    std::uniform_int_distribution<unsigned long long> distULL(0, static_cast<unsigned long long>(std::numeric_limits<int>::max()));
    if (randomNonce) {
        auto seed = static_cast<unsigned long long>(std::chrono::steady_clock::now().time_since_epoch().count()) ^ static_cast<unsigned long long>(std::hash<std::thread::id>()(std::this_thread::get_id()));
        rng.seed(seed);
    }

    long nonce = 0;
    string hash;
    bool found = false;
    while (std::chrono::steady_clock::now() < deadline) {
        if (randomNonce) {
            unsigned long long r = distULL(rng);
            newBlock.nonce = static_cast<int>(r & 0x7FFFFFFF);
        } else {
            newBlock.nonce = nonce++;
        }
        hash.clear();
        hashFunkcija(newBlock.previousHash + newBlock.merkleRoot + to_string(newBlock.timestamp) + to_string(newBlock.nonce), hash);
        if (hash.size() >= static_cast<size_t>(difficulty) && hash.substr(0, difficulty) == targetPrefix) { found = true; break; }
    }

    if (!found) return false; // deadline expired without finding valid nonce

    // commit: append block and commit UTXO state
    newBlock.mined = true;
    blocks.push_back(newBlock);
    users = std::move(temp_users);

    // remove used txs from mempool
    vector<Transaction> newMempool;
    newMempool.reserve(mempool.size() - includedTxs.size());
    for (size_t i = 0; i < mempool.size(); ++i) if (!usedFlag[i]) newMempool.push_back(mempool[i]);
    mempool.swap(newMempool);

    if (!minerLabel.empty()) cout << "[" << minerLabel << "] ";
    cout << "\nMined block " << (blocks.size() - 1) << " with nonce " << newBlock.nonce << " producing hash: " << hash << " (used " << includedTxs.size() << " tx)" << endl;
    return true;
}

// Run two miners in parallel on independent copies of the provided state for a fixed duration.
// Miner1 uses sequential nonces, miner2 uses random nonces. The function returns 1, 2 or 0 for
// no clear winner (difference < winThreshold).
int parallelMiningCompetition(const vector<Block>& blocksInit, const vector<Transaction>& mempoolInit, const vector<User>& usersInit, int durationSeconds, int winThreshold) {
    using clock = std::chrono::steady_clock;
    const auto start = clock::now();
    const auto deadline = start + std::chrono::seconds(durationSeconds);

    // independent copies
    vector<Block> blocks1 = blocksInit; vector<Transaction> mempool1 = mempoolInit; vector<User> users1 = usersInit;
    vector<Block> blocks2 = blocksInit; vector<Transaction> mempool2 = mempoolInit; vector<User> users2 = usersInit;

    std::atomic<int> mined1(0), mined2(0);

    auto worker = [&](vector<Block>& b, vector<Transaction>& m, vector<User>& u, std::atomic<int>& counter, bool randomNonce, const string &label) {
        while (clock::now() < deadline && !m.empty()) {
            bool ok = mineBlocks(b, m, u, deadline, randomNonce, label);
            if (!ok) break; // either deadline reached or nothing valid to mine
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread t1(worker, std::ref(blocks1), std::ref(mempool1), std::ref(users1), std::ref(mined1), false, string("Miner1"));
    std::thread t2(worker, std::ref(blocks2), std::ref(mempool2), std::ref(users2), std::ref(mined2), true, string("Miner2"));

    t1.join(); t2.join();

    const int c1 = mined1.load(); const int c2 = mined2.load();
    cout << "Parallel mining finished: miner1=" << c1 << " miner2=" << c2 << endl;

    // Write CSV outputs for both miners (simulation results). Filenames include timestamp to avoid clobbering.
    auto writeBlocksCsv = [&](const vector<Block>& blks, const string& fname, const string& label) {
        ofstream out(fname);
        if (!out) {
            cout << "Failed to open " << fname << " for writing\n";
            return;
        }
        out << "index,miner,label,mined,nonce,timestamp,previous_hash,merkle_root,block_hash,tx_count,tx_ids\n";
        for (size_t i = 0; i < blks.size(); ++i) {
            const Block &B = blks[i];
            out << i << ',' << label << ',' << label << ',' << (B.mined ? 1 : 0) << ',' << B.nonce << ',' << B.timestamp << ',';
            out << '"' << B.previousHash << '"' << ',' << '"' << B.merkleRoot << '"' << ',' << '"' << B.getBlockHash() << '"' << ',' << B.getTransactionCount() << ',';
            // transaction ids as semicolon-separated list
            string txids;
            for (size_t ti = 0; ti < B.transactions.size(); ++ti) {
                if (ti) txids += ';';
                txids += B.transactions[ti].getTransactionId();
            }
            out << '"' << txids << '"' << '\n';
        }
        out.close();
        cout << "Wrote " << fname << " (" << blks.size() << " blocks)\n";
    };

    // Use fixed filenames (overwrite previous runs) per user preference
    writeBlocksCsv(blocks1, string("miner1_blocks.csv"), "Miner1");
    writeBlocksCsv(blocks2, string("miner2_blocks.csv"), "Miner2");

    const int diff = std::abs(c1 - c2);
    if (diff >= winThreshold) {
        const int winner = (c1 > c2) ? 1 : 2;
        cout << "Winner: miner " << winner << " (difference " << diff << " >= " << winThreshold << ")\n";
        return winner;
    }
    cout << "No clear winner (difference " << diff << " < " << winThreshold << ")\n";
    return 0;
}