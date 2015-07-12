/*
Name:		Robert Valladares
PID: 		r3358660
Date:		03/30/2014
Assigment:	Programming Assignment #3: LinkedList 
Class: 		COP 3502, Spring 2014
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver.h"

int strsAreEqual(char * stt1, char *str2);

int printLexemeScanner = 0;	// -l
int printParserCode = 0;	// -a
int printVMStack = 0;		// -v


int main(int argc, char* argv[])
{
    int i = 0;
    char *test[] = {"-v", "-l"};
    
    for (i = 0 ; i < argc; i++){
    	
    	if ( (printLexemeScanner == 0) && strsAreEqual("-l", argv[i]) ) {
    		printLexemeScanner = 1;
    	}
    	
    	if ( (printParserCode == 0) && strsAreEqual("-a", argv[i]) ) {
    		printParserCode = 1;
    	}
    	
    	if ( (printVMStack == 0) && strsAreEqual("-v", argv[i]) ) {
    		printVMStack = 1;
    	}
    	
    }
 
    
    runVM(printVMStack);
    printf("\n");
    //runSC(printLexemeScanner);
    
    return 0;


}

int strsAreEqual(char * stt1, char *str2){
    
    return strcmp(stt1, str2) == 0;
    
}
