#include "src/headers/helperFunctionPrototypes.hpp"

// use functions defined in randomGen.cpp to generate user data
void generateUserData(vector<User>& users, int numUsers) {
    for (int i = 0; i < numUsers; ++i) {
        string username = generateUsername();
        string public_key = generatePublicKey();
        User newUser(username, public_key);
        double initial_balance = generateInitialBalance();
        newUser.addUTXO(initial_balance);
        users.push_back(newUser);
    }
}

void generateTransactionData(const vector<User>& users, vector<Transaction>& transactions, int numTransactions) {
    for (int i = 0; i < numTransactions; ++i) {
        string tx_id = generateTransactionID();
        string sender_key, receiver_key;
        selectTransactionParticipants(users, sender_key, receiver_key);
        double max_amount = 1000.0; // For simulation, max transaction amount
        double amount = generateTransactionAmount(max_amount);
        Transaction newTx(tx_id, sender_key, receiver_key, amount);
        transactions.push_back(newTx);
    }
}

