#include "src/headers/lib.hpp"

int main() {
    cout << "Hello, Blockchain!" << endl;
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