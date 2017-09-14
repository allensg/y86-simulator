#include <stdio.h>
#include <stdbool.h>
#include "tools.h"

/* 
   Replace the return 0 with the code to perform the required function.
   Do not change the function prototypes.
   Be sure to check for errors in input. 
   Document EACH function with a function header (description, parameters,
   return).
*/

unsigned int getBits(int low, int high, unsigned int source){
    if(low > high)
    {
        return 0;
    }

    source = source << (31 - high);
    source = source >> (low + (31 - high));
    return source;
} 

unsigned int setBits(int low, int high, unsigned int source){
    unsigned int notOne = -1;
    notOne = getBits(low, high, notOne);
    notOne = notOne << low;
    source = source | notOne;
    return source;
}

unsigned int clearBits(int low, int high, unsigned int source){   
    unsigned int notOne = -1;
    notOne = getBits(low, high, notOne);
    notOne = notOne << low;
    source = source & ~notOne;
    return source;
}

unsigned int assignOneBit(int bitNumber, int bitValue, unsigned int source){   
    if(bitValue == 0){
        return clearBits(bitNumber, bitValue, source);
    } else if(bitValue == 1){
        return setBits(bitNumber, bitNumber, source);
    } else return 0;
}

unsigned char getByteNumber(int byteNo, unsigned int source){
    int lower = byteNo * 8;
    int higher = lower + 7;
    return getBits(lower, higher, source);
}

unsigned int putByteNumber(int byteNo, unsigned char byteValue, 
                           unsigned int source){
    int low = byteNo * 8;
    int high = low + 7;
    source = clearBits(low, high, source);
    unsigned int aByte = byteValue;
    aByte = aByte << low;
    return (source | aByte);
}

unsigned int buildWord(unsigned char byte0, unsigned char byte1, 
                       unsigned char byte2, unsigned char byte3){   
    unsigned int word = 0;
    word = putByteNumber(0, byte0, word);
    word = putByteNumber(1, byte1, word);
    word = putByteNumber(2, byte2, word);
    word = putByteNumber(3, byte3, word);
    return word;
}

/*bool isNegative(unsigned int source)
{
    return getBits(31,31,source) == 1;   
}
*/
void expandBits(unsigned int source, char bits[36]){
    bits[35] = 0;

    int i;

    for(i = 34; i >=0; i--){
        if (i == 26 || i == 17 || i == 8){
            bits[i] = 32;
        } else {
            if(source % 2)
                bits[i] = 49;
            else
                bits[i] = 48;

          source = source >> 1;
        }
    }
}

void clearBuffer(char * buff, int size){
    unsigned int i;

    for(i = 0; i < size; i++){
        *buff = 0;
        buff++;
    }
}


