#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP
#pragma once

#include "lib.hpp"

class Transaction {
    public:
        string transaction_id;
        string sender_key;
        string receiver_key;
        double amount;

        Transaction(string tx_id, string sndr, string rcvr, double amt)
            : transaction_id(tx_id), sender_key(sndr), receiver_key(rcvr), amount(amt) {}

        string getTransactionId() const {
            return transaction_id;
        }

        string getSenderKey() const {
            return sender_key;
        }

        string getReceiverKey() const {
            return receiver_key;
        }

        double getAmount() const {
            return amount;
        }

        string getTransactionDetails() const {
            return "Transaction ID: " + transaction_id + "\nFrom: " + sender_key + "\nTo: " + receiver_key + "\nAmount: " + to_string(amount);
        }

        string getTransactionSummary() const {
            return sender_key + " -> " + receiver_key + ": " + to_string(amount);
        }

        //setters are kind of not needed as transaction data should be immutable after creation
};

#endif // TRANSACTION_HPP