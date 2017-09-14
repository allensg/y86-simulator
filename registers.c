#include "registers.h"
#include "tools.h"

//static fregister PLACEHOLDER;
static unsigned int rList[rSize];
static unsigned int CC;


// Function: getRegister
// Description: Returns a copy of the Register
// Params:
// Returns:
// Modifies:
void clearCC(){   
        CC = 0;
}

// Function: getRegister
// Description: Returns a copy of the Register
// Params:
// Returns:
// Modifies:
unsigned int getRegister(int regNum){
    if(regNum < rSize && regNum >= 0)
        return rList[regNum];
    else 
      return 0;
}

// Function: setRegister
// // Description: sets a register to regVal
// // Params:
// // Returns:
// // Modifies:
void setRegister(int regNum, unsigned int regValue){
    if(regNum < rSize && regNum >= 0)
         rList[regNum] = regValue;
}


// Function: clearRegister
// // Description: 
// // Params:
// // Returns:
// // Modifies:
//
void clearRegisters(){
   clearBuffer((char*)rList, sizeof(rList));
}

// Function: setCC
// // Description: 
// // Params:
// // Returns:
// // Modifies:
//
void setCC(unsigned int bitNumber, unsigned int value){
    if(bitNumber == SF || bitNumber == ZF || bitNumber == OF)
        CC = assignOneBit(bitNumber, value, CC);        
}

// Function: getCC
// // Description: 
// // Params:
// // Returns:
// // Modifies:

unsigned int getCC(unsigned int bitNumber){
    if(bitNumber == SF || bitNumber == ZF || bitNumber == OF)
        return getBits(bitNumber, bitNumber, CC);
    else
        return 0; 
}
