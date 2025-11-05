#ifndef BLOCK_HPP
#define BLOCK_HPP
#pragma once

#include "lib.hpp"
#include "transaction.hpp"
#include "hash_function.hpp"

class Block { 
    public:
        //header
        string previousHash;
        string merkleRoot;
        string version;
        int difficultyTarget;
        long timestamp;
        int nonce;
        //body
        vector<Transaction> transactions; //a vector of transactions. This would make it easier to manipulate blocks

        // Default constructor - initializes an empty block (useful when creating vectors of Block)
        Block()
            : previousHash(""), merkleRoot(""), version("1"), difficultyTarget(0), timestamp(0), nonce(0) {}

        // Parameterized constructor
        Block(string prevHash, string merkleRootHash, string ver, int diffTarget, long time, int nonceValue)
            : previousHash(prevHash), merkleRoot(merkleRootHash), version(ver), difficultyTarget(diffTarget), timestamp(time), nonce(nonceValue) {}

        void addTransaction(const Transaction& tx) {
            transactions.push_back(tx);
        }

        string getBlockHeaderInfo() const {
            return "Version: " + version + ", Previous Hash: " + previousHash + ", Merkle Root: " + merkleRoot +
                   ", Difficulty Target: " + to_string(difficultyTarget) + ", Timestamp: " + to_string(timestamp) +
                   ", Nonce: " + to_string(nonce);
        }

        int getTransactionCount() const {
            return transactions.size();
        }

        vector<string> getTransactionSummaries() const {
            vector<string> summaries;
            for (const auto& tx : transactions) {
                summaries.push_back(tx.getTransactionSummary());
            }
            return summaries;
        }

        //header setters
        string getBlockHash() const {
            string hash;
            hashFunkcija(previousHash + merkleRoot + to_string(timestamp) + to_string(nonce), hash);
            return hash;
        }

        void setPreviousHash(const string& prevHash) {
            previousHash = prevHash;
        }

        void setMerkleRoot(const string& merkleRootHash) {
            merkleRoot = merkleRootHash;
        }

        void setVersion(const string& ver) {
            version = ver;
        }

        void setDifficultyTarget(int diffTarget) {
            difficultyTarget = diffTarget;
        }

        void setTimestamp(long time) {
            timestamp = time;
        }

        void setNonce(int nonceValue) {
            nonce = nonceValue;
        }

};

#endif // BLOCK_HPP
