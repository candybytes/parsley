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

// ________________________________-line 672 identsym needs to check
//---------------------------------for const exist too

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
#define MAX_CODE_LENGTH 500             // max amount of tokenized instructions

int m_nVarCount = 0;
int m_nConstCount = 0;
int m_nProcCount = 0;
int strsAreEqual(char * stt1, char *str2);
void initializeNamerecord_table();

/*
 struct instructions {
	int OP; // OP_code
	int L;	// L_code
	int M;	// M_code
 };
 
 //structure of the symbol table record
 typedef struct {
 
 int TokenType;                  // token type
 int kind;                       // constant = 1; var = 2, proc = 3
 char name[MAX_VAR_LEN + 1];     // name up to 11 characters long, 11 + 1 for \0
 int val;                        // number (ASCII value)
 int level;                      // L level
 int adr;                        // M address
 
 } namerecord_t;
 
 typedef enum {lit = 1,opr,lod,sto,cal,inc,jmp,jpc,sio} eOPCODE;
 typedef enum {ret, neg, add, sub, mul, div_, odd, mod, eql, neq, lss, leq, gtr, geq} eSTACKOPS; // div is reserved word
 typedef enum {lexConstant = 1, lexVar, lexProc} eLexemeKind;
 
 */
// there will be at most m_nCleanCount separate namerecord_t tokens
namerecord_t namerecord_table[MAX_CODE_LENGTH];  // ARs token array
//namerecord_t singleNamerecord;  // // single Var, Const or Proc record
struct instructions codeLines[MAX_CODE_LENGTH];         // instructions array
//struct instructions IR;                                 // single instruction
int m_nCodeLineCount = 0;                               // index for code line genetator
int m_nNameRecordCount = 0;                             // index for ARs token array


//--------------------local data structures ---------------


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

int existVar(char varName[]);
int existConst(char constName[]);
int existProc(char procName[]);
void enterCode(int OPcode, int Lval, int Mval);
void enterNamerecord_table(int nKind, char caName[], int nVal, int nLevel, int nAdr );




void FreeMemoryAllocFront_to_Tail(NODE *head);



// -----------------Initial call to program  -----------------
int main(int argc, char *argv[]) {
    int i = 0;
    
    FILE *ifp = NULL;
    ifp = fopen("lexemelist.txt", "r");
    if (ifp == NULL) {
        printError(err35, " ");
        
    }
    
    
    
    // create a new ListHead
    NODE *ListHead = NULL;
    
    // will do linked list of char* to store each token without knowing count of tokens
    // and have a local-global token counter
    // all tokens no matter what, will be max length of 12
    char iToken[MAX_VAR_LEN + 1];  // One extra for nul char. //
    
    
    while (fscanf(ifp, "%s", iToken) != EOF) {
        // instead of //printf, just create a new node of linked list with string
        // or add them to a char * array, less code
        ListHead = InsertAtTail(ListHead, iToken);
        m_n_inputTokens++;
        
    }
    
    // close the input file
    fclose(ifp);
    // ----------test print ------------- //
    //printf("%d tokens \n", m_n_inputTokens);
    
    
    NODE *temp = NULL;
    temp = gListHead; // using global linkedlist pointer to head of list
    for (i =0; i < m_n_inputTokens; i++) {
        ////printf("token %s\n", temp->token);
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
    //fclose(ifp);
    
    
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
        printError(err30, "172");
        ////printf("Failer to malloc space for node\n");
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
            printError(err30, "192");
            ////printf("Failed to create a new 1 element LinkList head");
        }
        // if the head was null, a new list was created, return pointer to new list
        return head;
        
    }
    
    // create a new node to be added to the tail
    NODE *newTailNode = NULL;
    newTailNode = NewNode(str);
    if (newTailNode == NULL) {
        printError(err30, "192");
        ////printf("Failed to create a new node at the tail");
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
        nextTokenNode = process_Block(nextTokenNode);
        
        // program should have ended after processing code block, look for periodsym
        if (m_nCurrentToken != periodsym) {
            printError(err9, NULL);
        }
        // generate the last line of code for the vm, HALT vm program
        enterCode(sio, 0, 2);
        
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
        //printf("string token %s\n", head->token);
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
            // token constant declaration constsym = 28
            nextTokenNode = const_decl(nextTokenNode);
        }
        
        if (m_nCurrentToken == varsym){
            // token is variable declaration = 29
            nextTokenNode = var_decl(nextTokenNode);
        }
        
        if (m_nCurrentToken == procsym){
            // token Procedure declaration = 30
            nextTokenNode = proc_decl(nextTokenNode);
        }
        
        nextTokenNode = process_STATEMENT(nextTokenNode);
        
        return nextTokenNode;
        
    }
    
    // error reading the current token from getNextTokenNode
    printError(err35, "367 ");
    return nextTokenNode;
    
}


