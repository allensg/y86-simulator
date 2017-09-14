#include <stdbool.h>
#include "tools.h"
#include "status.h"
#include "forwarding.h"
#include "bubbling.h"
#include "instructions.h"
#include "registers.h"
#include "executeStage.h"
#include "memoryStage.h"


//E Register holds the input for the execute stage.
//it is only accessible from this file (static)
static eregister E;
unsigned int (*functionPtr[16])();

static unsigned int performZero(); 
static unsigned int performDump(); 
static unsigned int performOpl(statusType status); 
static unsigned int performIrmovl(); 
static bool getmBubble(statusType status); 
static unsigned int performRRMOVL(); 
static unsigned int performRMMOVL(); 
static unsigned int performMRMOVL(); 
static unsigned int performPOPL(); 
static unsigned int performPUSHL(); 
static unsigned int performCALL(); 
static unsigned int performJXX(); 
static unsigned int performRET(); 
static int computeCND(); 
static unsigned int getEVdstE(unsigned int e_CND);


// Function: getRegister
// Description: Returns a copy of the Register
// Params: none
// Returns: E register
// Modifies: none
eregister getEregister(){
    return E;
}

// Function: clearEegister
// Description: Returns a copy of the Register
// Params: none
// Returns: none
// Modifies: Clears E register
void clearEregister(){
    clearBuffer((char *) &E, sizeof(E));
    E.icode = NOP;
    E.stat = SAOK; 
}

//  Function: executeStage
//  Description: E.icode is used as n index into the
//  funtionPtr[] array. 
//  Params: forwardType * forward, statusType status, bubbleType * bubble
//  Returns: nothing
//  Modifies: e_valE and e_CND
void executeStage(forwardType * forward, statusType status, bubbleType * bubble){ 
    //bools
    bool m_bubble = getmBubble(status);

    //decide if m_stat or W_stat depending on code
    unsigned int valE = functionPtr[E.icode]();
    int e_CND = computeCND();
    unsigned int e_dstE = getEVdstE(e_CND);
    bubble->E_dstM = E.dstM;
    bubble->E_icode = E.icode;
    bubble->e_Cnd = e_CND;

    //forwarding
    forward->e_dstE = E.dstE;
    forward->e_valE = valE;
    if(m_bubble)
        updateMregister(E.stat, NOP, FNONE, FNONE, FNONE, RNONE, RNONE);
    else
        updateMregister(E.stat, E.icode, e_CND, valE, E.valA, e_dstE, E.dstM);
}


//  Function: initalizeFuncPtrArray
//  Description: sets all fields of the array to 0 and calls function
//  Params: none
//  Returns: none
//  Modifies: functionPtr[]
void initializeFuncPtrArray(){   
    int i;
    for(i = 0; i < 16; i++) functionPtr[i] = performZero;
    
    functionPtr[DUMP]= performDump;
    functionPtr[OPL] = performOpl;
    functionPtr[IRMOVL] = performIrmovl;
    functionPtr[RRMOVL] = performRRMOVL;
    functionPtr[RMMOVL] = performRMMOVL;
    functionPtr[MRMOVL] = performMRMOVL;
    functionPtr[POPL] = performPOPL;
    functionPtr[PUSHL] = performPUSHL;
    functionPtr[CALL] = performCALL;
    functionPtr[JXX] = performJXX;
    functionPtr[RET] = performRET;
}

//  Function: getmBubble
//  Description: decides if m needs a bubble or not
//  Params: statusType status
//  Returns: true if it does need a bubble. false otherwise
//  Modifies: nothing
bool getmBubble(statusType status){
    if((status.m_stat == SADR || status.m_stat == SINS || status.m_stat == SHLT) ||
       (status.W_stat == SADR || status.W_stat == SINS || status.W_stat == SHLT))
        return true;
    return false;
}

//  Function: computeCND()
//  Description: computes the condition code 
//  Params: none
//  Returns: e_CND
//  Modifies: none
int computeCND(){
    int e_CND = 0;
    int sf = getCC(SF);
    int zf = getCC(ZF);
    int of = getCC(OF);
   
    if(E.icode != RRMOVL && E.icode != JXX) return 0;
    
    if(E.ifun == 0){
       e_CND = 1;
    } else if(E.ifun == 1){
       if((sf ^ of) || zf) e_CND = 1;
    } else if(E.ifun == 2){
       if(sf ^ of) e_CND = 1;
    } else if(E.ifun == 3){
       if(zf) e_CND = 1;
    } else if(E.ifun == 4){
       if(!zf) e_CND = 1;
    } else if(E.ifun == 5){
       if(!(sf^of)) e_CND = 1;
    } else if(E.ifun == 6){
       if(!(sf^of) && !zf) e_CND = 1;
    } else e_CND = 0;
    
    return e_CND;
}

