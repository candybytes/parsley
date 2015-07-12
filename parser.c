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
#include "globals.h"

int m_n_inputTokens = 0;                // used to count the amount of tokens in lexemelist.txt
int m_nCurrentToken = INVALID_INT;      // used to mark if token is invalid
int m_nListIndex = 0;                   //  used as linkedlist index;
int m_nTokVarFlag = 0;                  // use to flag if token is variable or constant
int m_nVariableStackAdrx = 4;           // variables start of address of first AR
int m_nAR_Level = 0;                    // Activation Record first Level = 0;
struct NODE *gListHead = NULL;          // global list head in case of error printing, free malloc from this pointer

typedef char * string;
// character array of 100 = MAX_VARS_CONST_PROC
string m_caVariables[MAX_VARS_CONST_PROC];
// character array of 100 = MAX_VARS_CONST_PROC
string m_caConstants[MAX_VARS_CONST_PROC];
// character array of 100 = MAX_VARS_CONST_PROC
string m_caProcedures[MAX_VARS_CONST_PROC];
int m_nVarCount = 0;
int m_nConstCount = 0;
int m_nProcCount = 0;
int strsAreEqual(char * stt1, char *str2);


//--------------------local data structures ---------------

string str[100]; //make 100 strings

typedef struct NODE{
    char token[MAX_VAR_LEN + 1];
    struct NODE *next;
} NODE;

// function declaration look ahead
//int sumEveryOtherNode(NODE *head);
NODE *NewNode(char str[]);
NODE *InsertAtTail(NODE *head, char str[]);
void procedure_PROGRAM(NODE *head);
NODE *getNextTokenNode(NODE *head);
NODE *process_Block(NODE *head);
NODE *const_decl(NODE *head);
NODE *var_decl(NODE *head);
NODE *proc_decl(NODE *head);
NODE *process_STATEMENT(NODE *head);
NODE *process_EXPRESSION(NODE *head);
NODE *process_TERM(NODE *head);
NODE *process_FACTOR(NODE *head);
NODE *process_CONDITION(NODE *head);
void free_VAR_CONST_PROCS();
void printVAR_CONST_PROCS();
int existVar(char varName[]);
int existConst(char constName[]);
int existProc(char procName[]);




void FreeMemoryAllocFront_to_Tail(NODE *head);



// -----------------Initial call to program  -----------------
int main(int argc, char *argv[]) {
    int i = 0;
    
    FILE *ifp = NULL;
    ifp = fopen("lexemelist.txt", "r");
    if (ifp == NULL) {
        printf("Need to add error number for file open here\n");
    }
    
    // create a new ListHead
    NODE *ListHead = NULL;
    
    // will do linked list of char* to store each token without knowing count of tokens
    // and have a local-global token counter
    // all tokens no matter what, will be max length of 12
    char iToken[MAX_VAR_LEN + 1];  // One extra for nul char. //
    
    
    while (fscanf(ifp, "%s", iToken) != EOF) {
        // instead of printf, just create a new node of linked list with string
        // or add them to a char * array, less code
        ListHead = InsertAtTail(ListHead, iToken);
        m_n_inputTokens++;
        
    }
    
    // ----------test print ------------- //
    printf("%d tokens \n", m_n_inputTokens);
    
    
    free_VAR_CONST_PROCS();
    
    
    NODE *temp = NULL;
    temp = gListHead; // using global linkedlist pointer to head of list
    for (i =0; i < m_n_inputTokens; i++) {
        printf("token %s\n", temp->token);
        temp =  temp->next != NULL ? temp->next : temp;
        
    }
    // ----------test print ------------- //
    
    // call to analyse tokens -- parser
    temp = ListHead;
    procedure_PROGRAM(temp);
    
    
    
    // clean up after using the read tokens, you need to free the calloc spaced
    // when you are done with it
    if(gListHead != NULL) { FreeMemoryAllocFront_to_Tail(gListHead); gListHead = NULL;}
    //if(ListHead != NULL) { FreeMemoryAllocFront_to_Tail(ListHead); ListHead = NULL;}
    
    // close the input file
    fclose(ifp);
    
    
    return 0;
}

void printVAR_CONST_PROCS(){
    int i = 0;
    for (i = 0; i < 100; i++) {
        printf("%d %s %s %s\n", i, m_caConstants[i], m_caVariables[i], m_caProcedures[i]);
    }
}

//-----------------start -- read tokens in --------------------------

