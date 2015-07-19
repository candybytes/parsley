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
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"                        // import global declarations from globals.h

int m_n_inputTokens = 0;                    // used to count the amount of tokens in lexemelist.txt
int m_nCurrentToken = INVALID_INT;          // used for gettoken, store the token numerical value
char m_cCurrentTokenStr[MAX_VAR_LEN + 1];   // used for gettoken, store the token string form
int m_nTokenConstant = 0;                   // used to store the constant value when is read from linkedlist

int m_nListIndex = 0;                       //  used as linkedlist index;
int m_nTokVarFlag = 0;                      // use to flag if token is variable or constant
int m_nVariableStackAdrx = 4;               // variables start of address of first AR
int m_nAR_Level = 0;                        // Activation Record first Level = 0;

struct NODE *gListHead = NULL;              // global list head for parsing
struct NODE *gFreeListHead = NULL;          // global list head in case of error printing, free malloc from this pointer

#define MAX_CODE_LENGTH 500                 // max amount of tokenized instructions symbols
int OTC = 0;                                // Output to Console Flag

int m_nVarCount = 0;                        // keep track of how many variables are declared
int m_nConstCount = 0;                      // keep track of how many constants are declared
int m_nProcCount = 0;                       // keep track of how many procedure are declared


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
namerecord_t namerecord_table[MAX_CODE_LENGTH];         // ARs token array
namerecord_t table;                                   // ARs token array pointer -------------***********
int m_nNameRecordCount = 0;                             // index for ARs token array

struct instructions codeLines[MAX_CODE_LENGTH];         // instructions array
int m_nCodeLineCount = 0;                               // index for code line genetator



//--------------------local data structures ---------------


typedef struct NODE{
    char token[MAX_VAR_LEN + 1];
    struct NODE *next;
} NODE;

// function declaration look ahead

NODE *NewNode(char str[]);                              // create a new linked list, new linkedlist node
NODE *InsertAtTail(NODE *head, char str[]);             // insert a new node at the tail of the list
void initializeNamerecord_table();                      // initialize the array of const var procs
void initializeInstructions();                          // initialize the array of const var procs

void FreeMemoryAllocFront_to_Tail();                    // free the space allocated to avoid seg fault

void procedure_PROGRAM();                               // analyze the read prgram
void getNextTokenNode();                                // update global int token m_nCurrentToken, and str token m_cCurrentTokenStr
void process_Block();                                   // BLOCK
void const_decl();                                      // procedure_PROGRAM sub procedure, constants declarations
void var_decl();                                        // procedure_PROGRAM sub procedure, variables declarations
void proc_decl();                                       // procedure_PROGRAM sub procedure, procedure declarations

void process_STATEMENT();                               // STATEMENT
void process_EXPRESSION();                              // EXPRESSION
void process_TERM();                                    // TERM
void process_FACTOR();                                  // FACTOR
void process_CONDITION();                               // CONDITION

int strsAreEqual(char * stt1, char *str2);              // are two string equa;
int existVar(char varName[]);                           // does a variable  by the name passed exist, return the index where found
int existConst(char constName[]);                       // does a constant  by the name passed exist, return the index where found
int existProc(char procName[]);                         // does a procedure by the name passed exist, return the index where found
void enterCode(int OPcode, int Lval, int Mval);         // enter the vm code line passed by values
void printCodeLines();                                  // print the code lines to screen

void printCodeLinesTOFILE();                            // print the vm code lines to output file mcode.txt
void printCodeLines();                                  // print the vm code lines to console, uses OTC flag
FILE *ifp = NULL;




