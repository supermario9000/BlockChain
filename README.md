# Hash function header

## Description ##

This is my version of a hash function. The release link: https://github.com/supermario9000/BlockChain/releases/tag/hash_v1 .  
It must always be paired with an audio file to work correctly. The audio file should be in a parent directory (or you may configure it in the source code).  
All of the helper functions are included in the header and are ready to use.  

You may always use your own audio file for hashing.  
Requirements and notes of custom audio use:  
* The audio file must be in .mp3 format as I have not implemented other solutions.  
* Take note of audio length. The example audio "irasas.mp3" is around 2-3 seconds long and that makes it about 100,000 symbols (bytes) in size. After removing all null characters the audio returns about 2500-3000 sections (sections explained below). The numbers aren't exact, because I tested this long ago, but they should be around that range.  
* What you should take into account is that using longer audio files will certainly return more diverse and secure hashes, however at the cost of performance, as iterating and reading the audio would take longer.  

### How does it work? ###  

The hash function takes two parameters - input and hash.  
The hash parameter has to be given as an empty string, otherwise the data in the hash string will persist.  


This is a step-by-step explanation of my idea:  
1. The audio file gets read in binary format (this is done for special character handling).  
2. Salt is randomly generated taking input as the seed. This is done using mt19937_64, uniform_int_distribution.  
3. The input is extended with the newly generated salt.  
4. The input and the salt are separately converted to an integer array (from char[]).  
5. A sum variable is introduced to count the sum of all values of the newly converted input array.  
6. The audio is then cut into 32 byte pieces called "skyrius" (Lithuanian for "section") and the total number of such sections is counted.  
7. The sum _(mentioned in 5.)_ acts as a selector for the section (sum = section index). If the sum is bigger than the total number of sections, the sections get looped again.  
8. The main hash begins (first cycle):  
   8.1. The program takes the selected cutout section of the audio, converted input and converted salt and redeclares them as a, b and c (at this point all of these variables are int format).  
   8.2. We perform two XOR operations (a^b^c).  
   8.3. The result of these operations is converted into hexadecimal format and appends the hash string.  
9. The second cycle of hashing runs only if the hash hasn't reached 64 symbol size. The only difference - the selected audio section index is changed.  
10. If the hash is too long after these operations, it is shrunk to the first 64 symbols.  
11. The hash is returned.  
