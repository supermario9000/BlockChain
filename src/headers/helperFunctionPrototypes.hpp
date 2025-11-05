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

// mining helpers
// Mine a single block before the provided deadline. Returns true if a block was mined and appended.
bool mineBlocks(vector<class Block>& blocks, vector<class Transaction>& mempool, const std::chrono::steady_clock::time_point &deadline);
string calculateMerkleRoot(const vector<class Transaction>& transactions);//also a generation function helper

// CSV output helpers
void writeUsersCsv(const vector<class User>& users, const string& filename);
void writeTransactionsCsv(const vector<class Transaction>& transactions, const string& filename);

#endif // HELPERFUNCTIONPROTOTYPES_HPP