// -----------------Initial call to program  -----------------
int main(int argc, char *argv[]) {
    int i = 0;
    
    
    ifp = fopen("lexemelist.txt", "r");
    if (ifp == NULL) {
        printError(err35, " ");
    }
    
    if(argc > 1) {
        OTC = strsAreEqual(argv[1], "-a");
        
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
        //printf("read token %s\n", iToken);
        ListHead = InsertAtTail(ListHead, iToken);
        
        m_n_inputTokens++;
        
    }
    printf("input tokens %d\n",  m_n_inputTokens);
    initializeNamerecord_table();
    initializeInstructions();
    
    //gFreeListHead = ListHead;
    //gListHead = ListHead;
    
    // close the input file
    fclose(ifp);
    /*/// ----------test print ------------- //
     
     i = 0;
     for (i = 0; i < m_n_inputTokens; i = m_nListIndex) {
     getNextTokenNode();
     }
     
     /*/// ----------test print ------------- //
    
    // call to analyse tokens -- parser
    printf("calling the program \n");
    procedure_PROGRAM();
    
    // clean up after using the read tokens, you need to free the calloc space
    // when you are done with it
    if(gFreeListHead != NULL) { FreeMemoryAllocFront_to_Tail(gFreeListHead); gFreeListHead = NULL;}
    
    
    if (OTC) {
        printCodeLines();
    }
    printf("\n\nNo errors, program is syntactically correct.\n\n");
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
        gFreeListHead = head;
        gListHead = head;
        // check if creating a new list failed
        if (head == NULL) {
            printError(err30, "192");
            
        }
        // if the head was null, a new list was created, return pointer to new list
        return head;
        
    }
    
    // create a new node to be added to the tail
    NODE *newTailNode = NULL;
    newTailNode = NewNode(str);
    if (newTailNode == NULL) {
        printError(err30, "192");
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


void procedure_PROGRAM(){
    
    getNextTokenNode();
    process_Block();
    // program should have ended after processing code block, look for periodsym
    if (m_nCurrentToken != periodsym) {
        printError(err9, NULL);
    }
    enterCode(sio, 0, 2);
    return;
}

void getNextTokenNode(){
    
    strcpy(m_cCurrentTokenStr,"");          // reset to  empty string "" the string token
    m_nTokenConstant = 0;         // reset the token constant value
    // check that the list, and tokens are withing limits
    if (gListHead != NULL && m_nListIndex < m_n_inputTokens) {
        // transform the token to its integer value
        m_nCurrentToken = atoi(gListHead->token);
        gListHead = gListHead->next;        // move the pointer of the linkedlist to next
        m_nListIndex++;                     // indicate the linkedlist index
        
        if (m_nCurrentToken == identsym) {  // is the token just read the indication of a string name
            // copy the string name
            strcpy(m_cCurrentTokenStr,gListHead->token);
            gListHead = gListHead->next;    // move the pointer of the linkedlist to next
            m_nListIndex++;                 // indicate the linkedlist index
        }
        
        if (m_nCurrentToken == numbersym) { // is the token just read the indication of a constant value
            // copy the constant value
            m_nTokenConstant = atoi(gListHead->token);
            gListHead = gListHead->next;    // move the pointer of the linkedlist to next
            m_nListIndex++;                 // indicate the linkedlist index
        }
        // test print
        printf("token %3d, variable %12s constant %12d listIndex %d\n", m_nCurrentToken, m_cCurrentTokenStr,  m_nTokenConstant, m_nListIndex  );
        return;
        
    }
    printError(err35, "327 ");              // error reading token from linked list
    
    return;
}

void process_Block(){
    
    // is current token a valid token?
    if (m_nCurrentToken == INVALID_INT || gListHead == NULL) {
        printError(err35, "367 ");
        return;
    }
    
    if (m_nCurrentToken == constsym){
        // token constant declaration constsym = 28
        const_decl();
    }
    
    if (m_nCurrentToken == varsym){
        // token is variable declaration = 29
        var_decl();
    }
    
    if (m_nCurrentToken == procsym){
        // token Procedure declaration = 30
        proc_decl();
    }
    
    process_STATEMENT();
    return;
    
}

void const_decl(){
    
    namerecord_t singleNamerecord;  // single Var, Const or Proc record
    
    if ( gListHead == NULL){
        // error reading the current token from getNextTokenNode
        printError(err35, "443 ");
        return;
    }
    
    do {
        
        getNextTokenNode();
        // token = 2
        if (m_nCurrentToken != identsym) {
            printError(err36, "375 ");
        }
        
        // check if constant exist already as variable, constant or procedure
        if ( existVar(m_cCurrentTokenStr) || existConst(m_cCurrentTokenStr) || existProc(m_cCurrentTokenStr) ){
            // error constant already declared as a variable or procedure name
            printError(err45, "384 ");
        }
        
        // copy the name of the constant
        strcpy(singleNamerecord.name, m_cCurrentTokenStr);
        // set the type of the constant
        singleNamerecord.kind = lexConstant;
        
        getNextTokenNode();
        
        if (m_nCurrentToken != eqlsym) {
            printError(err3, " 395");
        }
        // token = 3
        getNextTokenNode();
        
        if (m_nCurrentToken != numbersym) {
            printError(err2, "401 ");
        }
        // token = 7 (the number that follows 3)
        
        singleNamerecord.val = m_nTokenConstant;
        // set the address of the constant
        singleNamerecord.adr = -1;
        // set the lelvel of the constant
        singleNamerecord.level = m_nAR_Level;
        
        // store the created single Name record into the array
        namerecord_table[m_nNameRecordCount++] = singleNamerecord;
        m_nConstCount++;
        
        printf("constant 426::  %d, %s, %d\n", singleNamerecord.kind, singleNamerecord.name, singleNamerecord.val);
        
        // token could be coma or semicolon , ;
        getNextTokenNode();
        
    } while (m_nCurrentToken == commasym);
    
    if (m_nCurrentToken != semicolonsym) {
        printError(err37, "434 ");
    }
    
    getNextTokenNode();
    
    return;
    
    
}

void var_decl(){
    
    namerecord_t singleNamerecord;  // single Var, Const or Proc record
    
    if ( gListHead == NULL){
        // error reading the current token from getNextTokenNode
        printError(err35, "510 ");
        return;
    }
    
    do {
        getNextTokenNode();
        
        if (m_nCurrentToken != identsym) {
            printError(err36, "466 ");
        }
        
        if ( existVar(m_cCurrentTokenStr) || existConst(m_cCurrentTokenStr) || existProc(m_cCurrentTokenStr) ){
            // error variable already declared as a variable or procedure name
            printError(err46, "474 ");
        }
        
        // copy the name of the variable
        strcpy(singleNamerecord.name, m_cCurrentTokenStr);
        // set the type of the record
        singleNamerecord.kind = lexVar;
        
        // set the value of the variable
        singleNamerecord.val = 0;
        // set the address of the constant
        singleNamerecord.adr = m_nVariableStackAdrx;// + m_nVarCount++; //4
        // set the lelvel of the constant
        singleNamerecord.level = m_nAR_Level;
        
        // store the created single Name record into the array
        namerecord_table[m_nNameRecordCount++] = singleNamerecord;
        
        getNextTokenNode();
        
        printf("variable 495 :: %d, %s, %d\n", singleNamerecord.adr, singleNamerecord.name, singleNamerecord.level);
        m_nVariableStackAdrx++;
        
    } while (m_nCurrentToken == commasym );
    
    if (m_nCurrentToken != semicolonsym) {
        printError(err37, "502 ");
    }
    enterCode(inc, m_nAR_Level, 4 + m_nVarCount);
    getNextTokenNode();
    
    return;
    
}

void proc_decl(){
    
    namerecord_t singleNamerecord;  // single Var, Const or Proc record
    
    if ( gListHead == NULL){
        // error reading the current token from getNextTokenNode
        printError(err35, "583 ");
        return;
    }
    
    while (m_nCurrentToken == procsym) {
        
        // update the current token value and get the node pointer to next token
        getNextTokenNode();
        
        if (m_nCurrentToken != identsym) {
            printError(err38, "526 ");
        }
        
        // check if variable exist already as variable, constant or procedure
        if ( existVar(m_cCurrentTokenStr) || existConst(m_cCurrentTokenStr) || existProc(m_cCurrentTokenStr) ){
            printError(err47, "531 ");
        }
        m_nAR_Level++;  // increase the level for each new procedure call
        
        m_nProcCount++; // increase the count of procedures declarations in the program
        
        strcpy(singleNamerecord.name, m_cCurrentTokenStr); // copy the name of the procedure
        
        singleNamerecord.kind = lexProc; // set the type of the record
        
        singleNamerecord.val = 0; // set the value of the procedure
        
        singleNamerecord.adr = m_nVariableStackAdrx; // set the address of the procedure
        
        singleNamerecord.level = m_nAR_Level; // set the lelvel of the procedure
        
        // store the created single Name record into the array
        namerecord_table[m_nNameRecordCount++] = singleNamerecord;
        
        printf("procedure 549::kind %d, name %s addrx %d level %d\n", singleNamerecord.kind, singleNamerecord.name, singleNamerecord.adr, singleNamerecord.level);
        
        m_nVariableStackAdrx += 4; // increase the variables starting address
        
        getNextTokenNode();
        
        if (m_nCurrentToken != semicolonsym) {
            printError(err39, "553 ");
        }
        
        // get next token, increase the L level, call block procedure again
        getNextTokenNode();
        
        // ** when increasing the AR level, you must also increase the stack space
        process_Block();
        
        if (m_nCurrentToken != semicolonsym) {
            printError(err40, "566 ");
        }
        
        getNextTokenNode();
    }
    // return normally from procedure
    return;
    
}


void process_STATEMENT(){
    
    
    int currentCodeLine = 0;
    int beginWhileLine = 0;
    int endWhileLine = 0;
    int nRecordFoundIndex = 0;
    
    if ( gListHead == NULL){
        printError(err35, "573");
        return;
    }
    
    switch (m_nCurrentToken) {
            
        case identsym:      // token is of kind identsym
            
            if ( !( nRecordFoundIndex = existVar(m_cCurrentTokenStr) )){
                printError(err11, " 606");
            }
            // fix the offset return from existVar i + 1;
            nRecordFoundIndex -= 1;
            printf("variable %s address %d\n", namerecord_table[nRecordFoundIndex].name, namerecord_table[nRecordFoundIndex].adr);
            
            getNextTokenNode();
            // if token <> " := "   error
            if (m_nCurrentToken != becomessym) {
                printError(err41, "615 ");
            }
            // gettoken
            getNextTokenNode();
            // expression
            process_EXPRESSION();
            // create a code line for VM
            enterCode(sto, namerecord_table[nRecordFoundIndex].level, namerecord_table[nRecordFoundIndex].adr);
            
            break;
        case callsym:
            
            getNextTokenNode();
            // if token <> " identsym " error
            if (m_nCurrentToken != identsym) {
                printError(err36, " 635");
            }
            // check if procedure exist or not, if it does not exit, error11
            if ( ! existProc(m_cCurrentTokenStr)  ){
                printError(err48, "639 ");
            }
            
            getNextTokenNode();
            
            break;
            
        case beginsym:
            
            getNextTokenNode();
            process_STATEMENT();
            
            while (m_nCurrentToken == semicolonsym) {
                getNextTokenNode();
                process_STATEMENT();
            }
            // if token <> "end" error
            if (m_nCurrentToken != endsym) {
                printError(err42, "654 ");
            }
            
            getNextTokenNode();
            break;
            
        case ifsym:
            
            // token is of kind ifsym
            getNextTokenNode();
            // condition
            process_CONDITION();
            // if token <> "then" error
            if (m_nCurrentToken != thensym) {
                printError(err16, "667 ");
            }
            // get current VM codeLine
            currentCodeLine = m_nCodeLineCount;
            // create a code line for VM
            enterCode(jpc, 0, 0);
            // gettoken
            getNextTokenNode();
            //statement
            process_STATEMENT();
            // store the value of M after statement
            codeLines[currentCodeLine].M = m_nCodeLineCount;
            
            break;
            
        case whilesym:
            
            // token is of kind whilesym
            // at what line did while loop start
            beginWhileLine = m_nCodeLineCount;
            // gettoken
            getNextTokenNode();
            //condition
            process_CONDITION();
            // at what line did while loop end
            endWhileLine = m_nCodeLineCount;
            // create a code line for VM
            enterCode(jpc, 0, 0);
            
            // if token <> "dosym" error
            if (m_nCurrentToken != dosym) {
                printError(err18, " 697");
            }
            // gettoken
            getNextTokenNode();
            //statement
            process_STATEMENT();
            enterCode(jmp, 0, beginWhileLine);
            // store the value of M after statement
            codeLines[endWhileLine].M = m_nCodeLineCount;
            
            break;
            
        case writesym:
            
            getNextTokenNode();
            
            if (m_nCurrentToken != identsym) {
                // token was not identifier
                printError(err50, "858 ");
            }
            nRecordFoundIndex = 0;
            
            // check if variable exist already as variable, constant
            if ( (nRecordFoundIndex = existVar(m_cCurrentTokenStr)) || (nRecordFoundIndex = existConst(m_cCurrentTokenStr)) ){
                
                nRecordFoundIndex -= 1; // fix the offset of 1, i + 1 from the return
                // get the record information
                if (namerecord_table[nRecordFoundIndex].kind == lexConstant) { // constant
                    enterCode(lit, 0, namerecord_table[nRecordFoundIndex].val);
                }
                if (namerecord_table[nRecordFoundIndex].kind == lexVar) { // variable
                    enterCode(lod, 0, namerecord_table[nRecordFoundIndex].adr);
                }
                // enter new VM code for SIO
                enterCode(sio , 0, 0);
                
                getNextTokenNode();
                break;
                
            } else {
                printError(err51, "734 ");
                break;
            }
            
            break;
    } // end switch
    
    
}

void process_EXPRESSION(){
    
    int nOp = 0;
    
    if ( gListHead == NULL){
        // need to do error number here for failed to read lexemelist token
        printError(err35, "708 ");
        // always must return something
        return;
    }
    
    if ( (m_nCurrentToken == plussym) || (m_nCurrentToken == minussym) ) {
        // store the negative or possive mark
        nOp = m_nCurrentToken;
        
        getNextTokenNode();
        
        process_TERM(); // TERM
        
        if (nOp == minussym){
            // create a code line for VM negation
            enterCode(opr, 0, neg); // this is a negation
        }
        
    } else {
        
        process_TERM(); // TERM
    }
    
    while ( (m_nCurrentToken == plussym) || (m_nCurrentToken == minussym) ) {
        
        nOp = m_nCurrentToken;
        
        getNextTokenNode();
        
        process_TERM(); // TERM
        enterCode(opr, 0, g_naArithOPLookup[nOp]);
        
        /*
         if (nOp == minussym){
         // create a code line for VM negation
         enterCode(opr, 0, sub);
         } else {
         // create a code line for VM negation
         enterCode(opr, 0, add);
         }
         */
    }
    
    return;
    
}

void process_TERM(){
    
    int nOp = 0;
    
    if ( gListHead == NULL){
        // need to do error number here for failed to read lexemelist token
        printError(err35, "741");
        // always must return something
        return;
        
    }
    // factor
    process_FACTOR();
    
    while ( (m_nCurrentToken == multsym) || (m_nCurrentToken == slashsym) ) {
        nOp = m_nCurrentToken;
        // get token
        getNextTokenNode();
        
        process_FACTOR(); // FACTOR
        
        enterCode(opr, 0, g_naArithOPLookup[nOp]);
        /*
         if (nOp == multsym){
         // create a code line for VM negation
         enterCode(opr, 0, mul);
         } else {
         // create a code line for VM negation
         enterCode(opr, 0, div_);
         }
         */
    }
    // return to TERM caller
    return;
}

void process_FACTOR(){
    
    int nRecordFoundIndex = 0;
    
    
    if ( gListHead == NULL){
        // need to do error number here for failed to read lexemelist token
        printError(err35, "810");
        // always must return something
        return;
    }
    
    switch (m_nCurrentToken) {
            
        case identsym:
            
            // check if variable exist or not, if it does not exit, error11
            
            if ( (nRecordFoundIndex = existVar(m_cCurrentTokenStr)) || (nRecordFoundIndex = existConst(m_cCurrentTokenStr)) ){
                nRecordFoundIndex -= 1; // adjust the offset from existVar or existConst return of i + 1 ;
                
                if (namerecord_table[nRecordFoundIndex].kind == lexConstant){
                    enterCode(lit, 0, namerecord_table[nRecordFoundIndex].val);
                }
                
                if (namerecord_table[nRecordFoundIndex].kind == lexVar){
                    enterCode(lod  , 0, namerecord_table[nRecordFoundIndex].adr);
                }
                
            } else {
                // variable or constant is not declared
                printError(err11, "Factor > Identsym ");
            }
            
            getNextTokenNode();
            
            break;
            
        case numbersym:
            
            enterCode(lit  , 0, m_nCurrentToken);
            
            getNextTokenNode();
            
            break;
        case lparentsym:
            
            getNextTokenNode();
            
            process_EXPRESSION(); // EXPRESSION
            
            if (m_nCurrentToken != rparentsym) {
                printError(err43, "794 ");
            }
            
            getNextTokenNode();
            
            break;
            
        default:
            printError(err44, "912 ");
            break;
            
    }
    
    return;
}

void process_CONDITION(){
    
    return;
}

void initializeInstructions(){
    // initialize instructions table to default 0's
    int i = 0;
    for (i = 0; i < MAX_CODE_LENGTH ; i++) {
        codeLines[i].OP = 0;
        codeLines[i].L = 0;
        codeLines[i].M = 0;
    }
    
    return;
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
        };                                      // name up to 11 characters long, 11 + 1 for \0
        namerecord_table[i].val = 0;            // number (ASCII value)
        namerecord_table[i].level = 0;          // L level
        namerecord_table[i].adr = 0;            // M address
    }
    return;
    
}

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
    // if the two strings are equal, returns 1, else returns 0
    return strcmp(stt1, str2) == 0;
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

