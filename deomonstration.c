#include <stdio.h>
#include <stdbool.h>
#include "instructions.h"
#include "tools.h"
#include "status.h"
#include "forwarding.h"
#include "bubbling.h"
#include "registers.h"
#include "dump.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "writebackStage.h"

//W register holds the input for the fetch stage.
//it is only accessible from this file. Its static
static wregister W;

// Function: getRegister
// Description: Returns a copy of the Register
// Params: none
// Returns: W register
// Modifies: none

wregister getWregister(){
    return W;
}

// Function: clearWregister
// Description: clears the W register
// Params: none
// Returns: none
// Modifies: Wregister
void clearWregister(){
    clearBuffer((char *) &W, sizeof(W));
    W.icode = NOP;
    W.stat = SAOK;
}


// Function: writebackStage
// Description: main of the Writeback
// Params: none
// Returns: false if something went wrong, true if no problem
// Modifies: 
bool writebackStage(forwardType * forward, statusType * status){   
    if(W.stat == SHLT) return true;
    else if(W.stat == SINS){
        printf("Invalid instruction\n");
        dumpProgramRegisters();
        dumpProcessorRegisters();
        dumpMemory();
        return true;
    } else if(W.stat == SADR){
        printf("Invalid memory address\n");
        dumpProgramRegisters();
        dumpProcessorRegisters();
        dumpMemory();
        return true;
    }
    if(W.icode == DUMP && getBits(0, 0, W.valE)) dumpProgramRegisters();
    if(W.icode == DUMP && getBits(1, 1, W.valE)) dumpProcessorRegisters();
    if(W.icode == DUMP && getBits(2, 2, W.valE)) dumpMemory(); 
    //forward->W_dstE= W.dstE; forward->W_valE = W.valE; forward->W_dstM = W.dstM; //added for lab8 forward->W_valM = W.valM; //added for lab8 forward->W_valM = W.valM; //lab9 forward->W_icode = W.icode; //lab9
    forward->W_dstE= W.dstE;
    forward->W_valE = W.valE;
    forward->W_dstM = W.dstM; 
    forward->W_valM = W.valM; 
    forward->W_icode = W.icode; //lab9
    
    setRegister(W.dstE, W.valE);
    setRegister(W.dstM, W.valM);

    status->W_stat = W.stat;
    return false;
}

// Function: updateWregister
// Description: updates the values in W register
// Returns: none
// Modifies: W.stat, W.icode, W.valM, W.dstE, W.valE, W.dstE
void updateWregister(unsigned int stat, unsigned int icode, unsigned int valE, 
                     unsigned int valM, unsigned int dstE, unsigned int dstM) {
    W.stat = stat;
    W.icode = icode;
    W.valE = valE;
    W.valM = valM;
    W.dstE = dstE;
    W.dstM = dstM;
}



