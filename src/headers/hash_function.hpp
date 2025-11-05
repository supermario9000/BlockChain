#ifndef HASH_FUNCTION_HPP
#define HASH_FUNCTION_HPP

#include "lib.hpp"

//prototypes of helper functions used in the hash function
inline string desimtaine_i_16(int desimtaine);
inline string druskyte (string input);
inline const vector<int>& getMp3Bytes();
inline void KonvertCharIx10 (const string &input, vector<int> &konvertuota_ivestis);
inline int Sumax10(const vector<int> &konvertuota_ivestis);
inline void clearData(string &input, string &hash, string &salt, int &suma, vector<int> &konvertuota_ivestis, vector<int> &konvertuota_druskyte);

//hash helper functions below
inline void hashFunkcija(string input, string &hash){

    const vector<int> &nuskaityti_binary_duomenys = getMp3Bytes();
    vector<int> konvertuota_ivestis;
    vector<int> konvertuota_druskyte;
    int suma;

    string salt = druskyte(input);
    input += salt;
    KonvertCharIx10(input, konvertuota_ivestis);
    KonvertCharIx10(salt, konvertuota_druskyte);
    suma = Sumax10(konvertuota_ivestis);
    // mp3 bytes loaded into nuskaityti_binary_duomenys via getMp3Bytes()

    int skyriu_sk = nuskaityti_binary_duomenys.size() / 32;
    int pradinis_skyrius = suma;
    while (pradinis_skyrius > skyriu_sk) pradinis_skyrius -= skyriu_sk;

    // primary pass: produce up to 32 bytes (64 hex chars). Guard indexes to avoid UB.
    for (size_t i = 0; i < 32; ++i) {
        size_t mp3_idx = static_cast<size_t>(pradinis_skyrius) * 32 + i;
        int a = 0, b = 0, c = 0;
        if (i < konvertuota_ivestis.size()) a = konvertuota_ivestis[i];
        if (mp3_idx < nuskaityti_binary_duomenys.size()) b = nuskaityti_binary_duomenys[mp3_idx];
        if (i < konvertuota_druskyte.size()) c = konvertuota_druskyte[i];
        hash += desimtaine_i_16(a ^ b ^ c);
    }

    // secondary pass: fill until 64 hex chars. Increment index to avoid infinite loop and use safe modulo indexing.
    size_t ii = 0;
    while (hash.length() < 64) {
        if (konvertuota_ivestis.empty() || nuskaityti_binary_duomenys.empty() || konvertuota_druskyte.empty()) {
            hash += "00"; // fallback byte
        } else {
            size_t a_idx = ii % konvertuota_ivestis.size();
            size_t b_idx = (static_cast<size_t>(pradinis_skyrius) * 32 + (ii % 32)) % nuskaityti_binary_duomenys.size();
            size_t c_idx = ii % konvertuota_druskyte.size();
            int a = konvertuota_ivestis[a_idx];
            int b = nuskaityti_binary_duomenys[b_idx];
            int c = konvertuota_druskyte[c_idx];
            hash += desimtaine_i_16(a ^ b ^ c);
        }
        ++ii;
    }
    if (hash.length() > 64) hash = hash.substr(0, 64);

    // cached data left intact for future calls
    konvertuota_ivestis.clear();
    konvertuota_druskyte.clear();
}

//these functions are taken from my earlier project which was creating a hash function. Optimized for this project.
inline string desimtaine_i_16(int desimtaine){
    // Always produce two hex digits for a byte (00..FF).
    unsigned int v = static_cast<unsigned int>(desimtaine) & 0xFFu;
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << v;
    return oss.str();
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

// Return mp3 bytes cached (read from disk only once). Subsequent calls return the same vector.
inline const vector<int>& getMp3Bytes() {
    static vector<int> cachedBytes;
    if (!cachedBytes.empty()) return cachedBytes;

    ifstream in("./irasas.mp3", ios::binary);
    if (!in) {
        cout << "Failas nerastas" << endl;
        return cachedBytes; // empty
    }
    for (char byte; in.get(byte);) {
        if (byte != '\0') cachedBytes.push_back(static_cast<unsigned char>(byte));
    }
    // leave file closed by destructor
    return cachedBytes;
}

inline void KonvertCharIx10 (const string &input, vector<int> &konvertuota_ivestis){
    konvertuota_ivestis.clear();
    konvertuota_ivestis.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        // cast to unsigned char first to avoid negative values for non-ASCII
        konvertuota_ivestis.push_back(static_cast<unsigned char>(input[i]));
    }
}

inline int Sumax10(const vector<int> &konvertuota_ivestis){ //calculating the sum of all values as the "sector" for the read file
    long long suma = 0;
    for (size_t i = 0; i < konvertuota_ivestis.size(); ++i) {
        suma += konvertuota_ivestis[i];
        if (suma < 0) suma = -suma;
    }
    // clamp to int range if necessary
    if (suma > std::numeric_limits<int>::max()) return std::numeric_limits<int>::max();
    return static_cast<int>(suma);
}

inline void clearData(string &input, string &hash, string &salt, int &suma, vector<int> &konvertuota_ivestis, vector<int> &konvertuota_druskyte){
    input.clear();
    hash.clear();
    salt.clear();
    suma=0;
    konvertuota_ivestis.clear();
    konvertuota_druskyte.clear();
}

#endif // HASH_FUNCTION_HPP
