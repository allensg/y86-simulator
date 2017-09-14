#ifndef EXECUTE_H
#define EXECUTE_H
#define INSTR_COUNT 16
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int valC;
    unsigned int valA;
    unsigned int valB;
    unsigned int dstE;
    unsigned int dstM;
    unsigned int srcA;
    unsigned int srcB;
    
} eregister;

//prototypes for functions
void executeStage(forwardType *forward, statusType status, bubbleType * bubble);
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun,  unsigned int valC, unsigned int valA, unsigned int valB, unsigned int dstE, unsigned int dstM, unsigned int srcA, unsigned int srcB);//added for lab6
void initializeFuncPtrArray();

eregister getEregister();
void clearEregister();
#endif
