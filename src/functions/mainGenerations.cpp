#include "../headers/helperFunctionPrototypes.hpp"

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

// CSV escaping: double internal quotes and wrap fields containing comma, quote or newline in quotes
static string csvEscape(const string &s) {
    bool needQuotes = false;
    string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        if (c == '"') {
            out += '"'; // escape by doubling
            out += '"';
            needQuotes = true;
        } else {
            if (c == ',' || c == '\n' || c == '\r') needQuotes = true;
            out += c;
        }
    }
    if (needQuotes) return string("\"") + out + string("\"");
    return out;
}

void writeUsersCsv(const vector<User>& users, const string& filename) {
    ofstream out(filename);
    if (!out) {
        cout << "Failed to open " << filename << " for writing\n";
        return;
    }
    // Header
    out << "username,public_key,total_balance,utxos\n";
    for (const User &u : users) {
        out << csvEscape(u.getUsername()) << ',';
        out << csvEscape(u.getPublicKey()) << ',';
        out << fixed << setprecision(2) << u.getTotalBalance() << ',';
        // utxos as semicolon-separated list inside quotes if necessary
        string utxos;
        for (size_t j = 0; j < u.UTXO_balance.size(); ++j) {
            if (j) utxos += ';';
            ostringstream oss;
            oss << fixed << setprecision(2) << u.UTXO_balance[j];
            utxos += oss.str();
        }
        out << csvEscape(utxos) << '\n';
    }
    out.close();
}

void writeTransactionsCsv(const vector<Transaction>& transactions, const string& filename) {
    ofstream out(filename);
    if (!out) {
        cout << "Failed to open " << filename << " for writing\n";
        return;
    }
    out << "transaction_id,sender_key,receiver_key,amount\n";
    for (const Transaction &t : transactions) {
        out << csvEscape(t.getTransactionId()) << ',';
        out << csvEscape(t.getSenderKey()) << ',';
        out << csvEscape(t.getReceiverKey()) << ',';
        out << fixed << setprecision(2) << t.getAmount() << '\n';
    }
    out.close();
}