//--------------NEED TO ADD ENTER METHOD ------------ NODE *const_decl(NODE *head) ------------------

int existVar(char varName[]){
    
    int i = 0;
    
    for (i = 0; i < m_nNameRecordCount; i++) {
        
        if ( strsAreEqual(namerecord_table[i].name, varName) & (namerecord_table[i].kind == lexVar) ) {
            return i + 1;
        }
        
    }
    
    
    return 0;
}

int existConst(char constName[]){
    
    int i = 0;
    
    for (i = 0; i < m_nNameRecordCount; i++) {
        
        if ( strsAreEqual(namerecord_table[i].name, constName) & (namerecord_table[i].kind == lexConstant) ) {
            return i + 1;
        }
        
    }
    
    
    return 0;
}

int existProc(char procName[]){
    
    int i = 0;
    
    for (i = 0; i < m_nNameRecordCount; i++) {
        
        if ( strsAreEqual(namerecord_table[i].name, procName) & (namerecord_table[i].kind == lexProc) ) {
            return i + 1;
        }
        
    }
    
    return 0;
}

int strsAreEqual(char * stt1, char *str2){
    
    return strcmp(stt1, str2) == 0;
    
}

NODE *const_decl(NODE *head){ // done
    
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    namerecord_t singleNamerecord;  // single Var, Const or Proc record
    
    if ( nextTokenNode != NULL){
        
        do {
            // " 28 2 m 9 3 7 " is translated as "const m = 7 "
            
            // token = 28 (constant declaration)
            
            // update the current token value and get the node pointer to next token
            nextTokenNode = getNextTokenNode(nextTokenNode);
            // token = 2
            if (m_nCurrentToken != identsym) {
                printError(err36, "449 ");
            }
            
            // token = m
            // store the variable name, and go to next token after string variable
            
            // check if constant exist already as variable, constant or procedure
            if ( existVar(nextTokenNode->token) || existConst(nextTokenNode->token) || existProc(nextTokenNode->token) ){
                // error constant already declared as a variable or procedure name
                printError(err45, "458 ");
            }
            
            // copy the name of the constant
            //printf(" 530 %s\n", nextTokenNode->token );
            strcpy(singleNamerecord.name, nextTokenNode->token);
            // set the type of the constant
            singleNamerecord.kind = lexConstant;
            
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
            
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            // set the value of the constant
            singleNamerecord.val = m_nCurrentToken;
            // set the address of the constant
            singleNamerecord.adr = -1;
            // set the lelvel of the constant
            singleNamerecord.level = m_nAR_Level;
            
            // store the created single Name record into the array
            namerecord_table[m_nNameRecordCount++] = singleNamerecord;
            m_nConstCount++;
            
            //printf("constant 493::  %d, %s, %d\n", singleNamerecord.kind, singleNamerecord.name, singleNamerecord.val);
            
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

NODE *var_decl(NODE *head){ // done
    
    namerecord_t singleNamerecord;  // single Var, Const or Proc record
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        // token is already 2, next token is the variable name and not a numerical value
        // var a, b;  is the same as 29 2 a 17 2 b 18,
        // this is processing from a, b; a is variable thus the 2 a 17 2 b 18
        // update the current token value and get the node pointer to next token
        
        // m_nCurrentToken is varsym 29  "var" at begining of do loop
        do {
            // m_nCurrentToken is a comma after first loop, then read next variable
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
            // token = 2
            if (m_nCurrentToken != identsym) {
                printError(err36, "531 ");
            }
            
            // store the variable name, and go to next token after string variable
            // check if variable exist already as variable, constant or procedure
            
            if ( existVar(nextTokenNode->token) || existConst(nextTokenNode->token) || existProc(nextTokenNode->token) ){
                // error constant already declared as a variable or procedure name
                printError(err46, "541 ");
            }
            
            // copy the name of the variable
            strcpy(singleNamerecord.name, nextTokenNode->token);
            // set the type of the record
            singleNamerecord.kind = lexVar;
            
            
            // just call nexttokennode and skip the variable string return integer (garbage)
            // token has garbage
            
            nextTokenNode = getNextTokenNode(nextTokenNode); // skip
            
            
            // set the value of the variable
            singleNamerecord.val = 0;
            // set the address of the constant
            singleNamerecord.adr = 4 + m_nVarCount++;
            // set the lelvel of the constant
            singleNamerecord.level = m_nAR_Level;
            
            // store the created single Name record into the array
            namerecord_table[m_nNameRecordCount++] = singleNamerecord;
            
            // is m_nCurrentToken a comma or different
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
            // TO-DO. Create method ENTER or some other method that processes the Values
            //ENTER(m_nVariableStackAdrx, cIdent, m_nAR_Level);
            //printf("variable 563 :: %d, %s, %d\n", singleNamerecord.adr, singleNamerecord.name, singleNamerecord.level);
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

NODE *proc_decl(NODE *head){ // done
    
    namerecord_t singleNamerecord;  // single Var, Const or Proc record
    
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
            
            // copy the name of the procedure
            strcpy(singleNamerecord.name, nextTokenNode->token);
            // set the type of the record
            singleNamerecord.kind = lexProc;
            
            // just call nexttokennode and skip the variable string return integer (garbage)
            // token has garbage
            
            nextTokenNode = getNextTokenNode(nextTokenNode); // skip
            
            // set the value of the procedure
            singleNamerecord.val = 0;
            // set the address of the constant
            singleNamerecord.adr = 0;
            // set the lelvel of the constant
            singleNamerecord.level = m_nAR_Level;
            
            // store the created single Name record into the array
            namerecord_table[m_nNameRecordCount++] = singleNamerecord;
            
            //printf("procedure 632:: %d, %s\n", singleNamerecord.kind, singleNamerecord.name);
            //ENTER(procedure, ident);
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            if (m_nCurrentToken != semicolonsym) {
                printError(err39, "610 ");
            }
            
            // get next token, increase the L level, call block procedure again
            nextTokenNode = getNextTokenNode(nextTokenNode);
            m_nProcCount++;
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



NODE *process_STATEMENT(NODE *head){ // done
    
    int currentCodeLine = 0;
    int beginWhileLine = 0;
    int endWhileLine = 0;
    
    int nRecordFoundIndex = 0;
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        
        switch (m_nCurrentToken) {
                
            case INVALID_INT:
                // error reading the current token from getNextTokenNode
                printError(err35, "655 ");
                break;
                
                // token is of kind identsym
            case identsym: // done
                
                // store the variable name, and go to next token after string variable
                
                // check if variable exist or not, if it does not exit, error11
                if ( !( nRecordFoundIndex = existVar(nextTokenNode->token) )){
                    printError(err11, " 672");
                }
                nRecordFoundIndex -= 1; // fix the offset return from existVar i + 1;
                //printf("variable %s address %d\n", namerecord_table[nRecordFoundIndex].name, namerecord_table[nRecordFoundIndex].adr);
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
                
                // create a code line for VM
                enterCode(sto, namerecord_table[nRecordFoundIndex].level, namerecord_table[nRecordFoundIndex].adr);
                
                break;
                
            case callsym:
                // token is of kind callsym
                
                // get token
                nextTokenNode = getNextTokenNode(nextTokenNode);
                // if token <> " identsym " error
                if (m_nCurrentToken != identsym) {
                    printError(err36, " 696");
                }
                
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
                
            case beginsym: //done
                // token is of kind  beginsym
                
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                
                // statement
                nextTokenNode = process_STATEMENT(nextTokenNode);
                /*//printf("750 current token %d next token string %s\n",m_nCurrentToken, nextTokenNode->token);
                 if (m_nCurrentToken == 2 && (existVar(nextTokenNode->token) || existConst(nextTokenNode->token))) {
                 nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                 }
                 */
                nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                //printf("755 current token %d next token string %s\n",m_nCurrentToken, nextTokenNode->token);
                while (m_nCurrentToken == semicolonsym) {
                    // gettoken
                    nextTokenNode = getNextTokenNode(nextTokenNode);
                    // statement
                    nextTokenNode = process_STATEMENT(nextTokenNode);
                }
                // if token <> "end" error
                //printf("760 current token %d next token string %s\n",m_nCurrentToken, nextTokenNode->token);
                if (m_nCurrentToken != endsym) {
                    printError(err42, "732 ");
                }
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            case ifsym: // done
                // token is of kind ifsym
                
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                
                // condition
                nextTokenNode = process_CONDITION(nextTokenNode);
                
                // if token <> "then" error
                if (m_nCurrentToken != thensym) {
                    printError(err16, "749 ");
                }
                // get current VM codeLine
                currentCodeLine = m_nCodeLineCount;
                // create a code line for VM
                enterCode(jpc, 0, 0);
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                //statement
                nextTokenNode = process_STATEMENT(nextTokenNode);
                // store the value of M after statement
                codeLines[currentCodeLine].M = m_nCodeLineCount;
                
                
                break;
                
            case whilesym: // done
                // token is of kind whilesym
                // at what line did while loop start
                beginWhileLine = m_nCodeLineCount;
                
                
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                //condition
                nextTokenNode = process_CONDITION(nextTokenNode);
                // at what line did while loop end
                endWhileLine = m_nCodeLineCount;
                // create a code line for VM
                enterCode(jpc, 0, 0);
                
                // if token <> "dosym" error
                if (m_nCurrentToken != dosym) {
                    printError(err18, " 767");
                }
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                //statement
                nextTokenNode = process_STATEMENT(nextTokenNode);
                enterCode(jmp, 0, beginWhileLine);
                // store the value of M after statement
                codeLines[endWhileLine].M = m_nCodeLineCount;
                
                
                break;
                
            case writesym: // done
                // generate
                // gettoken
                nextTokenNode = getNextTokenNode(nextTokenNode);
                if (m_nCurrentToken == identsym) {
                    nRecordFoundIndex = 0;
                    
                    // check if variable exist already as variable, constant
                    if ( !((nRecordFoundIndex = existVar(nextTokenNode->token)) || (nRecordFoundIndex = existConst(nextTokenNode->token))) ){
                        printError(err50, "836 ");
                    }
                    nRecordFoundIndex -= 1; // fix the offset of 1, i + 1
                    // get the record information
                    if (namerecord_table[nRecordFoundIndex].kind == 1) { // constant
                        enterCode(lit, 0, namerecord_table[nRecordFoundIndex].val);
                    }
                    if (namerecord_table[nRecordFoundIndex].kind == 2) { // variable
                        enterCode(lod, 0, namerecord_table[nRecordFoundIndex].adr);
                    }
                    // enter new VM code for SIO
                    enterCode(sio , 0, 0);
                    
                    nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                    nextTokenNode = getNextTokenNode(nextTokenNode); // after var or constant name
                    
                    //nextTokenNode = getNextTokenNode(nextTokenNode);
                    // create code (print top of stack OP, 0, 1)
                    break;
                }
                // token was not identifier
                printError(err50, "858 ");
                
                break;
                
            case readsym: // done
                ////printf(" %s token ar line 859, value %d\n", nextTokenNode->token, m_nCurrentToken );
                nextTokenNode = getNextTokenNode(nextTokenNode);
                
                // check if variable exist or not, if it does not exit, error11
                ////printf(" %s token ar line 863, value %d \n", nextTokenNode->token, m_nCurrentToken );
                if (m_nCurrentToken != identsym) {
                    printError(err49, "865 ");
                }
                
                nRecordFoundIndex = 0;
                if ( ! (nRecordFoundIndex = existVar(nextTokenNode->token)) ){
                    printError(err12, "870 ");
                    
                }
                // create a code line for VM read
                enterCode(sio, 0, 1);
                // create a code line for VM store
                enterCode(sto, 0, namerecord_table[nRecordFoundIndex].adr);
                
                break;
                
            case endsym:
                //retunextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            case periodsym:
                // nextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            default:
                // failed to read lexeme list
                printError(err35, "848 ");
                break;
        }
        
        return nextTokenNode = getNextTokenNode(nextTokenNode);;
        
    }
    
    // need to do error number here for failed to read lexemelist token
    printError(err35, "787 ");
    // always must return something, at this point nextTokenNode is NULL
    
    return nextTokenNode;
    
}

NODE *process_EXPRESSION(NODE *head){ // done
    
    int signOp = 0;
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        
        if ( (m_nCurrentToken == plussym) || (m_nCurrentToken == minussym) ) {
            // store the negative or possive mark
            signOp = m_nCurrentToken;
            nextTokenNode = getNextTokenNode(nextTokenNode);
            nextTokenNode = process_TERM(nextTokenNode);
            if (signOp == minussym){
                // create a code line for VM negation
                enterCode(opr, 0, neg);
            }
            
            
        } else {
            
            nextTokenNode = process_TERM(nextTokenNode);
        }
        
        while ( (m_nCurrentToken == plussym) || (m_nCurrentToken == minussym) ) {
            signOp = m_nCurrentToken;
            
            nextTokenNode = getNextTokenNode(nextTokenNode);
            
            nextTokenNode = process_TERM(nextTokenNode);
            if (signOp == minussym){
                // create a code line for VM negation
                enterCode(opr, 0, neg);
            } else {
                // create a code line for VM negation
                enterCode(opr, 0, add);
            }
        }
        
        return nextTokenNode;
    }
    
    // need to do error number here for failed to read lexemelist token
    printError(err35, "820 ");
    // always must return something
    return nextTokenNode;
}

NODE *process_TERM(NODE *head){ // done
    
    int nOp = 0;
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        // factor
        nextTokenNode = process_FACTOR(nextTokenNode);
        
        while ( (m_nCurrentToken == multsym) || (m_nCurrentToken == slashsym) ) {
            nOp = m_nCurrentToken;
            // get token
            nextTokenNode = getNextTokenNode(nextTokenNode);
            // factor
            nextTokenNode = process_FACTOR(nextTokenNode);
            if (nOp == multsym){
                // create a code line for VM negation
                enterCode(opr, 0, mul);
            } else {
                // create a code line for VM negation
                enterCode(opr, 0, div_);
            }
        }
        // return
        return nextTokenNode;
        
    }
    
    // need to do error number here for failed to read lexemelist token
    printError(err35, "846 ");
    // always must return something
    return nextTokenNode;
}

NODE *process_FACTOR(NODE *head){// done
    
    int nRecordFoundIndex = 0;
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        
        switch (m_nCurrentToken) {
                
            case INVALID_INT:
                
                // error reading the current token from getNextTokenNode
                printError(err35, "1004 ");
                break;
                
            case identsym:
                ////printf("factor, identsym\n");
                
                // check if variable exist or not, if it does not exit, error11
                
                if (  !(( nRecordFoundIndex = existVar(nextTokenNode->token)) || (nRecordFoundIndex = existConst(nextTokenNode->token)) )){
                    printError(err11, "line 1013 ");
                }
                if (namerecord_table[nRecordFoundIndex].kind == lexConstant){
                    enterCode(lit, 0, namerecord_table[nRecordFoundIndex].val);
                }
                if (namerecord_table[nRecordFoundIndex].kind == lexVar){
                    enterCode(lod  , 0, namerecord_table[nRecordFoundIndex].adr);
                }
                
                nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                
                // do i need to handle reading the variable at the return
                // get the variable name, need to do something with the variable name
                nextTokenNode = getNextTokenNode(nextTokenNode);
                break;
                
            case numbersym:
                
                nextTokenNode = getNextTokenNode(nextTokenNode); // skip
                enterCode(lit  , 0, m_nCurrentToken);
                
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
    
    int nOp = 0;
    
    NODE *nextTokenNode = NULL;
    nextTokenNode = head;
    
    if ( nextTokenNode != NULL){
        
        if (m_nCurrentToken == oddsym) {
            
            nOp = m_nCurrentToken;
            // gettoken
            nextTokenNode = getNextTokenNode(nextTokenNode);
            // expression
            nextTokenNode = process_EXPRESSION(nextTokenNode);
            
            enterCode(opr, 0, odd);
            
        } else {
            // expression
            nextTokenNode = process_EXPRESSION(nextTokenNode);
            // if token <> relation, error
            if ( (m_nCurrentToken < eqlsym) || (m_nCurrentToken > geqsym) ){
                printError(err20, " ");
            }
            nOp = m_nCurrentToken;
            // gettoken
            nextTokenNode = getNextTokenNode(nextTokenNode);
            // expression
            nextTokenNode = process_EXPRESSION(nextTokenNode);
            
            if(nOp == eqlsym)
                enterCode( opr, 0, eql ); // ==
            
            else if(nOp == neqsym )
                enterCode( opr, 0, neq ); // !=
            
            else if(nOp == lessym )
                enterCode( opr, 0, lss ); // <
            
            else if(nOp == leqsym )
                enterCode( opr, 0, leq ); // <=
            
            else if (nOp == gtrsym )
                enterCode( opr, 0, gtr ); // >
            
            else if(nOp == geqsym)
                enterCode( opr, 0, geq ); // >=
        }
        
        return nextTokenNode = getNextTokenNode(nextTokenNode);
        
    }
    
    // need to do error number here for failed to read lexemelist token
    printError(err35, "948 ");
    // always must return something
    return nextTokenNode;
}

/*
 *  initializeNamerecord_table(namerecord_t *record_table)
 *  set the default values for the namerecord_t record_table
 *  check that strcpy was valid for the char arrray
 */
void initializeNamerecord_table(){
    // initialize tokens record table
    int i = 0;
    for (i = 0; i < MAX_CODE_LENGTH ; i++) {
        
        namerecord_table[i].kind = 0;           // constant = 1; var = 2, proc = 3
        if ( strcpy(namerecord_table[i].name, "") == NULL) {
            printError(err33, "Namerecord_table");
            ////printf("Error: fail creating space to store token string\n");
            exit(EXIT_FAILURE);
        };                                  // name up to 11 characters long, 11 + 1 for \0
        namerecord_table[i].val = 0;            // number (ASCII value)
        namerecord_table[i].level = 0;          // L level
        namerecord_table[i].adr = 0;            // M address
    }
    return;
    
}

void enterNamerecord_table(int nKind, char caName[], int nVal, int nLevel, int nAdr ){
    
    
    namerecord_table[m_nNameRecordCount].kind = nKind;           // constant = 1; var = 2, proc = 3
    if ( strcpy(namerecord_table[m_nNameRecordCount].name, caName) == NULL) {
        printError(err33, "Namerecord_table");
        ////printf("Error: fail creating space to store token string\n");
        exit(EXIT_FAILURE);
    };                                                          // name up to 11 characters long, 11 + 1 for \0
    namerecord_table[m_nNameRecordCount].val = nVal;            // number (ASCII value)
    namerecord_table[m_nNameRecordCount].level = nLevel;        // L level
    namerecord_table[m_nNameRecordCount].adr = nAdr;            // M address
    m_nNameRecordCount++;
    
    return;
    
}

// ------------------end of analyze tokens ---------------------------

// ------------------code processing ------------------------

void enterCode(int nOPcode, int nLcode, int nMcode){
    
    // store the values into struct array
    codeLines[m_nCodeLineCount].OP = nOPcode;
    codeLines[m_nCodeLineCount].L = nLcode;
    codeLines[m_nCodeLineCount].M = nMcode;
    
    m_nCodeLineCount++;
    
    return;
}


/*
 *  printError(int ErrorNumber)
 *  Print the string that is represented by the error number passed
 */
void printError(int ErrorNumber, char *strToken){
    
    // print the error message given an error number from g_caErrorMsgs[] char* array
    if (ErrorNumber <= MAX_ERROR) {
        // to find error string, substract offset of 1
        printf("Error %d\n", ErrorNumber);
        //printf("Error %d, %s\n", ErrorNumber, g_caErrorMsgs[ErrorNumber - 1]);
        //printf("%s\n", strToken);
        return;
    }
    // clean up after using the read tokens, you need to free the calloc spaced
    // when you are done with it
    //if(gListHead != NULL) { FreeMemoryAllocFront_to_Tail(gListHead); gListHead = NULL;}
    
    exit(1);
    return;
    
}


















