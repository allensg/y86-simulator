#include <stdbool.h>
#include "tools.h"
#include "instructions.h"
#include "status.h"
#include "forwarding.h"
#include "bubbling.h"
#include "memory.h"
#include "registers.h"
#include "memoryStage.h"
#include "writebackStage.h"

static mregister M;



static int memAddr();
static bool memWrite();
static bool memRead();
static int getMStat(bool memError);
static bool checkWStall(statusType * s);

// Function: getMregister
// Description: Returns a copy of the Register
// Params: none
// Returns: M register
// Modifies: none
mregister getMregister(){
    return M;
}
    
// Function: memoryStage
// Description: main of memoryStage, controls everything
// Params: pointer of type forwardType, and pointer of statusType
// Returns: none
// Modifies: M_stat, M.dstM, m_valM, M.dstE, M.valE.
// also updates Wregister       
void memoryStage(forwardType * forward, statusType * status, bubbleType * bubble){
    unsigned int m_valM = 0;
    int addr = memAddr();
    bool memError = false;

    if(memRead()){
        m_valM = getWord(addr, &memError);
    } if(memWrite()){
        if(addr == M.valE)
            putWord(addr, M.valA, &memError);    
        else
            putWord(addr, M.valE, &memError);
    }
   
   int m_stat = getMStat(memError);

   //status
   status-> m_stat = m_stat;
   
   //forwarding
   forward->M_dstM = M.dstM;
   forward->m_valM = m_valM;
   forward->M_dstE = M.dstE;
   forward->M_valE = M.valE;


   forward->M_icode = M.icode;
   forward->M_valA = M.valA;
   forward->M_Cnd = M.Cnd;
   
   bubble->M_icode = M.icode; 
   //updateWregister(M.stat, M.icode, M.valE, m_valM, M.dstE, M.dstM);
   if(!checkWStall(status)) updateWregister(m_stat, M.icode, M.valE, m_valM, M.dstE, M.dstM);
}

// Function: memoryStage
// Description: decides if mem should be read
// Params: none
// Returns: true if a read is required, false if not
// Modifies: none
bool checkWStall(statusType * s){
   statusType status = *s;
   if(status.W_stat == SADR || status.W_stat == SINS || status.W_stat == SHLT)
      return true; 

   return false;
}

// Function: memoryStage
// Description: decides if mem should be read
// Params: none
// Returns: true if a read is required, false if not
// Modifies: none
bool memRead(){
    if(M.icode == MRMOVL || M.icode == POPL || M.icode == RET)
        return true;
    else
        return false;
}

// Function: memWrite
// Description: decides if mem should be written too
// Params: none
// Returns: true if a write is required
// Modifies: nothing
bool memWrite(){
    if(M.icode == RMMOVL || M.icode == PUSHL || M.icode == CALL)
        return true;
    else
        return false;
}

// Function: memAddr
// Description: denpending on the icode either valE or valA is choosen
// Params: none
// Returns: blah (either valA or valE)
// Modifies: none
int memAddr(){
    int toReturn;
    if(M.icode == RMMOVL || M.icode == PUSHL || M.icode == CALL || M.icode == MRMOVL)
        toReturn = M.valE;
    else if(M.icode == POPL || M.icode == RET)
        toReturn = M.valA;
    return toReturn; 
}

// Function: updateMregister
// Description: updates M register with the proper values
// Params: unsigned int stat, icode, Cnd, valE, valA, dstE, dstM
// Returns: none
// Modifies: M.stat, M.icode, M.Cnd, M.valE, M.valA, M.dstE, M.dstM
void updateMregister(unsigned int stat, unsigned int icode, unsigned int Cnd,
                     unsigned int valE, unsigned int valA, unsigned int dstE, 
                     unsigned int dstM){
    M.stat = stat;
    M.icode = icode;
    M.Cnd = Cnd;
    M.valE = valE;
    M.valA = valA;
    M.dstE = dstE;
    M.dstM = dstM;    
}

// Function: getMStat
// Description: gets m stat!
// Params: memError
// Returns: SADR if there is an memError, M.stat if there isnt
// Modifies: none
int getMStat(bool memError){
    if(memError)
        return SADR;
    else
        return M.stat;
}


// Function: clearMregister
// Description: clears the M register
// Params: none
// Returns: none
// Modifies: Mregister
void clearMregister(){
    clearBuffer((char*) &M, sizeof(M));
    M.icode = NOP;
    M.stat = 1; 
}