//  Function: performOpl
//  Description: performs the instruction
//  Params: statusType status
//  Returns: result (e_valE)
//  Modifies: set_cc and result (local)
unsigned int performOpl(statusType status){
    bool set_cc = true;
    if(status.m_stat == SADR || status.m_stat == SINS || status.m_stat == SHLT ||
        status.W_stat == SADR || status.W_stat == SINS || status.W_stat == SHLT)
       set_cc = false; 
    
    int a = (int) E.valA; 
    int b = (int) E.valB;
    int result = 0;

    if(set_cc) clearCC(); //The CC register is cleared before every OPL
    if(E.ifun == 0 && set_cc){ //addl
        result = b + a;
    
        if(result == 0 && set_cc) setCC(ZF, 1);
        if(result < 0 && set_cc)  setCC(SF, 1);
        if(((a < 0) == (b < 0)) && ((result < 0) != (a < 0)) && set_cc)
            setCC(OF, 1);
    } else if(E.ifun == 1 && set_cc){ //subl
        result = b - a;
        
        if(result == 0 && set_cc) setCC(ZF, 1);
        if(result < 0 && set_cc)  setCC(SF, 1);
        if((((a > 0) != (b > 0))) && ((result > 0) == (a > 0)) && set_cc)   
            setCC(OF, 1);

    } else if(E.ifun == 2 && set_cc){ //andl
        result = b & a;

        if(result == 0 && set_cc) setCC(ZF, 1);
        if(result < 0 && set_cc)  setCC(SF, 1);

    } else { //xorl
        result = b ^ a;

        if(result == 0 && set_cc) setCC(ZF, 1);
        if(result < 0 && set_cc)  setCC(SF, 1);
    } 
    if(!set_cc){
        setCC(ZF, 0);
        setCC(SF, 0);
        setCC(OF, 0);
    } 
    return result;
}


//  Function: performIrMovl
//  Description: simulates a IrMovl
//  Params: none
//  Returns: E.valC
//  Modifies: none
unsigned int performIrmovl(){
    return E.valC;
}

//  Function: performDump
//  Description: simulates Dump
//  Params: none
//  Returns: E.valC
//  Modifies: none
unsigned int performDump(){
    return E.valC;
}

//  Function: performZero()
//  Description: just returns 0
//  Params: none
//  Returns: zero
//  Modifies: none
unsigned int performZero(){
    return 0;
}

//  Function: perFormRMMOVL()
//  Description: simulates RMMOVL, adds E.valB + E.valC
//  Params: none
//  Returns: E.valB + E.valC
//  Modifies: none
unsigned int performRMMOVL(){
    return E.valB + E.valC;
}

//  Function: performRRMOVL()
//  Description: simulates RRMOVL
//  Params: none
//  Returns: E.valA
//  Modifies: none
unsigned int performRRMOVL(){
    return E.valA;
}

//  Function: performMRMOVL()
//  Description: adds E.valB + E.valC
//  Params: none
//  Returns: E.valB + E.valC
//  Modifies: none
unsigned int performMRMOVL(){
    return E.valB + E.valC;
}

//  Function: performPOPL()
//  Description: calcs the address for a popl
//  Params: none
//  Returns: E.valB + 4
//  Modifies: none
unsigned int performPOPL(){
    return E.valB + 4;
}

//  Function: performPUSHL()
//  Description: Opposite of POPL, decrement for PUSHL
//  Params: none
//  Returns: E.valB + 4
//  Modifies: none
unsigned int performPUSHL(){
    return E.valB - 4;
}

//  Function: performJXX()
//  Description: prepare for jump 
//  Params: none
//  Returns: 0 
//  Modifies: none
unsigned int performJXX(){
    return 0;
}

//  Function: performCALL)
//  Description: decrement E.valB
//  Params: none
//  Returns: E.valB - 4
//  Modifies: none
unsigned int performCALL(){
    return E.valB - 4;
}

//  Function: performRET()
//  Description: increments E.valB+4 
//  Params: none
//  Returns: E.valB + 4
//  Modifies: none
unsigned int performRET(){
    return E.valB + 4;
}

//  Function: getEVdstE
//  Description: calc the reg for dstE depending on e_CND
//  Params: unsigned int e_CND
//  Returns: RNONE depending on E.icode or E.dstE
//  Modifies: none
unsigned int getEVdstE(unsigned int e_CND){
    if(E.icode == RRMOVL && !e_CND) return RNONE;
    return E.dstE;
}


//  Function: updateEregister
//  Description: uppdates E register with the proper values
//  Params: unsigned int stat, icode, valC, valA, valV, dstE
//                   dstM, srcA.
//  Returns: zero
//  Modifies: E.stat, E.icode, E.valC. E.valA. E.valB, E.dstE
//            E.dstM, E.srcA
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun,
                     unsigned int valC, unsigned int valA, unsigned int valB, 
                                          unsigned int dstE, unsigned int dstM, unsigned int srcA, 
                                                               unsigned int srcB){
    E.stat = stat;
    E.icode = icode;
    E.ifun = ifun;
    E.valC = valC;
    E.valA = valA;
    E.valB = valB;
    E.dstE = dstE;
    E.dstM = dstM;
    E.srcA = srcA;
    E.srcB = srcB;
}

