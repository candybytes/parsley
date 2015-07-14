// TEAM

// First team member name  : Robert Valladares

// Second team member name : Danish Waheed

/**
 *  driver.c
 *
 *
 *  Created by Robert Valladares and Danish Waheed on 07/11/15.
 *  Copyright (c) 2015 ROBERT VALLADARES and DANISH WAHEED. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int strsAreEqual(char * stt1, char *str2);

int printLexemeScanner = 0;	// -l
int printParserCode = 0;	// -a
int printVMStack = 0;		// -v



int main(int argc, char* argv[])
{
    int i = 0;
    
    char *PrintArgv[3];
    
    for (i = 0 ; i < argc; i++){
        
        if ( strsAreEqual("-l", argv[i]) ) {
            printLexemeScanner = 1;	// -l
        }
        
        if ( strsAreEqual("-a", argv[i]) ) {
            printParserCode = 1; // -a
        }
        
        if ( strsAreEqual("-v", argv[i]) ) {
            printVMStack = 1; // -v
        }
        
    }
    
    if (printLexemeScanner) {
        PrintArgv[0] = "gcc -o scanner scanner.c && ./scanner -l";// print lexeme
    }
    
    PrintArgv[0] = "gcc -o scanner scanner.c && ./scanner"; // dont print lexeme
    
    if ( printParserCode ) {
        PrintArgv[1] = "gcc -o parser parser.c && ./parser -a";// print lexeme
    }
    
    PrintArgv[1] = "gcc -o parser parser.c && ./parser"; // dont print lexeme
    
    if (printVMStack ) {
        PrintArgv[2] = "gcc -o vm vm.c && ./vm - v"; // print stackt trace
    }
    PrintArgv[2] = "gcc -o vm vm.c && ./vm"; // dont print stackt trace
    
    system(PrintArgv[0]);	// -l
    //system(PrintArgv[1]);	// -a
    system(PrintArgv[2]); 	// -v
    return 0;
    
    
    
    return 0;
    
    
}

int strsAreEqual(char * stt1, char *str2){
    
    return strcmp(stt1, str2) == 0;
    
}
