# A simplified centralized blockchain simulation

## How to use

1. Paste "g++ -std=c++17 -I src/headers src/functions/*.cpp main.cpp -O2 -Wall -Wextra -o BlockChain.exe" in the project root terminal.  
2. Open BlockChain.exe or use "./BlockChain.exe".  
3. By typing numbers 0-4, you can navigate the menu and use the program.  
4. Enjoy :).

# Release notes  

## V0.1

This release comes with these features:  

* A functional blockchain simulation, which includes:
    1. 1000 users (class) which have generated usernames, public_keys and __UTXO__ type balances.  
    2. 10000 __UTXO__ type transactions (class) which consist of transaction_ids, sender_key, receiver_key and the amount.  
    3. Block class which has a vector of transactions as a body and a header of a previous_block_hash, timestamp, version, __merkle_root_hash__, nonce and difficulty_target. (Last two are set to 0 when generating.)  
    4. Ability to randomly generate blocks with 100 transactions each.  
    5. Ability to mine blocks with 100 transactions each. (the difficulty is set to 3 by default when mining for faster results.)  
    6. A hash function from the previous project (linked in main branch), modified for better performance and usability.  
    7. Automatically updating balances based on the transactions made.
    8. A functional __Merkle tree__ algorithm.  
* A functional menu, which handles wrong inputs (exept for arrow events).
* Commented out csv writing funcitons for testing the transactions and user generation results.

What is missing from this program:  
* Parallel mining simulation;
* Validation of transactions and balances, correct mining;
* Genesis block generation is not there when mining.


A showcase of the program:  
  
Main menu selection  
<img width="274" height="131" alt="image" src="https://github.com/user-attachments/assets/b321b7b4-489a-49f0-8721-8a95100a55ca" />  

Generation menu without generating blocks  
<img width="459" height="240" alt="image" src="https://github.com/user-attachments/assets/a44b203d-ab95-4e29-8745-11d8410c5e17" />  

Data explore menu  
<img width="292" height="107" alt="image" src="https://github.com/user-attachments/assets/9775807e-fc0d-423b-87ab-ffee99280009" />  

Explore a user menu  
<img width="695" height="176" alt="image" src="https://github.com/user-attachments/assets/00bdcb2f-bde7-4a74-a0a0-f5dbda4551e2" />  

Explore a transaction menu  
<img width="638" height="219" alt="image" src="https://github.com/user-attachments/assets/54bfcd2f-248a-42d8-803a-8c9530e97450" />  

Explore a block menu  
<img width="719" height="261" alt="image" src="https://github.com/user-attachments/assets/0a4112f1-252d-46be-a052-2ce20c53404f" />  

Mining menu + output information  
<img width="1128" height="186" alt="image" src="https://github.com/user-attachments/assets/13e38d94-da09-4d54-b28f-d9a950aaef4e" />  
