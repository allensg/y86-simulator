#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "loader.h"
#include "tools.h"
#include "memory.h"


static bool validFileName(char * fileName);
static bool isAddress(char * record);
static bool isData(char * data);
static bool isSpaces(char * record, int start, int end);
static bool checkHex(char * record, int start, int end);
static bool checkLine(char * record);
static void discardRest();
static int grabAddress(char * record);
static unsigned char grabDataByte(char * record, int start);
//static bool checkLink(char * record, int preInt);
static int countBytes(char * data);



// Function: load
// Description:
// Params: pointer to file
// Returns: true(1) if successful else false(0)
// Modifies: 
bool load(int argv, char * args[]){
    char * fileName = args[1];
    FILE * fl;
    bool memError = false; 
    char curLine[80];

    if(argv !=2 || !validFileName(fileName)){ 
        printf("\nFile failed to open");
        return 0; 
    }   
     
    fl = fopen(fileName, "r");
     
    if(fl==NULL){
       printf("\nFile failed to open");
       return 0; 
    }
   
    int prevAdd = -1;
    int add = 0;
    int numBytes = 0;
    int lineNo = 1;
    unsigned char dByte; // holds a byte of data from curLine 
    
    while(fgets(curLine,80,fl)!=NULL){
        int length = strlen(curLine); 
        if(curLine[length-1] != '\n') discardRest(fl);
        if(checkLine(curLine)){
             if(isAddress(curLine)){ //if valid address
                add = grabAddress(curLine);
               
               if(add <= prevAdd){ 
                   printf("Error on line %d\n",lineNo);
                   printf(curLine);
                   printf("\n");
                   fclose(fl);
                   return false;
                }
                
                if(isSpaces(curLine,9,21)){
                    
                }
                 
                else if(isData(curLine)){ //if valid data
                    //get number of bytes in the currentLine
                    numBytes = countBytes(curLine); 
                    
                    int byteNumber;
                    int offset;
                    for(byteNumber = 1; byteNumber <= numBytes; byteNumber++){
                        
                        offset = ((byteNumber*2)+7); //accounts for the offset
                        dByte = grabDataByte(curLine, offset); //get data byte
                        
                        putByte(add, dByte, &memError); //put byte in memory
                         
                        if(memError){
                            fclose(fl);
                            return 0;
                        }
                        add++; //move to next address 
                    } //end of for loop
                    prevAdd = add-1;
                }
                 
            } //*/
            lineNo++; //lineNo for error checking
        }
        else{
            printf("Error on line %d\n",lineNo);

            printf(curLine);
            printf("\n");
            fclose(fl);
            return false;
        }
    }

    fclose(fl);
    return true;
}



// Function: validFileName
// Description: Checks if the filename ends in ".yo"
// Params: *filename is a pointer of type char
// Returns: true if the file ends in .yo
// Modifies: nothing
bool validFileName(char * fileName){
    int len = strlen(fileName);
        if(fileName[len-1] == 'o' && fileName[len-2] == 'y' && fileName[len-3] =='.') return true;
        else{
            printf("usage: yess <fileName>.yo");
            return false;
        }
}

// Function: isAddress
// Description: checks to see if the record contains an address
// Params: A record pointer of type char
//  Returns: true if the record is an address, false if not
//  Modifies: nothing
bool isAddress(char * record){
    if(record[2]=='0' && record[3]=='x' && record[7]==':') return checkHex(record, 4, 6);
    return false;
}

// Function: isData
// Description: checks to see if the record contains data
// Params: pointer to data of type char
// Returns: true if the record contains data
// Modifies: nothing
bool isData(char * data){  
    int i = 9;
    while(i<21 && data[i] != ' ' && data[i] != '\0' ){
        if(!checkHex(data, i, i)) return false;
        i++;
    }
    if((i-1)%2 != 0) return false;
    return isSpaces(data,i, 21); 
}

int countBytes(char * data){
    int first = 9;
    int count = 0;
    while(checkHex(data,first,first+1)){
        first+=2;
        count++;
    }
    return count;
}

// Function: isSpaces
// Description: checks to see if there are spaces at the start index
// Params: start index of type int, record pointer of type char, end index of ty
// Returns: true if the record contains spaces at the indices
// Modifies: nothing
bool isSpaces(char * record, int start, int end){
    for(; start <= end; start++){
        if(record[start]!=' ') return false;
    }
    return true;
}

// Function: checkHex
// Description: checks if the address is formed correctly
// Params: pointer to a record with the address, preAdd is the previously wirrte
// Returns: true if the address is correct
// Modifies: nothing
bool checkHex(char * record, int start, int end){   
    while(start <= end){
        if(!isxdigit(record[start])) return false;
        start++;
    }
    return true;
}

// Function: checkLine
// Description: checks if the record is in the correct format
// Params: *record to be checked, preAdd is the address of the record we checkin
// Returns: true if the line is correct
// Modifies: nothing
bool checkLine(char * record){
    bool b = true;

    if(record[22]!='|') return false;
   
    if(isAddress(record)){
        if(!isSpaces(record,0,1) || !isSpaces(record, 8,8) || !isSpaces(record, 21,21)) 
            b = false; 
        if(!isData(record))
           b = false;                        
    } 
    else{
        if(!isSpaces(record,0,21))
            b = false;
    } 
    return b;   
}


// Function: discardRest
// Description: use this to get rid of the file
// Params: pointer to a file, of type FILE
// Returns: NOTHING
// Modifies: throws away the rest of the line being read.
 
void discardRest(FILE * filePtr){
    //use fgetc to read the file until it finds the newline
    //feof checks for the end of file indicator. meaning if
    //find it the files done.
    while (fgetc(filePtr) != '\n' && !feof(filePtr)){
        //do nothing, this 'discards' the junk until newline(\n)a
    }
}

// Function: grabAddress
// Description: returns a char[] containing the
//          value of the address
// Params: char pointer to record
// Returns: char address  
// Modifies:
int grabAddress(char * record){
    char addr[4] = {record[4], record[5], record[6], '\0'};
    return (int)strtol(addr,NULL,16);
}

// Function: grabDataBtye
// Description: Returns one byte of data at the index
// Params: pointer to record, start index
// Returns: one byte of data from record
// Modifies: nothing
unsigned char grabDataByte(char * record, int start){
    char byte[3];
    //2 bytes and add a null char to the end so strtol behaves
    byte[0] = record[start];
    byte[1] = record[start + 1];
    byte[2] = '\0'; // \0 is null i think* see random webstie COMMENT THIS PART OUT IF RIGHT
    return (unsigned char) strtol(byte,NULL,16);
}
