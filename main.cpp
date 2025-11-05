#include "src/headers/helperFunctionPrototypes.hpp"

int main() {
    cout<<"You are running a simulation of a centralized blockchain network."<<endl;
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
            vector<User> users;
            vector<Transaction> transactions;
            vector<Block> blocks;
            cout<< "Generating data..."<<endl;
            cout<<"Generating 1000 users..."<<endl;
            generateUserData(users, 1000);
            cout<<"Users generated."<<endl;

            cout<<"Generating 10000 transactions..."<<endl;
            generateTransactionData(users, transactions, 10000);
            cout<<"Transactions generated."<<endl;
            
            cout<<"Generating blocks..."<<endl;
            cout<<"Genesis block generated."<<endl;
            cout<<"100 blocks generated."<<endl;
            cout<<"Data generation complete. 🎉"<<endl;

            cout<<"You can now explore the data or try mining blocks."<<endl;
            break;
        case 2:
            cout << "What would you like to explore?" << endl;
            cout << "1. Users" << endl;
            cout << "2. Transactions" << endl;
            cout << "3. Blocks" << endl;
            int explore_choice;
            cin >> explore_choice;
            // Call function to explore data

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