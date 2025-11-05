#include "../headers/helperFunctionPrototypes.hpp"
#include "../headers/hash_function.hpp"

// Small terminal progress bar helper
static void printProgressBar(size_t current, size_t total) {
    const int barWidth = 50;
    if (total == 0) return;
    double fraction = double(current) / double(total);
    int pos = static_cast<int>(barWidth * fraction);
    std::cout << '\r' << "Generating blocks: [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << '=';
        else if (i == pos) std::cout << '>';
        else std::cout << ' ';
    }
    int percent = static_cast<int>(fraction * 100.0);
    std::cout << "] " << percent << "%" << std::flush;
}

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

void generateBlockData(const vector<Transaction>& transactions, vector<Block>& blocks, int numTransactionsPerBlock) {
    // using hash_function.hpp
    if (transactions.empty() || numTransactionsPerBlock <= 0) return;
    size_t txCount = transactions.size();
    size_t totalBlocks = (txCount + static_cast<size_t>(numTransactionsPerBlock) - 1) / static_cast<size_t>(numTransactionsPerBlock);
    for (size_t i = 0; i < totalBlocks; ++i) {
        Block newBlock;
        for (int j = 0; j < numTransactionsPerBlock && (i * static_cast<size_t>(numTransactionsPerBlock) + j) < txCount; ++j) {
            newBlock.addTransaction(transactions[i * static_cast<size_t>(numTransactionsPerBlock) + j]);
        }
        // calculating the header fields
        newBlock.setPreviousHash(blocks.empty() ? "" : blocks.back().getBlockHash());
        newBlock.setMerkleRoot(calculateMerkleRoot(newBlock.transactions));
        newBlock.setVersion("1.0");
        newBlock.setDifficultyTarget(0); // example difficulty
        newBlock.setTimestamp(std::time(0));
        newBlock.setNonce(0); // to be set during mining
        blocks.push_back(newBlock);
        // update progress bar
        printProgressBar(i + 1, totalBlocks);
        if (i == 0) std::cout << "\nGenesis block generated." << std::endl;
    }
    // finish line
    std::cout << std::endl;
}

string calculateMerkleRoot(const vector<Transaction>& transactions) {
    if (transactions.empty()) return "";

    vector<string> layer;
    string hash;
    for (const auto& tx : transactions) {
        hashFunkcija(tx.getTransactionId(), hash);
        layer.push_back(hash);
    }
    hash.clear();

    while (layer.size() > 1) {
        vector<string> nextLayer;
        for (size_t i = 0; i < layer.size(); i += 2) {
            if (i + 1 < layer.size()) {
                string combined = layer[i] + layer[i + 1];
                string hash;
                hashFunkcija(combined, hash);
                nextLayer.push_back(hash);
            } else {
                nextLayer.push_back(layer[i]);
            }
        }
        layer = nextLayer;
    }
    return layer.front();
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

