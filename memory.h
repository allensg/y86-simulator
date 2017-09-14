#define MEMSIZE 1024     //1024 words of memory
#ifndef MEMORY_H
#define MEMORY_H
unsigned char getByte(int address, bool * memError);
void putByte(int address, unsigned char value, bool * memError);
void clearMemory();
unsigned int getWord(int address, bool * memError);
void putWord(int address, unsigned int value, bool * memError);
unsigned int makeWord(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
//void store(int address, unsigned int value, bool * memError);
//unsigned int fetch(int address, bool * memError);
#endif 

