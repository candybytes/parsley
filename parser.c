// TEAM

// First team member name  : Robert Valladares

// Second team member name : Danish Waheed

/**
 *  parser.c
 *
 *
 *  Created by Robert Valladares and Danish Waheed on 06/29/15.
 *  Copyright (c) 2015 ROBERT VALLADARES and DANISH WAHEED. All rights reserved.
 */

// Library declarations
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h" // for this demo because I am using MAX_VAR_LEN from global.h

int inputTokens = 0;


// -----------------Initial call to program  -----------------
int main(int argc, char *argv[]) {
    
    FILE *ifp = NULL;
    ifp = fopen("lexemelist.txt", "r");
    if (ifp == NULL) {
        printf("Need to add error number for file open here\n");
    }
    
    // will do linked list of char* to store each token without knowing count of tokens
    // and have a local-global token counter
    // all tokens no matter what, will be max length of 12
    char iToken[MAX_VAR_LEN + 1];  // One extra for nul char. //
    

    while (fscanf(ifp, "%s", iToken) != EOF) {
        // instead of printf, just create a new node of linked list with string
        printf("%s\n", iToken);
        inputTokens++;
        
    }
    printf("%d tokens \n", inputTokens);

    // close the input file
    fclose(ifp);
    

    
    
    return 0;
}