/*
 *  struct node *NewNode(int nData)
 *  create a new LinkedList node, inserting the value that was passed
 *  return the pointer of the new created node
 */
NODE *NewNode(char str[]){
    
    // create a memory space to store the node
    NODE *brandNewNode = NULL;
    brandNewNode = malloc(sizeof(NODE) );
    // check if malloc was successful
    if (brandNewNode != NULL) {
        // store the value pass in nData
        strcpy(brandNewNode->token, str);
        brandNewNode->token[MAX_VAR_LEN] = '\0';
        // set the node next pointer to null
        brandNewNode->next = NULL;
        
    } else {
        // malloc failed, ptr is still null
        printf("Failer to malloc space for node\n");
    }
    // return the pointer to the new created node, or NULL if it failed to malloc space
    return brandNewNode;
}

/*
 *  struct node *InsertAtTail(struct node *head, int data)
 *  insert a value at the tail of a linked list, return the
 *  same pointer that was passed, if the passed pointer is null
 *  creare a new list and return the head of the list
 */
NODE *InsertAtTail(NODE *head, char str[]){
    // check if head is null, if true, create new linked list
    if (head == NULL) {
        head = NewNode(str);
        gListHead = head;
        // check if creating a new list failed
        if (head == NULL) {
            printf("Failed to create a new 1 element LinkList head");
        }
        // if the head was null, a new list was created, return pointer to new list
        return head;
        
    }
    
    // create a new node to be added to the tail
    NODE *newTailNode = NULL;
    newTailNode = NewNode(str);
    if (newTailNode == NULL) {
        printf("Failed to create a new node at the tail");
    }
    // check if you are at the tail before inserting
    NODE *current = head;
    // run the while loop until you hit a node->next that is null
    while (current->next != NULL) {
        // loop until you find the tail of the list
        current = current->next;
    }
    
    // the tail node next gets the new node
    current->next = newTailNode;
    // return the original node passed
    return head;
}

/*
 * FreeMemoryAllocFront_to_Tail(struct node *head)
 * free all callor or malloc created, start at the node passed
 * and move forward freeing the memory as you go
 * free the last node at the end of last recursion
 */
void FreeMemoryAllocFront_to_Tail(NODE *head){
    
    if  (head == NULL){
        // empty list or node
        return;
    }
    // if the head was not null
    // then check if a leaf, if not a leaf, then
    // recursion on the next node and free the current
    NODE *current = NULL;
    
    if(head->next != NULL){
        current = head->next;
        free(head);
        head = NULL;
        FreeMemoryAllocFront_to_Tail(current);
    } else {
        // free the last node or leaf
        free(head);
        head = NULL;
    }
    
    return;
    
}


void free_VAR_CONST_PROCS(){
    
    
    
    int i = 0;
    for (i = 0; i < m_nVarCount; i++) {
        
        free(m_caVariables[i]);
    }
    
    for (i = 0; i < m_nConstCount; i++) {
        
        free(m_caConstants[i]);
    }
    
    for (i = 0; i < m_nProcCount; i++) {
        
        free(m_caProcedures[i]);
    }
    
}


//-----------------end -- read tokens in -------------------------------

// ------------------start of analyze tokens ---------------------------
/*
 * void procedure_PROGRAM(NODE *head)
 * start of the top-down token analyser from lexemelist
 */
void procedure_PROGRAM(NODE *head){
    // lexemelist has been read already and is stored in a linkedList
    
    NODE *nextTokenNode = NULL;
    
    
    if ( head != NULL) {
        // update the current token value and get the node pointer to next token
        nextTokenNode = getNextTokenNode(head);
        
        // process code block
        nextTokenNode = process_Block(nextTokenNode);
        
        // program should have ended after processing code block, look for periodsym
        if (m_nCurrentToken != periodsym) {
            printError(err9, NULL);
        }
        return;
    }
    
    // if LinkedList is null or empty, return and print error
    printError(err35, " 299");
    return;
    
    
}

NODE *getNextTokenNode(NODE *head){
    
    // if the head passed is not null, update the current token
    // the list next index, and return the pointer to next list node
    if ( head != NULL && m_nListIndex < m_n_inputTokens ){
        // cast string token to integer value only if its not a variable or constant declaration
        // " 2 m 9 3 7 " is translated as " m = 7 "
        printf("string token %s\n", head->token);
        m_nCurrentToken = atoi(head->token);    // update the current token value
        head = head->next;                      // next token node
        m_nListIndex++;                         // next list index
        return head;                            // return pointer to next token
    }
    
    m_nCurrentToken = INVALID_INT;              // if the head was NULL, set currentToken to invalid value
    printError(err35, "320 ");                     // error reading token from linked list
    return head;                                // it will never reach this point, error will terminate program
    
}

