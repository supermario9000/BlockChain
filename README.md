# Hyperledger fabric

## Contents:

1. Installation and basic commands.
2. _Smart contract modification._
3. _New smart contract deployment._
4. Full task formulation (lithuanian)

## 1. Installation and basic commands

For the installation, i used the latest official hyperledger fabric documentation.  
It can be found here: https://hyperledger-fabric.readthedocs.io/en/latest  

And the biggest help was the installation documentation in 13-14 paskaitų skaidrės document.  

This is the step by step of the prerequisites installation:  
1. Install git (i already had this before the project started)
2. Install docker (i already had this before the project started)
3. Install curl (i already had this before the project started)
4. Install npm and node (i already had this before the project started)
5. Install WSL2 with ubuntu (i already had this before the project started)
6. Install jq. I did this in WSL with the _"sudo apt install jq"_ command: <img width="555" height="49" alt="image" src="https://github.com/user-attachments/assets/e7284880-9a00-4571-ab73-4ed6ff778f98" />
7. I didn't install Go because I thought it wasn't needed (I will be coding in JS), but it is the recommended option.


Step by step installation of hyperledger fabric:
1. I made the recommended directory for it: <img width="761" height="71" alt="image" src="https://github.com/user-attachments/assets/ede55c19-46aa-4d86-9876-bf9e85b27d00" />
2. I ran the install script with these commands: <img width="1453" height="70" alt="image" src="https://github.com/user-attachments/assets/f84f5b41-af79-42ee-9086-17d4a0e03341" />
   These are the results i got:  
   <img width="868" height="189" alt="image" src="https://github.com/user-attachments/assets/58452853-d052-4fd3-a91a-f88a701c2915" />  
   That was happening because docker was not running, after i started the program and it detected my WSL, everything went fine:  
   <img width="776" height="48" alt="image" src="https://github.com/user-attachments/assets/f4d871ec-7633-44e5-a70f-ce2b694bb1fb" />  
   So after i ran the install script, this is what i got:  
   <img width="214" height="101" alt="image" src="https://github.com/user-attachments/assets/570a753f-fe88-47e3-b4d8-cf1e5ddf1f5c" />
3. I deployed the test network.  
   <img width="1468" height="22" alt="image" src="https://github.com/user-attachments/assets/c07d6cf6-4ccf-4661-b298-8c599023c504" />  
   <img width="1435" height="202" alt="image" src="https://github.com/user-attachments/assets/6b1530ed-ecb8-4ee7-8530-55bcc63df9e9" />
4. Installation is finished.


Using commands and showing the results:
* with the command _"./network.sh createChannel -c mif2026"_ i created a channel called mif2026 between the 2 organizations in the sample program:
  <img width="443" height="208" alt="image" src="https://github.com/user-attachments/assets/8fe94bd3-2910-42a1-b49a-7e4231339082" />
* with the command _"./monitordocker.sh fabric_test"_ i set up a Logspout in another terminal for debugging:
  <img width="1368" height="250" alt="image" src="https://github.com/user-attachments/assets/c4e6dee4-5739-4002-b84c-37f33786ceb7" />
* with the command _"./network.sh deployCC -c mif2026 -ccn basic -ccp ../asset-transfer-basic/chaincode-javascript -ccl javascript "_ i installed a chaincode (a chain of smart contracts stored in peers) on my channel mif2026:  
  <img width="896" height="419" alt="image" src="https://github.com/user-attachments/assets/4e9850e7-3a20-4d6b-9fc6-fb67cb026a38" />
* with a chain of commands i comnfigured the environment to operate the peer cli as Org1 (this can be set again to operate as Org2):  
  <img width="1454" height="468" alt="image" src="https://github.com/user-attachments/assets/001972f3-9648-45ca-b192-36d8e3fdb78c" />
* with a chain of commands i initialized a ledger with assets on my mif2026 channel:
  <img width="1461" height="389" alt="image" src="https://github.com/user-attachments/assets/a01521cb-a854-4409-8913-5c001b23e10d" />
* with the command _"peer chaincode query -C mif2026 -n basic -c '{"Args":["GetAllAssets"]}'"_ i queried my ledger and got these results:
  <img width="1448" height="160" alt="image" src="https://github.com/user-attachments/assets/af7b6b91-6d7f-4c8b-8af7-dfc56a169789" />
* with a chain of commands i transfered an asset from one member to another:  
  <img width="1459" height="384" alt="image" src="https://github.com/user-attachments/assets/7af6bf5d-5a98-4838-ad00-2ae53945d045" />
  If we run the previous command, we can see the change: 
  <img width="1452" height="171" alt="image" src="https://github.com/user-attachments/assets/81dc8e78-843c-4572-beaa-54c9ebf68cc4" />
* To bring down the network, I would just need to use _"./network.sh down"_.


Bringing up the network with Certificate Authorities and CouchDB (for production):
1. With the command _"./network.sh up createChannel -ca -c mychannel -s couchdb"_ I started a network and deployed a channel with CA and CouchDB.
2. With the command _"./network.sh deployCC -ccn basic -ccp ../asset-transfer-basic/chaincode-javascript -ccl javascript"_ I started a chaincode on the channel.
3. I configured the environment to exetute peer1.
4. Initialized the ledger with assets.
5. Tested the application state in docker desktop:
   <img width="1490" height="445" alt="image" src="https://github.com/user-attachments/assets/df60c4fe-0b23-411a-bf2e-2fb5fe0f2482" />
6. Opened CouchDB with http://127.0.0.1:5984/_utils/#login user: admin pass: adminpw.
   <img width="1153" height="444" alt="image" src="https://github.com/user-attachments/assets/b9b9f551-bd2f-49a9-937c-bef7c096a893" />













## 4. Full task formulation

1. Hyperledger Fabric diegimo ir komandų atkartojimas (praktiškai ta pati užduotis kurią jau buvo galima atsiskaityti paskutinės gruodžio paskaitos metu)
Atkartokite „Paskaita 13-14. Installing and using Hyperledger Fabric“ pateiktas Hyperledger Fabric tinklo su CA ir CouchDB diegimo, kanalo išmaniosios sutarties paleidimo bei transakcijų vykdymo (invoke ir query) komandas.
Paruoškite trumpą aprašymą, akcentuodami, jei susidūrėte su problemomis, ir paaiškinkite, kaip jas išsprendėte. Pridėkite ekrano kopijas, rodančias sėkmingą procesą.
Užduoties įvertinimas: iki 0,75 balo.  
2. Išmaniosios sutarties modifikavimas
Remdamiesi AssetTransfer išmaniosios sutarties (AssetTransfer.{go, java, ts, js}) pavyzdžiu, modifikuokite išmaniąją sutartį pridėdami dvi savo sugalvotas funkcijas šalia jau egzistuojančių.
Egzamino metu pademonstruokite, kaip šiomis funkcijomis naudotis.
Užduoties įvertinimas: iki 0,75 balo.  
3. Naujos išmaniosios sutarties kūrimas ir demonstravimas
Sukurkite naują išmaniąją sutartį (chaincode), skirtą kitai taikymo sričiai (ne turto registrui).
Paleiskite ją Hyperledger-Fabric lokaliame tinkle (Kaip pvz., žr.: https://hyperledger-fabric.readthedocs.io/en/latest/deploy_chaincode.html)  
Egzamino metu pademonstruokite tinklo veikimą: įvykdykite „query“ ir „invoke“ transakcijas, bei parodykite, kaip informacija keičiasi duomenų bazėje.
Užduoties įvertinimas: iki 1,5 balo.
