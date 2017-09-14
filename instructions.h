#define HALT 0x0
#define NOP 0x1
#define RRMOVL 0x2
#define IRMOVL 0x3 
#define RMMOVL 0x4 
#define MRMOVL 0x5
#define OPL 0x6
#define JXX 0x7 
#define CMOVXX 0x2
#define CALL 0x8
#define RET 0x9
#define PUSHL 0xA
#define POPL 0xB

#define DUMP 0xc
#define FNONE 0x0

// Status codes
#define SAOK  1
#define SHLT  2
#define SADR  3
#define SINS  4


//function codes
#define ADDL 0x60
#define SUBL 0x61
#define ANDL 0x62
#define XORL 0x63

//added for lab8
#define RESP 0x4
