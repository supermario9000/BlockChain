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
bool mineBlocks(vector<Block>& blocks, vector<Transaction>& mempool, const std::chrono::steady_clock::time_point &deadline) {
    if (mempool.empty()) {
        // nothing to mine
        return false;
    }

    size_t take = std::min(mempool.size(), static_cast<size_t>(requiredTxAmount));
    Block newBlock;
    // add transactions to the new block
    for (size_t i = 0; i < take; ++i) {
        newBlock.addTransaction(mempool[i]);
    }
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

    // append to chain
    newBlock.mined = true;
    blocks.push_back(newBlock);
    // remove used transactions from mempool
    mempool.erase(mempool.begin(), mempool.begin() + static_cast<ptrdiff_t>(take));

    cout << "\nMined block " << (blocks.size() - 1) << " with nonce " << newBlock.nonce << " producing hash: " << hash << " (used " << take << " tx)" << endl;
    return true;
}