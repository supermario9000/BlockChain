#ifndef HASH_FUNCTION_HPP
#define HASH_FUNCTION_HPP

#include "lib.hpp"

//hash helper functions below
void hashFunkcija(string input, string &hash){

    vector<int> nuskaityti_binary_duomenys;
    vector<int> konvertuota_ivestis;
    vector<int> konvertuota_druskyte;
    int suma;

    string salt = druskyte(input);
    input += salt;
    KonvertCharIx10(input, konvertuota_ivestis);
    KonvertCharIx10(salt, konvertuota_druskyte);
    suma = Sumax10(konvertuota_ivestis);
    nuskaitytimp3(nuskaityti_binary_duomenys);

    int skyriu_sk = nuskaityti_binary_duomenys.size() / 32;
    int pradinis_skyrius = suma;
    while (pradinis_skyrius > skyriu_sk) pradinis_skyrius -= skyriu_sk;

    //pati hash funkcija
    for(int i=0; i<32; i++)
    {
        hash += desimtaine_i_16(konvertuota_ivestis[i] ^ nuskaityti_binary_duomenys[pradinis_skyrius * 32 + i] ^ konvertuota_druskyte[i]);
    }
    int i=0;
    while (hash.length()<64) 
    {
        hash += desimtaine_i_16(konvertuota_ivestis[i] ^ nuskaityti_binary_duomenys[pradinis_skyrius * (32 + i)] ^ konvertuota_druskyte[i]);
    }
    if(hash.length()>64) hash = hash.substr(0,64);
    
    nuskaityti_binary_duomenys.clear();
    konvertuota_ivestis.clear();
    konvertuota_druskyte.clear();
}

//these functions are taken from my earlier project which was creating a hash function. Optimized for this project.
inline string desimtaine_i_16(int desimtaine){
    string hex = "";
    int liekana;
    char simbolis;
    while(desimtaine != 0){
        if(desimtaine <0 ) desimtaine = (-1) * desimtaine; //jei ivedami lietuviški simboliai, jų reikšmė būna minusinė. Tai trikdo veiklai.
        liekana = desimtaine % 16;
        if (liekana < 10) simbolis = liekana + '0';
        else simbolis = liekana - 10 + 'A';
        hex = simbolis + hex;
        desimtaine /= 16;
    }
    return hex;
}

inline string druskyte (string input){
    std::seed_seq seed(input.begin(), input.end());
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, 15);
    string result;
    for (int i = 0; i < 64; ++i) {
        int val = dist(rng);
        result += "0123456789ABCDEF"[val];//"0123456789ABCDEF" is a string containing all hexadecimal digits. [val] selects one based on the random value.
    }
    return result;
}

void nuskaitytimp3(vector<int> &nuskaityti_binary_duomenys){
    ifstream in("../irasas.mp3", ios::binary); //binary input output flag
    if(!in){
        cout<<"Failas nerastas"<<endl;
        return;
    }
    else 
    {
        for(char byte; in.get(byte);)
        {
            if(byte != '\0') nuskaityti_binary_duomenys.push_back((unsigned char)byte);
        }
    }
    in.close();
}

void KonvertCharIx10 (string input, vector<int> &konvertuota_ivestis){
    char simbolis;
    for(int i=0; i<=input.length(); i++)
    {
        simbolis = input[i]; //null becomes 0, then 0 is 48
        konvertuota_ivestis.push_back((int)simbolis);
    }
}

int Sumax10(vector<int> &konvertuota_ivestis){ //calculating the sum of all values as the "sector" for the read file
    int suma = 0;
    for(int i=0; i<konvertuota_ivestis.size(); i++){
        suma += konvertuota_ivestis[i];
        if (suma<0) suma *= -1;
    }
    return suma;
}

void clearData(string &input, string &hash, string &salt, int &suma, vector<int> &konvertuota_ivestis, vector<int> &konvertuota_druskyte){
    input.clear();
    hash.clear();
    salt.clear();
    suma=0;
    konvertuota_ivestis.clear();
    konvertuota_druskyte.clear();
}

#endif // HASH_FUNCTION_HPP
