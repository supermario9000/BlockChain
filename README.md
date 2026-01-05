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
