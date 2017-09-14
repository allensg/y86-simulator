#include <stdbool.h>
#include "tools.h"
#include "forwarding.h"
#include "bubbling.h"
#include "status.h"
#include "instructions.h"
#include "registers.h"
#include "decodeStage.h"
#include "executeStage.h"

static dregister D;
static int selectFwdA(int srcA, forwardType forward);
static int fwdB(int srcB, forwardType forward);
static bool checkEBubble(bubbleType * b);
static int getSrcA();
static int getSrcB();
static int getDstM();
static int getDstE();



//Function: decodeStage
//Description: main of DecodeStage, controls almost everything
//Params: forwardType forward - forwardType object 
//        bubbleType * bubble - ptr to bubbleType object 
//Returns: nothing
//Modifies: d_srcA, d_srB, d_dstE, d_dstM, d_rvalA, d_rvalB, bubble->D_icode . 
//Also modifies registerE
void decodeStage(forwardType forward, bubbleType * bubble){
    int d_srcA = getSrcA();  //what register should be used as the A source?
    bubble->d_srcA = d_srcA; //what register should be used as the B source?
    int d_srcB = getSrcB();
    bubble->d_srcB = d_srcB; 
    int d_dstE = getDstE(); //what register should be used as the E destination?
    int d_dstM = getDstM(); //what register should be used as the M destination?

    //what register should be used as the A value?
    int d_rvalA = (int)selectFwdA(d_srcA, forward);

    //what register should be used as the B value?
    int d_rvalB = (int)fwdB(d_srcB, forward);
    
    bubble->D_icode = D.icode;
    
    //stat, icode, ifun, valc, vala, valb, dstE, dstM, srcA, srcB);
    if(!checkEBubble(bubble))
        updateEregister(D.stat, D.icode, D.ifun, D.valC, d_rvalA, d_rvalB, d_dstE, d_dstM, d_srcA, d_srcB);
    else
        updateEregister(D.stat, NOP, FNONE, FNONE, FNONE, FNONE, RNONE, RNONE, RNONE, RNONE);
}

//Function:checkEBubble 
//Description: checks for bubble cases 
//Params: bubbleType * b - prt to bubbleType object
//Returns: the forwarding location
//Modifies: none
bool checkEBubble(bubbleType * b){
    bubbleType bubble = *b;
    if(((bubble.E_icode == JXX) && !bubble.e_Cnd) ||
        (((bubble.E_icode == MRMOVL) || (bubble.E_icode == POPL)) &&
        ((bubble.E_dstM == bubble.d_srcA) || (bubble.E_dstM == bubble.d_srcB))))
       return true;
   return false;
 
}

//Function: selectFwdA
//Description: decide where to forward srcA
//Params: srcA and forward
//Returns: the forwarding location
//Modifies: none
int selectFwdA(int srcA, forwardType forward){
    if(D.icode == CALL|| D.icode == JXX) return D.valP;
    else if(srcA == RNONE) return 0;
    else if(srcA == forward.e_dstE) return forward.e_valE;
    else if(srcA == forward.M_dstM) return forward.m_valM;
    else if(srcA == forward.M_dstE) return forward.M_valE;
    else if(srcA == forward.W_dstM) return forward.W_valM;
    else if(srcA == forward.W_dstE) return forward.W_valE;
    else return getRegister(srcA);    
}

//Function: fwdB
//Description: Same as seletFwdA, just for srcB
//Params: srcB and forward
//Returns: forwarding location
//Modifies: none
int fwdB(int srcB, forwardType forward){
   if(srcB == RNONE) return 0; 
   else if(srcB == forward.e_dstE) return forward.e_valE;
   else if(srcB == forward.M_dstM) return forward.m_valM;
   else if(srcB == forward.M_dstE) return forward.M_valE;
   else if(srcB == forward.W_dstM) return forward.W_valM;
   else if(srcB == forward.W_dstE) return forward.W_valE;
   else return getRegister(srcB);    
}

// Function: getDregister
// // Description: Returns a copy of the Dregister
// // Params:
// // Returns:
// // Modifies:
dregister getDregister(){
    return D;
}

//  Function: getSrcA
//  Description: get Source A depending on the icode
//  Params: 
//  Returns: Source A
int getSrcA(){
    int srcA = RNONE;

    switch(D.icode){
        case RRMOVL:
            srcA = D.rA;
            break;
        case RMMOVL:
            srcA = D.rA;
            break;
        case OPL:
            srcA = D.rA;
            break;
        case PUSHL:
            srcA = D.rA;
            break;
        case POPL:
            srcA = ESP;
            break;
        case RET:
            srcA = ESP;
            break;
        default:
            srcA = RNONE; 
    }
    return srcA;
}


//  Function: getSrcB
//  Description: get source B depending on the icode
//  Params: none
//  Returns: the proper source B
int getSrcB(){
    int srcB = RNONE;

    switch(D.icode){
        case OPL:  
            srcB = D.rB;
            break;
        case RMMOVL:
            srcB = D.rB;
            break;
        case MRMOVL:
            srcB = D.rB;
            break;
        case PUSHL:
            srcB = ESP;
            break;
        case POPL:
            srcB = ESP;
            break;
        case CALL:
            srcB = ESP;
            break;
        case RET:
            srcB = ESP;
            break;
        default:
            srcB = RNONE;
    }
    return srcB;
}


//  Function: getDstE
//  Description: get destination E! Depending on the I code
//  Params: none
//  Returns: the proper dstE
int getDstE(){
    int dstE = RNONE;

    switch(D.icode){
        case RRMOVL:
            dstE = D.rB;
            break;
        case IRMOVL:
            dstE = D.rB;
            break;
        case OPL:
            dstE = D.rB;
            break;
        case PUSHL:
            dstE = ESP;
            break;
        case POPL:
            dstE = ESP;
         break;
        case CALL:
            dstE = ESP;
            break;
        case RET:
            dstE = ESP;
            break;
        default:
            dstE = RNONE;
    }
    return dstE;
}


//  Function: getDstM
//  Description: gets dstM depending on icodes
//  Params: none
//  Returns: dstM
int getDstM(){
    int dstM = RNONE;
    switch(D.icode){
        case MRMOVL:
            dstM = D.rA;
            break;
        case POPL:
            dstM = D.rA;
            break;
        default:
            dstM = RNONE;
    }
    return dstM;
}

//  Function: upDateDregister
//  Description: updates the values of D register
//  Params: unsigned int stat, icode, ifun, rA, rB, valC, valP
//  Returns: nothing
void updateDregister( unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int rA, unsigned int rB, 
                      unsigned int valC, unsigned int valP){
    D.stat = stat;
    D.icode = icode;
    D.ifun = ifun;
    D.rA = rA;
    D.rB = rB;
    D.valC = valC;
    D.valP = valP;    
}


// Function: getRegister
// // Description: Returns a copy of the Register
// // Params: none
// // Returns: none
// // Modifies: clears the D register

void clearDregister(){
    clearBuffer((char *) &D, sizeof(D));
    D.icode = NOP;
    D.stat = SAOK;
}