NODE *process_Block(NODE *head){
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    // is current token a valid token?
    if (m_nCurrentToken != INVALID_INT || nextTokenNode != NULL) {
        
        
        
        if (m_nCurrentToken == constsym){
            //--------------NEED TO ADD ENTER METHOD ------------------
            // token is of kind constant constsym = 28
            // if token = "const" then CONST-DECL();
            printf("\nCalling constant declaration\n");
            nextTokenNode = const_decl(nextTokenNode);
            printf("\nreturning from constant declaration\n");
        }
        
        if (m_nCurrentToken == varsym){
            // token is of kind variable = 29
            printf("\nCalling  variable declaration\n");
            nextTokenNode = var_decl(nextTokenNode);
            printf("\nreturning from variable declaration\n");
        }
        
        if (m_nCurrentToken == procsym){
            // token is of kind Procedure = 30
            printf("\nCalling  procedure declaration\n");
            nextTokenNode = proc_decl(nextTokenNode);
            printf("\nreturning from procedure declaration\n");
        }
        
        
        printf("\nCalling statement\n\n");
        nextTokenNode = process_STATEMENT(nextTokenNode);
        printf("\nreturning from statement\n");
        return nextTokenNode;
        
    }
    
    // error reading the current token from getNextTokenNode
    printError(err35, "367 ");
    return nextTokenNode;
    
    
}


//--------------NEED TO ADD ENTER METHOD ------------ NODE *const_decl(NODE *head) ------------------

int existVar(char varName[]){
    
    int i = 0;
    
    for (i = 0; i < m_nVarCount; i++) {
        
        if ( strsAreEqual(m_caVariables[i], varName) ) {
            return 1;
        }
        
    }
    
    return 0;
}

int existConst(char constName[]){
    
    int i = 0;
    
    for (i = 0; i < m_nConstCount; i++) {
        
        if ( strsAreEqual(m_caConstants[i], constName) ) {
            return 1;
        }
        
    }
    
    return 0;
}

int existProc(char procName[]){
    
    int i = 0;
    
    for (i = 0; i < m_nProcCount; i++) {
        
        if ( strsAreEqual(m_caProcedures[i], procName)) {
            return 1;
        }
        
    }
    
    return 0;
}

int strsAreEqual(char * stt1, char *str2){
    
    return strcmp(stt1, str2) == 0;
    
}

NODE *const_decl(NODE *head){
    
    char cIdent[MAX_STR + 1] = " ";
    int nNumber = 0;
    int nConstant = 0;
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        // token = 28 (constant)
        nConstant = m_nCurrentToken;
        
        do {
            // " 28 2 m 9 3 7 " is translated as "const m = 7 "
            
            // token = 28 (constant)
            //nConstant = m_nCurrentToken;
            
            // update the current token value and get the node pointer to next token
            nextTokenNode = getNextTokenNode(nextTokenNode);
            // token = 2
            if (m_nCurrentToken != identsym) {
                printError(err36, "449 ");
            }
            
            // token = m
            // store the variable name, and go to next token after string variable
            //strncpy(cIdent, nextTokenNode->token, MAX_STR + 1);
            // check if constant exist already as variable, constant or procedure
            if ( existVar(nextTokenNode->token) || existConst(nextTokenNode->token) || existProc(nextTokenNode->token) ){
                // error constant already declared as a variable or procedure name
                printError(err45, "458 ");
            }
            
            
            m_caConstants[m_nConstCount] = malloc((MAX_STR + 1) * sizeof(char));
            strncpy(m_caConstants[m_nConstCount++], nextTokenNode->token, MAX_STR + 1);
            // just call nexttokennode and skip the variable string return integer (garbage)
            // token has garbage
            nextTokenNode = getNextTokenNode(nextTokenNode); // skip
            
            
            // token 9 or =
            nextTokenNode = getNextTokenNode(nextTokenNode);
            if (m_nCurrentToken != eqlsym) {
                printError(err3, " 472");
            }
            // token = 3
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
            if (m_nCurrentToken != numbersym) {
                printError(err2, "478 ");
            }
            // token = 7 (the number that follows 3)
            //printf("token %s\n", nextTokenNode->token);
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            nNumber = m_nCurrentToken;
            
            
            printf("%d, %s, %d\n", nConstant, m_caConstants[m_nConstCount-1], nNumber);
            // TO-DO. Create method ENTER or some other method that processes the Values
            //ENTER(constant, ident, number);
            
            // token could be coma or semicolon , ;
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
        } while (m_nCurrentToken == commasym);
        
        if (m_nCurrentToken != semicolonsym) {
            printError(err37, "497 ");
        }
        
        return ( nextTokenNode = getNextTokenNode(nextTokenNode) );
    }
    // error reading the current token from getNextTokenNode
    printError(err35, "503 ");
    return NULL;
    
}

