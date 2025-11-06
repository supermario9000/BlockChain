#include "src/headers/helperFunctionPrototypes.hpp"

int main() {
    vector<User> users;
    vector<Transaction> transactions;
    vector<Block> blocks;

    cout<<"You are running a simulation of a centralized blockchain network."<<endl;

    while (true) {
        cout<<"\nAction menu:"<<endl;
    cout << "1. Generate data" << endl; //with blocks, transactions, users
    cout << "2. Explore generated data" << endl;
    cout << "3. Mine blocks (single-threaded)" << endl;
    cout << "4. Parallel mining competition (two miners)" << endl;
    cout << "5. Exit" << endl;
    cout << "Select an action (1-5): ";
        int choice;
        cin >> choice;
        switch (choice) {
            case 1:
                cout<< "Generating data... \n"<<endl;

                cout<<"Generating 1000 users..."<<endl;
                generateUserData(users, 1000);
                //writeUsersCsv(users, "users.csv");
                cout<<"Users generated."<<endl;

                cout<<"Generating 10000 transactions..."<<endl;
                generateTransactionData(users, transactions, 10000);
                //writeTransactionsCsv(transactions, "transactions.csv");
                cout<<"Transactions generated."<<endl;

                cout<<"Do you want to generate blocks now? (y/n): ";
                char gen_blocks_choice;
                cin >> gen_blocks_choice;
                if (gen_blocks_choice == 'y' || gen_blocks_choice == 'Y') {
                    cout<<"Generating blocks..."<<endl;
                    generateBlockData(transactions, blocks, 100, users); // 100 transactions per block
                    cout<<"100 blocks generated."<<endl;
                }
                cout<<"Data generation complete. 🎉 \n"<<endl;

                cout<<"You can now explore the data or try mining blocks."<<endl;
                break;
            case 2:
                // require generated data first
                if (users.empty() || transactions.empty()) {
                    cout << "Please generate data first (choose menu option 1)." << endl;
                    break;
                }
                cout << "\nWhat would you like to explore?" << endl;
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
                            if (user_choice < 1 || user_choice > static_cast<int>(users.size())) {
                                cout << "Invalid user selection." << endl;
                            } else {
                                const User& selected_user = users[user_choice - 1];
                                cout << "Exploring user: \n" << selected_user.getUserInfo() << endl;
                            }
                            cout << "\nEnter another user number to explore or 0 to go back: ";
                            cin >> user_choice;
                            if (user_choice == 0) {
                                break;
                            }
                        }
                        break;
                    case 2:
                        if (transactions.empty()) {
                            cout << "No transactions available. Generate data first." << endl;
                            break;
                        }
                        cout << "Total transactions: " << transactions.size() << endl;
                        cout << "Select a transaction to explore (1-" << transactions.size() << "), or 0 to go back: ";
                        int tx_choice;
                        cin >> tx_choice;
                        while (true) {
                            if (tx_choice == 0) break;
                            if (tx_choice < 1 || tx_choice > static_cast<int>(transactions.size())) {
                                cout << "Invalid transaction selection." << endl;
                            } else {
                                const Transaction &selected_tx = transactions[tx_choice - 1];
                                cout << "\nExploring transaction indexed at " << (tx_choice - 1) << ": \n" << selected_tx.getTransactionDetails() << endl;
                            }
                            cout << "\nEnter another transaction number to explore or 0 to go back: ";
                            cin >> tx_choice;
                        }
                        break;
                    case 3:
                        if (blocks.empty()) {
                            cout << "No blocks available. Generate data first." << endl;
                            break;
                        }
                        cout << "Total blocks: " << blocks.size() << endl;
                        cout << "Select a block to explore (1-" << blocks.size() << "), or 0 to go back: ";
                        int blk_choice;
                        cin >> blk_choice;
                        while (true) {
                            if (blk_choice == 0) break;
                            if (blk_choice < 1 || blk_choice > static_cast<int>(blocks.size())) {
                                cout << "Invalid block selection." << endl;
                            } else {
                                const Block &selected_block = blocks[blk_choice - 1];
                                cout << "\nExploring block: \n" << selected_block.getBlockHeaderInfo() << "\nTransactions: " << selected_block.getTransactionCount() << endl;

                                cout << "Do you want to see transaction summaries inside the block? (y/n)" << endl;
                                char see_summaries;
                                cin >> see_summaries;
                                if (see_summaries == 'y' || see_summaries == 'Y') {
                                    auto summaries = selected_block.getTransactionSummaries();
                                    for (size_t ti = 0; ti < summaries.size(); ++ti) {
                                        cout << "  [" << (ti + 1) << "] " << summaries[ti] << endl;
                                    }
                                }
                            }
                            cout << "\nEnter another block number to explore or 0 to go back: ";
                            cin >> blk_choice;
                        }
                        break;
                    default:
                        cout << "Invalid choice." << endl;
                        break;
                }

                break;
            case 3:
                // require blocks to exist before mining
                if (users.empty() || transactions.empty()) {
                    cout << "Please generate data first (choose menu option 1)." << endl;
                    break;
                }
                cout<<"How long do you want to mine? (in seconds): ";
                int miningDuration;
                cin >> miningDuration;

                cout << "Mining blocks for " << miningDuration << " seconds..." << endl;
                {
                    using clock = steady_clock;
                    auto start = clock::now();
                    auto deadline = start + seconds(miningDuration);
                    int mined_count = 0;
                    // keep mining one block at a time until deadline or mempool empty
                    while (clock::now() < deadline && !transactions.empty()) {
                        bool mined = mineBlocks(blocks, transactions, users, deadline);
                        if (!mined) break; // time expired during mining or nothing to mine
                        ++mined_count;
                    }
                    cout << "Mining session finished. Blocks mined: " << mined_count << "\n";
                }
                break;
            case 4:
                // Parallel competition (two independent miners)
                if (users.empty() || transactions.empty()) {
                    cout << "Please generate data first (choose menu option 1)." << endl;
                    break;
                }
                cout<<"\nParallel mining works on two threads. 1st miner uses sequential nonces, 2nd miner uses random nonces."<<endl;
                cout<<"Data of mined blocks by each miner will be printed in miner1_blocks.csv and miner2_blocks.csv files."<<endl;

                cout << "\nParallel mining competition - duration (seconds): ";
                {
                    int compDuration = 0;
                    cin >> compDuration;
                    cout << "Win threshold (difference in blocks to declare winner, default 2): ";
                    int winThreshold = 2;
                    if (!(cin >> winThreshold)) {
                        winThreshold = 2;
                        cin.clear();
                        string dummy; getline(cin, dummy);
                    }
                    cout << "Running competition for " << compDuration << "s with threshold " << winThreshold << "..." << endl;
                    int winner = parallelMiningCompetition(blocks, transactions, users, compDuration, winThreshold);
                    if (winner == 0) cout << "Competition ended with no clear winner." << endl;
                    else cout << "Competition winner: miner " << winner << endl;
                    cout << "(Note: this was a simulation on independent copies; program state was not modified.)" << endl;
                }
                break;
            case 5:
                cout << "Exiting program." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please select a valid action." << endl;
                break;
        }
    }
    return 0;
}

//notes to self below

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