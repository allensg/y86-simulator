#include <stdbool.h>
#include "tools.h"
#include "memory.h"
#include "status.h"
#include "instructions.h"
#include "forwarding.h"
#include "registers.h"
#include "fetchStage.h"
#include "decodeStage.h"

static fregister F;
static bool instrValid(unsigned int f_icode);
static bool needRegId(unsigned int f_icode);
static bool needValC(unsigned int f_icode);
static unsigned int pcIncrement(unsigned int pc, bool needValC, bool needRegIds);
static unsigned int predictPC(unsigned int icode, unsigned int valC, unsigned int valP);
static int getValC(int f_pc, bool needRId, bool needValC);
int getStat(unsigned int f_icode, bool memError, bool valid_icode);
static bool checkFStall(bubbleType bubble); 
static bool checkDStall(bubbleType bubble); 
static bool checkDBubble(bubbleType bubble); 



// Function: fetchStage()
// Description: fetches the instruction from the .yo file and sends it 
//              into the pipeline 
// Params: forwardType forward
//         bubbleType bubble
// Returns: nothing
// Modifies: f_register values and more
void fetchStage(forwardType forward, bubbleType bubble){
   
   unsigned int f_pc; //address of instruction be fetched
   if((forward.M_icode == JXX) && !forward.M_Cnd) f_pc = forward.M_valA;
   else if(forward.W_icode == RET) f_pc = forward.W_valM;
   else f_pc = F.predPC; 
  
   bool memError= false; 
   unsigned int tempByte = getByte(f_pc, &memError);
   unsigned int f_ifun;
   unsigned int f_icode;
   if(memError){   //determine icode and ifun
        f_ifun = FNONE;
        f_icode = NOP;
   } else{
        f_ifun = getBits(0,3, tempByte);
        f_icode = getBits(4,7, tempByte);
   } 
   
   //is instruction valid
   bool instr_valid = instrValid(f_icode);
   
   //determine status code for fetched instruction 
   unsigned int f_stat;
   if(memError) f_stat = SADR;
   else if(!instr_valid) f_stat = SINS;
   else if(f_icode==HALT) f_stat = SHLT;
   else f_stat = SAOK;         
   
   //does fetched instruction require a register_id byte?
   unsigned int rA;
   unsigned int rB;
   bool needRId = needRegId(f_icode);
   if(needRId){
       unsigned int rByte = getByte(f_pc+1, &memError);
       rA = getBits(4, 7, rByte);
       rB = getBits(0, 3, rByte);     
   } else{
        rA = RNONE;
        rB = RNONE;
   } 
   
   //does fetched instruction require a constant word?
   //bool need_valC;
   bool needVC = needValC(f_icode);
   unsigned int f_valC;

   f_valC = getValC(f_pc, needRId, needVC);
   
   unsigned int f_valP;
   f_valP = pcIncrement(f_pc, needVC, needRId);   
   
   if(!checkFStall(bubble)) 
        F.predPC = predictPC(f_icode, f_valC, f_valP); 

   if(checkDBubble(bubble))
        updateDregister(SAOK, NOP, FNONE, FNONE, FNONE, RNONE, RNONE);

   else if(!checkDStall(bubble))
        updateDregister(f_stat, f_icode, f_ifun, rA, rB, f_valC, f_valP);
}

// Function: getStat()
// Description: returns proper stat
// Params: unsigned int f_icode, bool memError, bool valid_icod
// Returns: the proper SADR SINS SHLT OR SAOK
// Modifies: nothing
int getStat(unsigned int f_icode, bool memError, bool valid_icode){
    if(memError) return SADR;
    else if(!valid_icode) return SINS;
    else if(f_icode == HALT) return SHLT;
    else return SAOK;
}

// Function: checkFStall()
// Description: checks for F stalling
// Params: bubble object
// Returns: true if one of the conditions are met
// Modifies: nothing
bool checkFStall(bubbleType bubble){
   if(((bubble.E_icode == MRMOVL || bubble.E_icode == POPL) &&
      (bubble.E_dstM == bubble.d_srcA || bubble.E_dstM == bubble.d_srcB)) || 
      (bubble.D_icode == RET || bubble.E_icode == RET || bubble.M_icode == RET))
       return true;   
   return false; 
}

