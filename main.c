#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tools.h"
#include "memory.h"
#include "dump.h"
#include "registers.h"
#include "loader.h"
#include "status.h"
#include "forwarding.h"
#include "bubbling.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "writebackStage.h"

void initialize();

//main functions
int main(int argv, char * args[]){
    initialize(); 
    if(!load(argv, args)){
        dumpMemory();
        exit(0);
    }

    int clockCount = 0;
    bool stop = false;
    forwardType forward; //added for lab7
    statusType status;//lab 9
    bubbleType bubble;
    while(!stop){
        stop = writebackStage(&forward, &status);
        memoryStage(&forward, &status, &bubble);
        executeStage(&forward, status, &bubble);
        decodeStage(forward, &bubble);
        fetchStage(forward, bubble);
        clockCount++;
    }

    printf("\nTotal clock cycles = %d\n", clockCount);
}


void initialize(){
    initializeFuncPtrArray();
    clearMemory();
    clearRegisters();
    clearFregister();
    clearDregister();
    clearEregister();
    clearMregister();
    clearWregister();
}

void testFunctions(){
/*
printf("inside main\n \n ");
printf("\n validFileName tests: \n");
printf("bumba.y should return 0: ");
printf("%d\n", validFileName("bumba.y"));
printf("noDot should return 0: ");
printf("%d\n", validFileName("noDot"));
printf("victory.yo should return 1: ");
printf("%d\n", validFileName("victory.yo"));

printf("\n isAddress tests: \n");
printf("  0x01H: should return 0:");
printf("%d\n",isAddress("  0x01H:"));
printf("  1x014: should return 0:");
printf("%d\n",isAddress("  1x014:"));
printf("  0x014: should return 1:");
printf("%d\n",isAddress("  0x014:"));
printf("  0x6A4: should return 1:");
printf("%d\n",isAddress("  0x064:"));

//need to add testing for 0x014:00 ... spaces ... |
printf("\n isData tests: \n");
printf("  0x014: 0z  should return 0:");
printf("%d\n",isData("  0x014: 0z"));
printf("  0x014: 00ddxx01daq should return 0:");
printf("%d\n",isData("  0x014: 00ddxx01da0q"));
printf("  0x014: 00345dzqa000 should return 0:");
printf("%d\n",isData("  0x014: 000345dzqa000"));
printf("  0x014: 00ddd0000000 should return 1:");
printf("%d\n",isData("  0x014: 00ddd0000000 "));
printf("  0x014: 00000000001 should return 1: ");
printf("%d\n",isData("  0x014: 000000000001"));
printf("  0x014: 01 should return 1: ");
printf("%d\n",isData("  0x014: 01"));

printf("\n isSpaces tests: \n");
printf("should return 0: wxrHtl-jhplzsk");
printf("%d\n",isSpaces("wxrHtl-jhplzsk",0,14));
printf("%d\n",isSpaces("wxrHtl-jhplzsk",0,14));
printf("isSpaces should return 1: ");
printf("%d\n",isSpaces("               ",0,14));
printf("isSpaces should return 1: ");
printf("%d\n",isSpaces("   ",0,2));
printf("isSpaces should return 0: ");
printf("%d\n",isSpaces("d  ",0,2));

printf("checkHex tests: \n");
printf("abcdzf0123456789 should return 0: ");
printf("%d\n",checkHex("abcdzf0123456789",0,15));
printf("abcdef0123456789 should return 1: ");
printf("%d\n",checkHex("abcdef0123456789",0,15));

printf("grabAddress tests: \n");
printf("  0x01a should return 0x01a: ");
printf("%x\n",grabAddress("  0x01a"));
printf("  0x014 should return 0x014: ");
printf("%x\n",grabAddress("  0x014"));

printf("grabDataByte tests: \n");
printf("  01a402, 4 should return 02: ");
printf("%x\n", grabDataByte("01a402",4));
printf("  01a402, 2 should return a4: ");
printf("%x\n", grabDataByte("01a402",2));
printf("  01, 0 should return 01: ");
printf("%x\n", grabDataByte("01",0));


printf("countBytes tests: \n");
printf("  0x014: 00000000001 should return 5: ");
printf("%x\n", countBytes("  0x014: 0000000001 "));
printf("  0x014: 0201 should return 2: ");
printf("%x\n", countBytes("  0x014: 0201"));
printf("  0x014: 020100 should return 3: ");
printf("%x\n", countBytes("  0x014: 020100"));
printf("  0x014:  should return 0: ");
printf("%x\n", countBytes("  0x014: "));

printf("checkLine tests: \n");
printf("  0x014 000000000001 |comments should return 0: ");
printf("%d\n", checkLine("  0x014 000000000001 |comments\n"));
printf("0x014: 000000000001 |comments should return 0: ");
printf("%d\n", checkLine("0x014: 000000000001 |comments\n"));
printf("  0x014: 000000000001 |comments should return 1: ");
printf("%d\n", checkLine("  0x014: 000000000001 |comments\n"));
//printf("  01a402, 2 should return a4: ");
//printf("%x\n", grabDataByte("01a402",2));

//printf("  01, 0 should return 01: ");
//
printf("discardRest should return 0: \n");
printf("%d\n",discardRest("",0,15));
printf("discardRest should return 1: \n");
printf("%d\n",discardRest("",0,15));
*/
}