NODE *var_decl(NODE *head){
    
    char cIdent[MAX_STR + 1] = " ";
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        // token is already 2, next token is the variable name and not a numerical value
        // var a, b;  is the same as 29 2 a 17 2 b 18,
        // this is processing from a, b; a is variable thus the 2 a 17 2 b 18
        // update the current token value and get the node pointer to next token
        // need to check if variable is valid, does it start with a number?
        
        
        // m_nCurrentToken is varsym 29  "var" at begining of do loop
        do {
            // m_nCurrentToken is a comma after first loop, then read next variable
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
            // token = 2
            if (m_nCurrentToken != identsym) {
                printError(err36, "531 ");
            }
            
            // store the variable name, and go to next token after string variable
            //m_caVariables[m_nVarCount++] = cIdent;
            //strncpy(cIdent, nextTokenNode->token, MAX_STR + 1);
            // check if variable exist already as variable, constant or procedure
            
            if ( existVar(nextTokenNode->token) || existConst(nextTokenNode->token) || existProc(nextTokenNode->token) ){
                // error constant already declared as a variable or procedure name
                printError(err46, "541 ");
            }
            m_caVariables[m_nVarCount] = malloc((MAX_STR + 1) * sizeof(char));
            strncpy(m_caVariables[m_nVarCount++], nextTokenNode->token, MAX_STR + 1);
            // just call nexttokennode and skip the variable string return integer (garbage)
            // token has garbage
            
            nextTokenNode = getNextTokenNode(nextTokenNode); // skip
            
            
            
            // is m_nCurrentToken a comma or different
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
            // TO-DO. Create method ENTER or some other method that processes the Values
            //ENTER(m_nVariableStackAdrx, cIdent, m_nAR_Level);
            printf("%d, %s, %d\n", m_nVariableStackAdrx, m_caVariables[m_nVarCount - 1], m_nAR_Level);
            m_nVariableStackAdrx++;
            
        } while (m_nCurrentToken == commasym );
        
        
        if (m_nCurrentToken != semicolonsym) {
            printError(err37, "564 ");
        }
        
        return ( nextTokenNode = getNextTokenNode(nextTokenNode) );
        
    }
    // error reading the current token from getNextTokenNode
    printError(err35, "571 ");
    return NULL;
}

NODE *proc_decl(NODE *head){
    
    char cProcedure[MAX_STR + 1] = " ";
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        while (m_nCurrentToken == procsym) {
            
            // update the current token value and get the node pointer to next token
            nextTokenNode = getNextTokenNode(nextTokenNode);
            if (m_nCurrentToken != identsym) {
                printError(err38, "588 ");
            }
            
            // check if variable exist already as variable, constant or procedure
            if ( existVar(nextTokenNode->token) || existConst(nextTokenNode->token) || existProc(nextTokenNode->token) ){
                printError(err47, "593 ");
            }
            
            // store the procedure name, and go to next token
            //strncpy(cProcedure, nextTokenNode->token, MAX_STR + 1);
            m_caProcedures[m_nProcCount] = malloc((MAX_STR + 1) * sizeof(char));
            strncpy(m_caProcedures[m_nProcCount++], nextTokenNode->token, MAX_STR + 1);
            // just call nexttokennode and skip the variable string return integer (garbage)
            // token has garbage
            
            nextTokenNode = getNextTokenNode(nextTokenNode); // skip
            
            printf("%d, %s\n", procsym, m_caProcedures[m_nProcCount - 1]);
            //ENTER(procedure, ident);
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            if (m_nCurrentToken != semicolonsym) {
                printError(err39, "610 ");
            }
            
            // get next token, increase the L level, call block procedure again
            nextTokenNode = getNextTokenNode(nextTokenNode);
            m_nAR_Level++;
            m_nVariableStackAdrx += 3; // increase the variables starting address
            // ** when increasing the AR level, you must also increase the stack space
            nextTokenNode = process_Block(nextTokenNode);
            
            
            if (m_nCurrentToken != semicolonsym) {
                printError(err40, "622 ");
            }
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
        }
        // return normally from procedure
        return nextTokenNode;
        
    }
    // error reading the current token from getNextTokenNode
    printError(err35, "632 ");
    return NULL;
    
}



