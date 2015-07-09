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


//--------------------local data structures ---------------


typedef struct NODE{
    char token[MAX_VAR_LEN + 1];
    struct NODE *next;
} NODE;

// function declaration look ahead
int sumEveryOtherNode(NODE *head);
NODE *NewNode(char str[]);
NODE *InsertAtTail(NODE *head, char str[]);
NODE *getNextTokenNode(NODE *head);
void process_Block(NODE *head);
NODE *const_decl(NODE *head);
NODE *var_decl(NODE *head);
NODE *proc_decl(NODE *head);
void process_statement(NODE *head);


void FreeMemoryAllocFront_to_Tail(NODE *head);
void procedure_PROGRAM(NODE *head);


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
    
    NODE *temp = NULL;
    temp = gListHead; // using global linkedlist pointer to head of list
    for (i =0; i < m_n_inputTokens; i++) {
        //printf("token %s\n", temp->token);
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
        process_Block(nextTokenNode);
        
        // program should have ended after processing code block, look for periodsym
        if (m_nCurrentToken != periodsym) {
            printError(err9, NULL);
        }
        return;
    }
    
    // if LinkedList is null or empty, return and print error
    printError(err35, " ");
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
    printError(err35, " ");                     // error reading token from linked list
    return head;                                // it will never reach this point, error will terminate program
    
}

void process_Block(NODE *head){
    
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
        process_statement(nextTokenNode);
        printf("\nreturning from statement\n");
        return;
        
    }
    
    // error reading the current token from getNextTokenNode
    printError(err35, " ");
    return;
    
    
}


//--------------NEED TO ADD ENTER METHOD ------------ NODE *const_decl(NODE *head) ------------------
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
                printError(err36, " ");
            }
            
            // token = m
            // store the variable name, and go to next token after string variable
            strncpy(cIdent, nextTokenNode->token, MAX_STR + 1);
            // just call nexttokennode and skip the variable string return integer (garbage)
            // token has garbage
            nextTokenNode = getNextTokenNode(nextTokenNode); // skip
            
            
            // token 9 or =
            nextTokenNode = getNextTokenNode(nextTokenNode);
            if (m_nCurrentToken != eqlsym) {
                printError(err3, " ");
            }
            // token = 3
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
            if (m_nCurrentToken != numbersym) {
                printError(err2, " ");
            }
            // token = 7 (the number that follows 3)
            //printf("token %s\n", nextTokenNode->token);
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            nNumber = m_nCurrentToken;
            
            
            printf("%d, %s, %d\n", nConstant, cIdent, nNumber);
            // TO-DO. Create method ENTER or some other method that processes the Values
            //ENTER(constant, ident, number);
            
            // token could be coma or semicolon , ;
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
        } while (m_nCurrentToken == commasym);
        
        if (m_nCurrentToken != semicolonsym) {
            printError(err37, " ");
        }
        
        return ( nextTokenNode = getNextTokenNode(nextTokenNode) );
    }
    // have to do error number for this
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
                printError(err36, " ");
            }
            
            // store the variable name, and go to next token after string variable
            strncpy(cIdent, nextTokenNode->token, MAX_STR + 1);
            // just call nexttokennode and skip the variable string return integer (garbage)
            // token has garbage
            
            nextTokenNode = getNextTokenNode(nextTokenNode); // skip
            
            
            // is m_nCurrentToken a comma or different
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
            // TO-DO. Create method ENTER or some other method that processes the Values
            //ENTER(m_nVariableStackAdrx, cIdent, m_nAR_Level);
            printf("%d, %s, %d\n", m_nVariableStackAdrx, cIdent, m_nAR_Level);
            m_nVariableStackAdrx++;
            
        } while (m_nCurrentToken == commasym );
        
        
        if (m_nCurrentToken != semicolonsym) {
            printError(err37, " ");
        }
        
        return ( nextTokenNode = getNextTokenNode(nextTokenNode) );
        
    }
    // have to do error number for this
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
                printError(err38, " ");
            }
            
            // store the procedure name, and go to next token
            strncpy(cProcedure, nextTokenNode->token, MAX_STR + 1);
            // just call nexttokennode and skip the variable string return integer (garbage)
            // token has garbage
            nextTokenNode = getNextTokenNode(nextTokenNode); // skip
            
            
            printf("%d, %s\n", procsym, cProcedure);
            //ENTER(procedure, ident);
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            if (m_nCurrentToken != semicolonsym) {
                printError(err39, " ");
            }
            
            // get next token, increase the L level, call block procedure again
            nextTokenNode = getNextTokenNode(nextTokenNode);
            m_nAR_Level++;
            m_nVariableStackAdrx += 3; // increase the variables starting address
            // ** when increasing the AR level, you must also increase the stack space
            process_Block(nextTokenNode);
            
            
            if (m_nCurrentToken != semicolonsym) {
                printError(err40, " ");
            }
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
        }
        // return normally from procedure
        return nextTokenNode;
        
    }
    // have to do error number for this
    return NULL;
    
}

