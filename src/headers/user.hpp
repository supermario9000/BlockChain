#ifndef USER_HPP
#define USER_HPP
#pragma once

#include "lib.hpp"

class User {
    public:
        string username;
        string public_key;
        vector<double> UTXO_balance;
    
        User(string name, string pub_key)
            : username(name), public_key(pub_key) {}

        string getUsername() const {
            return username;
        }

        string getPublicKey() const {
            return public_key;
        }

        double getTotalBalance() const {
            double total = 0.0;
            for (double utxo : UTXO_balance) {
                total += utxo;
            }
            return total;
        }

        void addUTXO(double amount) {
            UTXO_balance.push_back(amount);
        }

        bool spendUTXO(double amount) {
            double total = getTotalBalance();
            if (amount > total) {
                return false; // Not enough balance
            }

            double remaining = amount;
            //looping through utxos until we cover the amount to spend
            for (auto it = UTXO_balance.begin(); it != UTXO_balance.end() && remaining > 0; ) {
                if (*it <= remaining) {
                    remaining -= *it;
                    it = UTXO_balance.erase(it);
                } else {
                    *it -= remaining;
                    remaining = 0;
                }
            }
            return true;
        }
};

#endif // USER_HPP