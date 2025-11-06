#include "../headers/helperFunctionPrototypes.hpp"
#include "../headers/hash_function.hpp"

const int requiredTxAmount = 100;

int getDifficultyForBlock(const vector<Block>& blocks) {
    const int baseDifficulty = 3;
    // count only blocks that were actually mined by miner (ignore simulated blocks)
    size_t minedCount = 0;
    for (const auto &b : blocks) if (b.mined) ++minedCount;
    return baseDifficulty + static_cast<int>(minedCount / 10);
}

// Mine a single block from the front of the mempool and append it to the chain.
// If mempool has fewer than requiredTxAmount transactions, it will mine a smaller block.
bool mineBlocks(vector<Block>& blocks, vector<Transaction>& mempool, vector<User>& users, const std::chrono::steady_clock::time_point &deadline) {
    if (mempool.empty()) {
        // nothing to mine
        return false;
    }

    // We'll select up to requiredTxAmount valid transactions from the front of the mempool.
    // To ensure UTXO correctness we simulate applying spends on a copy of users, and only
    // commit the changes to the real users vector after the block is successfully mined.
    vector<User> temp_users = users; // copy to simulate spends
    vector<Transaction> includedTxs;
    vector<char> usedFlag(mempool.size(), 0);

    for (size_t i = 0; i < mempool.size() && includedTxs.size() < static_cast<size_t>(requiredTxAmount); ++i) {
        const Transaction &tx = mempool[i];
        // find sender in temp_users
        auto s_it = std::find_if(temp_users.begin(), temp_users.end(), [&](const User &u){ return u.getPublicKey() == tx.getSenderKey(); });
        if (s_it == temp_users.end()) continue; // unknown sender -> skip
        // attempt to spend on temp_users (this mutates temp_users)
        bool spent = s_it->spendUTXO(tx.getAmount());
        if (!spent) {
            continue; // insufficient funds at mining time, skip tx
        }
        // credit receiver
        auto r_it = std::find_if(temp_users.begin(), temp_users.end(), [&](const User &u){ return u.getPublicKey() == tx.getReceiverKey(); });
        if (r_it != temp_users.end()) {
            r_it->addUTXO(tx.getAmount());
        }
        includedTxs.push_back(tx);
        usedFlag[i] = 1;
    }

    if (includedTxs.empty()) {
        // no valid transactions to include
        return false;
    }

    Block newBlock;
    for (const auto &t : includedTxs) newBlock.addTransaction(t);

    // set header fields
    newBlock.setPreviousHash(blocks.empty() ? "" : blocks.back().getBlockHash());
    newBlock.setMerkleRoot(calculateMerkleRoot(newBlock.transactions));
    newBlock.setVersion("1.0");
    newBlock.setDifficultyTarget(getDifficultyForBlock(blocks));
    newBlock.setTimestamp(std::time(0));
    newBlock.setNonce(0);

    // mining loop (interruptible)
    int difficulty = getDifficultyForBlock(blocks);
    string targetPrefix(difficulty, '0');
    long nonce = 0;
    string hash;
    while (true) {
        // check deadline before each attempt
        if (std::chrono::steady_clock::now() >= deadline) {
            // time expired; abort this mining attempt and do not append the block
            return false;
        }
        newBlock.nonce = nonce++;
        hash.clear();
        hashFunkcija(newBlock.previousHash + newBlock.merkleRoot + to_string(newBlock.timestamp) + to_string(newBlock.nonce), hash);
        if (hash.size() >= static_cast<size_t>(difficulty) && hash.substr(0, difficulty) == targetPrefix) {
            break; // mined successfully
        }
    }

    // append to chain and commit UTXO changes
    newBlock.mined = true;
    blocks.push_back(newBlock);
    users = std::move(temp_users); // commit the simulated UTXO state

    // build remaining mempool (remove used txs)
    vector<Transaction> newMempool;
    newMempool.reserve(mempool.size() - includedTxs.size());
    for (size_t i = 0; i < mempool.size(); ++i) {
        if (!usedFlag[i]) newMempool.push_back(mempool[i]);
    }
    mempool.swap(newMempool);

    cout << "\nMined block " << (blocks.size() - 1) << " with nonce " << newBlock.nonce << " producing hash: " << hash << " (used " << includedTxs.size() << " tx)" << endl;
    return true;
}