NODE *process_STATEMENT(NODE *head){
    
    printf("\nEntered statement, token is %d\n", m_nCurrentToken);
    
    char cIdent[MAX_STR + 1] = " ";
    char cProcedureCall[MAX_STR + 1] = " ";
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        
        switch (m_nCurrentToken) {
                
            case INVALID_INT:
                // error reading the current token from getNextTokenNode
                printError(err35, "655 ");
                break;
                
                // token is of kind identsym
            case identsym:
                
                // store the variable name, and go to next token after string variable
                // need to handle variable address lookup here most likely
                strncpy(cIdent, nextTokenNode->token, MAX_STR + 1);
                
                // check if variable exist or not, if it does not exit, error11
                if ( ! existVar(nextTokenNode->token) ){
                    printError(err11, " 667");
                }
                
                // just call nexttokennode and skip the variable string return integer (garbage)
                // token has garbage
                
                nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                
                
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                // if token <> " := "   error
                if (m_nCurrentToken != becomessym) {
                    printError(err41, "680 ");
                }
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                // expression
                nextTokenNode = process_EXPRESSION(nextTokenNode);
                
                break;
                
            case callsym:
                // token is of kind callsym
                
                // get token
                nextTokenNode = getNextTokenNode(nextTokenNode);
                // if token <> " identsym " error
                if (m_nCurrentToken != identsym) {
                    printError(err36, " 696");
                }
                // store the procedure name, and go to next token
                // does it have to call the procedure address? or just tokenize the lexemelist
                strncpy(cProcedureCall, nextTokenNode->token, MAX_STR + 1);
                
                // check if procedure exist or not, if it does not exit, error11
                if ( ! existProc(nextTokenNode->token)  ){
                    printError(err48, "704 ");
                }
                // just call nexttokennode and skip the variable string return integer (garbage)
                // token has garbage
                nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                
                break;
                
            case beginsym:
                // token is of kind  beginsym
                
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                
                // statement
                nextTokenNode = process_STATEMENT(nextTokenNode);
                
                while (m_nCurrentToken == semicolonsym) {
                    // gettoken
                    nextTokenNode = getNextTokenNode(nextTokenNode);
                    // statement
                    nextTokenNode = process_STATEMENT(nextTokenNode);
                }
                // if token <> "end" error
                if (m_nCurrentToken != endsym) {
                    printError(err42, "732 ");
                }
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            case ifsym:
                // token is of kind ifsym
                
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                
                // condition
                nextTokenNode = process_CONDITION(nextTokenNode);
                
                // if token <> "then" error
                if (m_nCurrentToken != thensym) {
                    printError(err16, "749 ");
                }
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                //statement
                nextTokenNode = process_STATEMENT(nextTokenNode);
                
                break;
                
            case whilesym:
                // token is of kind whilesym
                
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                //condition
                nextTokenNode = process_CONDITION(nextTokenNode);
                // if token <> "dosym" error
                if (m_nCurrentToken != dosym) {
                    printError(err18, " 767");
                }
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                //statement
                nextTokenNode = process_STATEMENT(nextTokenNode);
                
                break;
                
            case writesym:
                // generate
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                if (m_nCurrentToken == identsym) {
                    
                    // check if variable exist or not, if it does not exit, error11
                    printf("token at line 783 %s\n", nextTokenNode->token );
                    if ( existVar(nextTokenNode->token) ){
                        // create code for LOD, 0, var address M // code for variable
                        // create code for SIO 0 0
                    } else if ( existConst(nextTokenNode->token) ) {
                        // create code for LIT, 0, constant address M //code for constant
                        // create code for SIO 0 0
                    } else {
                        // undeclared constant or variable
                        printError(err11, "791 ");
                    }
                    nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                    nextTokenNode = getNextTokenNode(nextTokenNode); // after var or constant name
                    printf("token at line 795 %s token is %d\n", nextTokenNode->token, m_nCurrentToken );
                    // if token <> " ; semicolonsym " error
                    if (m_nCurrentToken != semicolonsym) {
                        printError(err5, "798 ");
                    }
                    
                    //nextTokenNode = getNextTokenNode(nextTokenNode);
                }
                break;
                
            case endsym:
                //retunextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            case periodsym:
                // nextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            default:
                // failed to read lexeme list
                printError(err35, "808 ");
                break;
        }
        
        return nextTokenNode;
        
    }
    
    // need to do error number here for failed to read lexemelist token
    printError(err35, "787 ");
    // always must return something, at this point nextTokenNode is NULL
    
    return nextTokenNode;
    
}

