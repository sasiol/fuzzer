//store file to memory
//flip, insert delete bytes
#include "mutator.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>


// Mutate the data (flip random bytes)   
// todo: deterministic mode? (examp afl)
void mutate(std::vector<unsigned char>& data) {
    if (data.empty()) return;

    int numMutations = rand() % 5 + 1; // change to use random next?

    for (int i = 0; i < numMutations; i++) {
        int x = rand() % 4;

        int index = rand() % data.size();   // pick random position

        //to add : interesting values, other flips?
        switch(x){
            case 0: //flip bit
                data[index] ^= (1 << (rand()% 8));
                break;
            
            case 1:  //flip a byte
                 data[index] ^= 0xFF; 
                 break;

            case 2: //add byte
                data.insert(data.begin() + index, rand() % 256);
                break;

            case 3: //delete byte
                if (data.size() > 1)
                data.erase(data.begin()+index);
                break;
        }
       
    }
}


