#include "../headers/helperFunctionPrototypes.hpp"

// Return true if a user with given public_key exists and has total balance >= amount
bool hasSufficientBalance(const vector<User>& users, const string &public_key, double amount) {
    for (const auto &u : users) {
        if (u.getPublicKey() == public_key) {
            return u.getTotalBalance() >= amount;
        }
    }
    return false; // user not found -> insufficient
}

// Return true if a transaction id already exists in the given transaction list
bool transactionIdExists(const vector<Transaction>& transactions, const string &tx_id) {
    for (const auto &t : transactions) {
        if (t.getTransactionId() == tx_id) return true;
    }
    return false;
}
