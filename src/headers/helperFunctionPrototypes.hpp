#ifndef HELPERFUNCTIONPROTOTYPES_HPP
#define HELPERFUNCTIONPROTOTYPES_HPP

#pragma once

#include "lib.hpp"
#include "user.hpp"
#include "transaction.hpp"
#include "block.hpp"

// helper generations
string generateUsername();
string generatePublicKey();
int generateInitialBalance();
string generateTransactionID();
double generateTransactionAmount(double max_amount);
void selectTransactionParticipants(const vector<class User>& users, string& sender_key, string& receiver_key);

// main generations
void generateUserData(vector<class User>& users, int numUsers);
void generateTransactionData(const vector<class User>& users, vector<class Transaction>& transactions, int numTransactions);
void generateBlockData(const vector<class Transaction>& transactions, vector<class Block>& blocks, int numTransactionsPerBlock, vector<class User>& users);//only a simulation, not how a real block would work while mining

// mining function and helpers
// If randomNonce is true the miner will try random nonces instead of sequentially incrementing.
bool mineBlocks(vector<class Block>& blocks, vector<class Transaction>& mempool, vector<class User>& users, const std::chrono::steady_clock::time_point &deadline, bool randomNonce = false, const string &minerLabel = "");
int getDifficultyForBlock(const vector<class Block>& blocks);
string calculateMerkleRoot(const vector<class Transaction>& transactions);//also a generation function helper

// input checks
bool hasSufficientBalance(const vector<class User>& users, const string &public_key, double amount);
bool transactionIdExists(const vector<class Transaction>& transactions, const string &tx_id);

// Parallel mining competition: run two miners in parallel on independent copies of the same.
// 1st miner runs sequential nonce, miner 2 guesses the nonce randomly.
// initial state for `durationSeconds`. Returns 1 if miner1 wins, 2 if miner2 wins, 0 for tie/no clear winner.
int parallelMiningCompetition(const vector<class Block>& blocks, const vector<class Transaction>& mempool, const vector<class User>& users, int durationSeconds, int winThreshold = 2);

// CSV output helpers
//void writeUsersCsv(const vector<class User>& users, const string& filename);
//void writeTransactionsCsv(const vector<class Transaction>& transactions, const string& filename);

#endif // HELPERFUNCTIONPROTOTYPES_HPP