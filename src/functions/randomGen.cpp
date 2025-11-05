#include "src/headers/lib.hpp"
#include "src/headers/user.hpp"

string generateUsername() {
    // Function to generate a random username from predefined pools
    string name_start_pool[] = {"the","mister","lady","dr","professor","captain","agent","shadow","dark","light","fire","ice","storm","wolf","fox","eagle","lion","tiger","bear","dragon"};
    string name_middle_pool[] = {"quick","silent","fierce","brave","clever","sly","bold","wise","strong","swift","mighty","noble","fierce","gentle","wild","free","loyal","proud","fierce","calm"};
    string name_end_pool[] = {"hunter","warrior","rider","seeker","guardian","fighter","runner","walker","strider","chaser","stalker","prowler","voyager","explorer","nomad","wanderer","traveler","adventurer","pilgrim","roamer"};
    
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<> start_dist(0, sizeof(name_start_pool)/sizeof(name_start_pool[0]) - 1);
    uniform_int_distribution<> middle_dist(0, sizeof(name_middle_pool)/sizeof(name_middle_pool[0]) - 1);
    uniform_int_distribution<> end_dist(0, sizeof(name_end_pool)/sizeof(name_end_pool[0]) - 1);
    
    string username = name_start_pool[start_dist(eng)] + "_" + name_middle_pool[middle_dist(eng)] + "_" + name_end_pool[end_dist(eng)];
    return username;
}

string generatePublicKey() {
    // Function to generate a random public key (a simple hex string for simulation)
    const char hex_chars[] = "0123456789ABCDEF";
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<> hex_dist(0, 15);
    
    string pub_key = "";
    for (int i = 0; i < 64; ++i) { // Generate a 64-character hex string
        pub_key += hex_chars[hex_dist(eng)];
    }
    return pub_key;
}

int generateInitialBalance() {
    // Function to generate a random initial balance between 100.0 and 1000000.0
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<> balance_dist(100, 1000000);
    
    return balance_dist(eng);
}

string generateTransactionID() {
    // Function to generate a random transaction ID (a simple hex string for simulation)
    const char hex_chars[] = "0123456789ABCDEF";
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<> hex_dist(0, 15);
    
    string tx_id = "";
    for (int i = 0; i < 32; ++i) { // Generate a 32-character hex string
        tx_id += hex_chars[hex_dist(eng)];
    }
    return tx_id;
}

void selectTransactionParticipants(const vector<User>& users, string& sender_key, string& receiver_key) {
    // Function to randomly select sender and receiver from the list of users
    if (users.size() < 2) {
        sender_key = "";
        receiver_key = "";
        return; // Not enough users to select from
    }
    
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<> user_dist(0, users.size() - 1);
    
    int sender_index = user_dist(eng);
    int receiver_index;
    do {
        receiver_index = user_dist(eng);
    } while (receiver_index == sender_index); // Ensure sender and receiver are different
    
    sender_key = users[sender_index].getPublicKey();
    receiver_key = users[receiver_index].getPublicKey();
}

double generateTransactionAmount(double max_amount) {
    // Function to generate a random transaction amount between 0 and max_amount
    random_device rd;
    mt19937_64 eng(rd());
    uniform_real_distribution<> amount_dist(0.0, max_amount);
    
    return amount_dist(eng);
}

