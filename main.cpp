#include "src/headers/helperFunctionPrototypes.hpp"

int main() {
    vector<User> users;
    vector<Transaction> transactions;
    vector<Block> blocks;

    cout<<"You are running a simulation of a centralized blockchain network."<<endl;

    while (true) {
        cout<<"Action menu:"<<endl;
        cout << "1. Generate data" << endl; //with blocks, transactions, users
        cout << "2. Explore generated data" << endl;
        cout << "3. Mine blocks" << endl;
        cout << "4. Exit" << endl;
        cout << "Select an action (1-4): ";
        int choice;
        cin >> choice;
        switch (choice) {
            case 1:
                cout<< "Generating data... \n"<<endl;

                cout<<"Generating 1000 users..."<<endl;
                generateUserData(users, 1000);
                writeUsersCsv(users, "users.csv");
                cout<<"Users generated."<<endl;

                cout<<"Generating 10000 transactions..."<<endl;
                generateTransactionData(users, transactions, 10000);
                writeTransactionsCsv(transactions, "transactions.csv");
                cout<<"Transactions generated."<<endl;

                cout<<"Generating blocks..."<<endl;
                generateBlockData(transactions, blocks, 100); // 100 transactions per block
                cout<<"100 blocks generated."<<endl;
                cout<<"Data generation complete. 🎉 \n"<<endl;

                cout<<"You can now explore the data or try mining blocks."<<endl;
                cout<<"Enter any key to continue..."<<endl;
                cin.ignore();
                break;
            case 2:
                cout << "What would you like to explore?" << endl;
                cout << "1. Users" << endl;
                cout << "2. Transactions" << endl;
                cout << "3. Blocks" << endl;
                int explore_choice;
                cin >> explore_choice;
                switch (explore_choice) {
                    case 1:
                        cout<<"Select a user to explore (1-" << users.size() << "): ";
                        int user_choice;
                        cin >> user_choice;
                        while (true) {
                            if (user_choice < 1 || user_choice > users.size()) {
                                cout << "Invalid user selection." << endl;
                            } else {
                                const User& selected_user = users[user_choice - 1];
                                cout << "Exploring user: " << selected_user.getUserInfo() << endl;
                            }
                            cout << "Enter another user number to explore or 0 to go back: ";
                            cin >> user_choice;
                            if (user_choice == 0) {
                                break;
                            }
                        }
                        break;
                    case 2:
                        cout << "Total transactions: " << transactions.size() << endl;
                        for (const auto& tx : transactions) {
                            cout << tx.getTransactionDetails() << endl;
                        }
                        break;
                    case 3:
                        cout << "Total blocks: " << blocks.size() << endl;
                        for (const auto& block : blocks) {
                            cout << block.getBlockHeaderInfo() << ", Transactions: " << block.getTransactionCount() << endl;
                        }
                        break;
                    default:
                        cout << "Invalid choice." << endl;
                        break;
                }

                break;
            case 3:
                cout << "Mining blocks..." << endl;
                // Call function to mine blocks
                break;
            case 4:
                cout << "Exiting program." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please select a valid action." << endl;
                break;
        }
    }
    return 0;
}

/* kasimas grindziamas proof of work algoritmu.
Tai reiskia, kad kiekvienas "kasejas" turi atlikti daug darbo, kol suras tinkama hash reiksme.
Tik surades ta reiksme jis gali prideti bloka i blokchain. T.y. isbroadcastinti ji tinkle.
*/

/* merkel root hash - tai yra kiekvienos transakcijos hashu hashas.
Kai yra kelios transakcijos, jos hashuojamos, tada sujungiamos poromis, 
Tada tos poros hashuojamos ir t.t.
*/

/* utxo modelis veikia kaip gryni pinigai: 
siunciami pinigai (input) perduodami kitam asmeniui, jis juos priima kaip output. 
Tada, automatiskai grazinama graza (antras output), kuria gauna pirmasis siuntejas.
Tokiu budu saugomas ne balansas, o turimi utxo "pinigai", turintys ivairias vertes.
Praktiskai, utxo ir yra pinigas, tik neturintis standartizuotos kupiuros ar monetos vertes.
1 utxo gali buti vertas 100 arba 0.00001. Si technologija pritaikoma BTC.
*/