NODE *process_EXPRESSION(NODE *head){
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        
        if ( (m_nCurrentToken == plussym) || (m_nCurrentToken == minussym) ) {
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
        }
        
        nextTokenNode = process_TERM(nextTokenNode);
        
        while ( (m_nCurrentToken == plussym) || (m_nCurrentToken == minussym) ) {
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
            nextTokenNode = process_TERM(nextTokenNode);
        }
        
        return nextTokenNode;
    }
    
    // need to do error number here for failed to read lexemelist token
    printError(err35, "820 ");
    // always must return something
    return nextTokenNode;
}

NODE *process_TERM(NODE *head){
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        // factor
        nextTokenNode = process_FACTOR(nextTokenNode);
        
        while ( (m_nCurrentToken == multsym) || (m_nCurrentToken == slashsym) ) {
            // get token
            nextTokenNode = getNextTokenNode(nextTokenNode);
            // factor
            nextTokenNode = process_FACTOR(nextTokenNode);
        }
        // return
        return nextTokenNode;
        
    }
    
    // need to do error number here for failed to read lexemelist token
    printError(err35, "846 ");
    // always must return something
    return nextTokenNode;
}

NODE *process_FACTOR(NODE *head){
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        
        switch (m_nCurrentToken) {
                
            case INVALID_INT:
                
                // error reading the current token from getNextTokenNode
                printError(err35, "863 ");
                break;
                
            case identsym:
                printf("factor, identsym\n");
                
                // check if variable exist or not, if it does not exit, error11
                
                if ( ! existConst(nextTokenNode->token) ){
                    printError(err11, "line 866 ");
                }
                nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                
                // do i need to handle reading the variable at the return
                // get the variable name, need to do something with the variable name
                nextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            case numbersym:
                
                nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                
                // do i need to handle reading the number at the return
                // get the number, need to do something with the number
                nextTokenNode = getNextTokenNode(nextTokenNode);
                break;
            case lparentsym:
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                // expression
                nextTokenNode = process_EXPRESSION(nextTokenNode);
                if (m_nCurrentToken != rparentsym) {
                    printError(err43, " ");
                }
                nextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            default:
                printError(err44, " ");
                break;
                
        }
        
        return nextTokenNode;
        
    }
    
    // need to do error number here for failed to read lexemelist token
    printError(err35, "911 ");
    // always must return something
    return nextTokenNode;
    
}

NODE *process_CONDITION(NODE *head){
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        
        if (m_nCurrentToken == oddsym) {
            // gettoken
            nextTokenNode = getNextTokenNode(nextTokenNode);
            // expression
            nextTokenNode = process_EXPRESSION(nextTokenNode);
        } else {
            // expression
            nextTokenNode = process_EXPRESSION(nextTokenNode);
            // if token <> relation, error
            if ( (m_nCurrentToken < eqlsym) || (m_nCurrentToken > geqsym) ){
                printError(err20, " ");
            }
            
            // gettoken
            nextTokenNode = getNextTokenNode(nextTokenNode);
            // expression
            nextTokenNode = process_EXPRESSION(nextTokenNode);
        }
        
        return nextTokenNode;
        
    }
    
    // need to do error number here for failed to read lexemelist token
    printError(err35, "948 ");
    // always must return something
    return nextTokenNode;
}



// ------------------end of analyze tokens ---------------------------


/*
 *  printError(int ErrorNumber)
 *  Print the string that is represented by the error number passed
 */
void printError(int ErrorNumber, char *strToken){
    
    // print the error message given an error number from g_caErrorMsgs[] char* array
    if (ErrorNumber <= MAX_ERROR) {
        // to find error string, substract offset of 1
        printf("Error %d, %s\n", ErrorNumber, g_caErrorMsgs[ErrorNumber - 1]);
        printf("%s\n", strToken);
        return;
    }
    // clean up after using the read tokens, you need to free the calloc spaced
    // when you are done with it
    //if(gListHead != NULL) { FreeMemoryAllocFront_to_Tail(gListHead); gListHead = NULL;}
    //free_VAR_CONST_PROCS();
    exit(EXIT_FAILURE);
    return;
    
}


















