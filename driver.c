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
        system("gcc -o scanner scanner.c && ./scanner -l");
        //PrintArgv[0] = "gcc -o scanner scanner.c && ./scanner -l";// print
    } else {
        system("gcc -o scanner scanner.c && ./scanner");
    }
    
    
    
    if ( printParserCode ) {
        system("gcc -o parser parser.c && ./parser -a");  // print lexeme
    } else {
        system("gcc -o parser parser.c && ./parser");
    }
    
    
    
    if (printVMStack ) {
        system("gcc -o vm vm.c && ./vm - v"); // print stack
        
    } else {
        system("gcc -o vm vm.c && ./vm");
    }
    
    return 0;
    
    
}

int strsAreEqual(char * stt1, char *str2){
    
    return strcmp(stt1, str2) == 0;
    
}