void printCodeLines(){
    int i = 0;
    printf("VM Code Lines mcode.txt\n");
    printf("%3s%3s%3s\n","OP","L","M");
    for (i = 0; i < m_nCodeLineCount; i++) {
        printf("%3d%3d%3d\n", codeLines[i].OP, codeLines[i].L, codeLines[i].M);
    }
    
}

void printCodeLinesTOFILE(){
    
    FILE * fp = NULL;
    fp = fopen("mcode.txt","w");
    int i = 0;
    for (i = 0; i < m_nCodeLineCount; i++) {
        fprintf(fp,"3%d3%d3%d\n", codeLines[i].OP, codeLines[i].L, codeLines[i].M);
    }
    fprintf(fp,"\n");
    // close the file
    fclose(fp);
    
}

/*
 *  printError(int ErrorNumber)
 *  Print the string that is represented by the error number passed
 */
void printError(int ErrorNumber, char *strToken){
    
    // print the error message given an error number from g_caErrorMsgs[] char* array
    if (ErrorNumber <= MAX_ERROR) {
        // to find error string, substract offset of 1
        
        printf("Error %d, %s %s\n", ErrorNumber, g_caErrorMsgs[ErrorNumber - 1], strToken);
        
    } else {
        printf("Error %d, Error # not defined\n", ErrorNumber);
        
    }
    // clean up after using the read tokens, you need to free the calloc spaced
    // when you are done with it
    if(gFreeListHead != NULL) { FreeMemoryAllocFront_to_Tail(gFreeListHead); gFreeListHead = NULL;}
    
    exit(1); // exit 0 to terminate immediatly the program
    return;
    
}













