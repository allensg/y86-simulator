#ifndef MEMSTAGE_H
#define MEMSTAGE_H

typedef struct{
        unsigned int stat;
        unsigned int icode;
        unsigned int Cnd;
        unsigned int valE;
        unsigned int valA;
        unsigned int dstE;
        unsigned int dstM;
} mregister;

void memoryStage(forwardType * forward, statusType * status, bubbleType * bubble);
void updateMregister(unsigned int stat, unsigned int icode, unsigned int Cnd,
                     unsigned int valE, unsigned int valA, unsigned int dstE, 
                                          unsigned int dstM);

mregister getMregister();
void clearMregister();
//bool memRead();
//bool memWrite();
//int memAddr();
#endif