// Function: checkDStall()
// Description: checks for D stalling
// Params: bubble object
// Returns: true if one of the conditions are met
// Modifies: nothing
bool checkDStall(bubbleType bubble){
   if((bubble.E_icode == MRMOVL || bubble.E_icode == POPL) &&
      (bubble.E_dstM == bubble.d_srcA || bubble.E_dstM == bubble.d_srcB)) 
       return true;   
   return false; 
}

// Function: checkDBubble()
// Description: checks for D Bubbling
// Params: bubble object
// Returns: true if one of the conditions are met
// Modifies: nothing
bool checkDBubble(bubbleType bubble){
   if((bubble.E_icode == JXX && !bubble.e_Cnd)||
      (!((bubble.E_icode == MRMOVL || bubble.E_icode == POPL) &&
      (bubble.E_dstM == bubble.d_srcA || bubble.E_dstM == bubble.d_srcB)) && 
      (bubble.D_icode == RET || bubble.E_icode == RET || bubble.M_icode == RET))) 
       return true;   
   return false; 
}

// Function: getValC
// Description: takes characters and creates a word
// Params: int f_pc, bool needRId, bool needValC
// Returns: an assembled word 
// Modifies: memError (local)
int getValC(int f_pc,bool needRId, bool needValC){
       bool memError = false;
       int addr = f_pc+1;
       if(!needValC) return 0;
       if(needRId) addr++;
       unsigned char byte0 = getByte(addr, &memError);
       unsigned char byte1 = getByte(addr+1, &memError);
       unsigned char byte2 = getByte(addr+2, &memError);
       unsigned char byte3 = getByte(addr+3, &memError);
       return buildWord(byte0, byte1, byte2, byte3);        
}

// Function: pcIncrement
// Description: increments pc 
// Params: unt pc, bool needValC, bool needRegIds
// Returns: unt pc 
// Modifies: nothing
unsigned int pcIncrement(unsigned int pc, bool needValC, bool needRegIds){
    pc++; //f_ifun
    if(needRegIds) pc++; //f_icode
    if(needValC) pc=pc+4; //V/D/Dest
    return pc;
}

// Function: predictPC
// Description: valC if JXX or Call, valP otherwise. 
// Params: unt icode, unt valC, unt valP 
// Returns: valC or valP 
// Modifies: nothing
unsigned int predictPC(unsigned int icode, unsigned int valC, unsigned int valP){
    if(icode == JXX || icode == CALL ) return valC; 
    else return valP;
}

// Function: instrValid
// Description: checks to see if the inc instruction is valid
// Params: f_icode
// Returns: true if the instructions valid (NOP OR HALT)
//          False if f_icode != NOP or HALT
// Modifies: none
bool instrValid(unsigned int f_icode){
  if(f_icode == NOP || f_icode == HALT || f_icode == RRMOVL || f_icode == RMMOVL ||
      f_icode == MRMOVL || f_icode == OPL || f_icode == JXX || f_icode == CALL ||
      f_icode == RET || f_icode == PUSHL || f_icode == POPL || f_icode == DUMP || f_icode == IRMOVL)
       return true;
  
  else return false;
}

// Function: needRegId
// Description: does the instruction need a regid byte?
// Params: f_icode
// Returns: true if the icode needs regid
//          false if the icode does not need regid
// Modifies: none
bool needRegId(unsigned int f_icode){
   if(f_icode == RRMOVL || f_icode == OPL || f_icode == PUSHL || f_icode == POPL ||
      f_icode == IRMOVL || f_icode == RMMOVL || f_icode == MRMOVL)
      return true;
   
   else return false;
}

// Function: needValC
// Description: checks if the instruction needs a constant word, valC
// Params: f_icode
// Returns: true if valC is needed
//          false if the valC is not needed
// Modifies: none
bool needValC(unsigned int f_icode){
   if(f_icode == IRMOVL || f_icode == RMMOVL || f_icode == MRMOVL || 
      f_icode == JXX || f_icode == CALL || f_icode == DUMP)
        return true;

   else return false;
}


// Function: selectPC
// Description: Find the source of the PC
// Params: f_icode
// Returns: F.predPC
// Modifies: none
/*
  unsigned int selectPC(unsigned int f_icode){
    return F.predPC;    
  
  }
*/

// Function: getFregisteir
// Description: Returns a copy of the F register
// Params: none
// Returns: fregister
// Modifies: none
fregister getFregister(){
    return F;
}

// Function: clearFregister
// Description: Returns a copy of the F register
// Params: none
// Returns: none
// Modifies: F 
void clearFregister(){
    clearBuffer((char *) &F, sizeof(F));
}