void process_statement(NODE *head){
    
    char cIdent[MAX_STR + 1] = " ";
    char cProcedureCall[MAX_STR + 1] = " ";
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        
        switch (m_nCurrentToken) {
                
            case INVALID_INT:
                // error reading the current token from getNextTokenNode
                printError(err35, " ");
                break;
                
            case identsym:
                // token is of kind identsym
                // store the variable name, and go to next token after string variable
                
                // need to handle variable address lookup here most likely
                strncpy(cIdent, nextTokenNode->token, MAX_STR + 1);
                // just call nexttokennode and skip the variable string return integer (garbage)
                // token has garbage
                nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                
                
                nextTokenNode = getNextTokenNode(nextTokenNode);
                if (m_nCurrentToken != becomessym) {
                    printError(err41, " ");
                }
                
                nextTokenNode = getNextTokenNode(nextTokenNode);
                
                // EXPRESSION METHOD();
                
                break;
                
            case callsym:
                // token is of kind callsym
                nextTokenNode = getNextTokenNode(nextTokenNode);
                if (m_nCurrentToken != identsym) {
                    printError(err36, " ");
                }
                // store the procedure name, and go to next token
                // does it have to call the procedure address? or just tokenize the lexemelist
                strncpy(cProcedureCall, nextTokenNode->token, MAX_STR + 1);
                // just call nexttokennode and skip the variable string return integer (garbage)
                // token has garbage
                nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                
                nextTokenNode = getNextTokenNode(nextTokenNode);
                
                break;
                
            case beginsym:
                // token is of kind constant beginsym
                nextTokenNode = getNextTokenNode(nextTokenNode);
                process_statement(nextTokenNode);
                
                while (m_nCurrentToken = semicolonsym) {
                    nextTokenNode = getNextTokenNode(nextTokenNode);
                    process_statement(nextTokenNode);
                }
                if (m_nCurrentToken != endsym) {
                    printError(err42, " ");
                }
                
                nextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            case ifsym:
                // token is of kind ifsym
                
                break;
                
            case whilesym:
                // token is of kind whilesym
                break;
                
            default:
                // HAVE TO DO AN ERROR HERE
                break;
        }
        
        
    }
    // statement can not be processed, token node was null
    //printError(err40, " "); // need to create error for this
    
    return;
    
}


// ------------------end of analyze tokens ---------------------------


/*
 *  printError(int ErrorNumber)
 *  Print the string that is represented by the error number passed
 */
void printError(int ErrorNumber, char *strToken){
    // always print what caused the error, then if error number is identified
    // print error information, else general message outside "IF" statement
    if(strToken) {
        //printf("Lexeme or token  :%s:  caused an error\n", strToken);
    }
    
    // print the error message given an error number from g_caErrorMsgs[] char* array
    if (ErrorNumber <= MAX_ERROR) {
        // to find error string, substract offset of 1
        printf("Error %d, %s\n", ErrorNumber, g_caErrorMsgs[ErrorNumber - 1]);
        return;
    }
    // if error number is not in array, still print there is an error, but number
    // not recognized
    printf("Error, ErrorNumber %d not recognized\n", ErrorNumber);
    // clean up after using the read tokens, you need to free the calloc spaced
    // when you are done with it
    if(gListHead != NULL) { FreeMemoryAllocFront_to_Tail(gListHead); gListHead = NULL;}
    return;
    
}


















