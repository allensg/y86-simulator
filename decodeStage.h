#ifndef DECODESTAGE_H
#define DECODESTAGE_H

typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int rA;
    unsigned int rB;
    unsigned int valC;
    unsigned int valP;
} dregister;


//prototypes go here
dregister getDregister();
void updateDregister( unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP);
void clearDregister();
void decodeStage(forwardType forward, bubbleType * bubble);


//might not be needed yet -josh
//might need to be unsigned...
/*
int getSrcA();
int getSrcB();
int getDstM();
int getDstE();
*/
#endif
