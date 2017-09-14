#include <stdbool.h>
#include "memory.h"
#include "tools.h"
static unsigned int memory[MEMSIZE]; 

static void store(int address, unsigned int value, bool * memError);
static unsigned int fetch(int address, bool * memError);

unsigned int fetch(int address, bool * memError){
    if(address > MEMSIZE || address < 0){
        (*memError) = 1; 
         return 0;
    } else{
        (*memError) = 0;
        return memory[address];
    }
}

void store(int address, unsigned int value, bool * memError){
    if(address > MEMSIZE || address < 0){
        (*memError) = 1; 
    } else{
        memory[address] = value;
        (*memError) = 0;
    }
}

unsigned char getByte(int address, bool * memError){
        unsigned int w = fetch((address/4), memError);
        unsigned int j = getBits(0,1,address);
        return getByteNumber(j,w);
}

void putByte(int address, unsigned char value, bool * memError){
       if(address >= 0 || address < MEMSIZE){
            unsigned int w = fetch((address/4), memError);
            int j = getBits(0,1,address);
            w= putByteNumber(j, value, w); 
            store(address/4, w, memError);
       }
       else (*memError) = 1;
}

unsigned int getWord(int address, bool * memError){
    if(address > MEMSIZE || address < 0 || address%4 != 0) {
        (*memError) = 1;
        return 0;
    }
    (*memError) = 0;
    return fetch(address/4, memError);
}

void putWord(int address, unsigned int value, bool * memError){
    if(address > MEMSIZE || address < 0 || address%4 != 0) {
        (*memError) = 1;
    } else{
        store(address/4,value,memError);
        (*memError) = 0;
    }
}

void clearMemory(){
        clearBuffer((char*) memory, 4096);
}

unsigned int makeWord(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3){
    unsigned int w1 = (b1 << 8); 
    unsigned int w2 = (b2 << 16); 
    unsigned int w3 = (b3 << 24); 
    return b0 | w1 | w2 | w3;
}
