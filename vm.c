// TEAM

// First team member name  : Robert Valladares

// Second team member name : Danish Waheed

/**
 *  vm.c
 *  PM/0 virtual machine
 *
 *  Created by Robert Valladares and Danish Waheed on 5/20/15.
 *  Copyright (c) 2015 ROBERT VALLADARES and DANISH WAHEED. All rights reserved.
 */

/** Library declarations  */
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"


/** global constants and
 * given constants by assigment */

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
int OTC = 0;
int codeCount = 0;
int codeLine = -1;
int RUN = 1;
int prevSP = 0;
// register variables
// bp base pointer, sp stack pointer, pc program counter
int bp = 1;
int sp = 0;
int pc = 0;

// global strings for input output file names
#define IFS "mcode.txt"
#define OFS "stacktrace.txt"

/** Read ahead function prototypes */
void readInputFile(char fileName[]);
void outCodeLinesToFile();
void exeInstruction();
void outStackAndRegistersToFile();
char *getOpCode(int OPcode);
int base(int level, int b);
void exeStackOP();
void initGlobalArrays();
void endOfProgram();

// added july 12 to combine with driver
void outStackAndRegistersToConsole();
void outCodeLinesToConsole();
int strsAreEqual(char * stt1, char *str2);


/** global data structures */
/*
 struct instructions {
 int OP; // OP_code
 int L;	// L_code
 int M;	// M_code
 };
 */

struct instructions code[MAX_CODE_LENGTH];
struct instructions IR;
int stack[MAX_STACK_HEIGHT];


/*
 char *OPCODES[] = {"---","lit","opr","lod","sto","cal","inc","jmp","jpc","sio"};
 typedef enum {lit = 1,opr,lod,sto,cal,inc,jmp,jpc,sio} eOPCODE;
 */


char *LABELS[] = { "Line", "OP", "L", "M", "pc", "bp", "sp", "stack", "Initial values" };
typedef enum { pop, read, halt } eSIO;

/*
 char *STACKOPS[] = {"ret","neg","add","sub","mul","div","odd","mod","eql","neq","lss","leq","gtr","geq"};
 // div is a reserved word from the compiler in c, can not be redeclared, use div_ instead for enum
 typedef enum {ret, neg, add, sub, mul, div_, odd, mod, eql, neq, lss, leq, gtr, geq} eSTACKOPS; // div is reserved word
 */
int ARdiv[4 * MAX_LEXI_LEVELS];

/*************** Initial call to program  ***********/
int main(int argc, char *argv[]) {
    
    char *filename = NULL;
    if (argc > 1) {
        OTC = strsAreEqual(argv[1], "-v");
        
    }

   
    filename = IFS;
    
    // read the input from file
    readInputFile(filename);
    
    
    // set up initial IR values
    IR.OP = 0;
    IR.L = 0;
    IR.M = 0;
    // print Code lines to file
    outCodeLinesToFile();
    if (OTC) {
        
        printf("\nvirtual machine execution trace\n\n");
        outCodeLinesToConsole();
    }
    
    
    int x = 0; int y = 0;
    // declare a reg[bp][sp] array
    // initialize array to 0's
    int bpsp[codeCount][2];
    for (x = 0; x < codeCount; x++){
        for (y = 0; y < 2; y++){
            bpsp[x][y] = 0;
        }
    }
    
    int ARplace = 0;
    bpsp[0][0] = bp;
    int i = 0;
    for (i = 0; i < codeCount; i++){
        outStackAndRegistersToFile();
        // if output to console instruction is true print stact trace to console
        if (OTC) { outStackAndRegistersToConsole(); }
        // store the values of the bp and sp to check for
        // changes on the activation record
        if (codeLine >= 0){
            bpsp[i][0] = bp;
            bpsp[i][1] = sp;
        }
        
        if (i > 0 && bpsp[i - 1][0] != bpsp[i][0]){
            // if there is a change in the AR value aka "bp"
            // capture the previous sp value and store it
            ARdiv[ARplace++] = bpsp[i - 1][1];
        }
        
        if (RUN){
            exeInstruction();
        }
        else {
            endOfProgram();
            exit(EXIT_SUCCESS);
        }
    }
    
    endOfProgram();
    return 0;
    
}
/**
 * void initGlobalArrays()
 * initialize global arrays, in c language
 * arrays can have garbage if not normalized
 */
void initGlobalArrays(){
    // use ARdiv[] array to store the | places
    int k = 0;
    for (k = 0; k < (4 * MAX_LEXI_LEVELS); k++){
        ARdiv[k] = 0;
    }
    
    for (k = 0; k < MAX_STACK_HEIGHT; k++){
        stack[k] = 0;
    }
    
    return;
    
}

/**
 * "readInputFile(char fileName[])"
 * reads code from file
 * code is given in integer values
 * then store into a structure array
 *
 */
void readInputFile(char fileName[]){
    
    if (fileName != NULL){
        FILE *fpt;
        fpt = fopen("mcode.txt", "r");
        
        if (!fpt) {
            printf("Warning!, Could not open input file %s\n", IFS);
            exit(EXIT_FAILURE);
        }
        int nCodeIndex = 0;
        int nOPcode;
        int nLcode;
        int nMcode;
        
        while (fscanf(fpt, "%d", &nOPcode) != EOF){
            // check if amount of instructions is greater than code
            // array size
            if (nCodeIndex > MAX_CODE_LENGTH - 1){
                printf("Warning!, the instructions read are greater than the size o the code array \n");
                exit(EXIT_FAILURE);
            }
            // read the values from file, op code has been read above
            fscanf(fpt, "%d", &nLcode);
            fscanf(fpt, "%d", &nMcode);
            
            // store the values into struct array
            code[nCodeIndex].OP = nOPcode;
            code[nCodeIndex].L = nLcode;
            code[nCodeIndex].M = nMcode;
            // increment the index counter
            nCodeIndex++;
        }
        codeCount = nCodeIndex;
        fclose(fpt);
        
    }
    return;
}

/**
 * Execute the codeline instruction
 * exeInstruction()
 * update registry values based on operation
 */
void exeInstruction(){
    // Fetch Instruction
    // PC is initially set to 0, then IR
    // would contain first instruction and
    // fetch following intruction after that
    codeLine = pc;
    IR = code[pc];
    pc++;
    
    eOPCODE eOP = IR.OP;
    eSIO esio = IR.M;
    
    // update previous AR stack pointer sp
    if (bp == 1) { prevSP = sp; }
    // {lit = 1,opr,lod,sto,cal,inc,jmp,jpc,sio}
    // execute instruction
    
    switch (eOP){
        case lit:
            // increment the stack pointer by 1
            // Push value M onto stack
            sp += 1;
            stack[sp] = IR.M;
            break;
        case opr:
            // arithmetic or logical operations on the stack
            // TO DO  add stack operations call here
            exeStackOP();
            break;
        case lod:
            // Get value at offset M in frame L levels down and push it
            sp += 1;
            stack[sp] = stack[base(IR.L, bp) + IR.M];
            break;
        case sto:
            // Pop stack and insert value at offset M in frame L levels down
            stack[base(IR.L, bp) + IR.M] = stack[sp];
            sp -= 1;
            break;
        case cal:
            // Call procedure at M (generates new stack frame)
            stack[sp + 1] = 0;		/* return value FV */
            stack[sp + 2] = base(IR.L, bp); /* static link SL */
            stack[sp + 3] = bp;		/* return value FV */
            stack[sp + 4] = pc;		/* return value FV */
            bp = sp + 1;
            pc = IR.M;
            break;
        case inc:
            // Allocate M locals on stack
            sp += IR.M;
            break;
        case jmp:
            // Jump to M
            pc = IR.M;
            
            break;
        case jpc:
            // Pop stack and jump to M if value is equal to 0
            if (stack[sp] == 0){ pc = IR.M; }
            sp -= 1;
            break;
        case sio:
            // M = 0 Pop stack and print out value
            // M = 1 Read in input from user and push it
            // M = 2 Halt the machine
            switch (esio){
                    
                case pop:
                    printf("%d\n", stack[sp]);
                    sp -= 1;
                    break;
                case read:
                    
                    sp -= 1;
                    int i = 0;
                    printf("Enter an SIO code into stack:");
                    scanf("%d", &i);
                    stack[sp] = i;
                    break;
                case halt:
                    // update the RUN code to false
                    // to prevent the code from executing next line
                    RUN = 0;
                    break;
                default:
                    
                    printf("Terminating Error! Incorrect SIO code: %d at line: %d\n", esio, codeLine);
                    exit(EXIT_FAILURE);
                    
            } // end of sio operations
            
            break;
        default:
            
            printf("Terminating Error! Incorrect OPCode: %d at line: %d\n", IR.OP, codeLine);
            exit(EXIT_FAILURE);
            
    }
}

/**
 * Execute the stack OPR instruction
 * exeStackOP()
 * update registry values and stack based on operation
 */
void exeStackOP(){
    
    // evaluate stack operation from IR.M
    /* {ret = 0, neg, add, sub, mul, div_, odd, mod,
     * eql, neq, lss, leq, gtr, geq} eSTACKOPS;
     */
    
    eSTACKOPS eSTAOP = IR.M;
    // execute instruction
    switch (eSTAOP){
        case ret:
            sp = bp - 1; pc = stack[sp + 4]; bp = stack[sp + 3];
            break;
        case neg:
            stack[sp] *= -1;
            break;
        case add:
            sp -= 1; stack[sp] += stack[sp + 1];
            break;
        case sub:
            sp -= 1; stack[sp] -= stack[sp + 1];
            break;
        case mul:
            sp -= 1; stack[sp] *= stack[sp + 1];
            break;
        case div_:
            sp -= 1; stack[sp] /= stack[sp + 1];
            break;
        case odd:
            stack[sp] %= 2;
            break;
        case mod:
            sp -= 1; stack[sp] %= stack[sp + 1];
            break;
        case eql:
            sp -= 1; stack[sp] = stack[sp] == stack[sp + 1];
            break;
        case neq:
            sp -= 1; stack[sp] = stack[sp] != stack[sp + 1];
            break;
        case lss:
            sp -= 1; stack[sp] = stack[sp] < stack[sp + 1];
            break;
        case leq:
            sp -= 1; stack[sp] = stack[sp] <= stack[sp + 1];
            break;
        case gtr:
            sp -= 1; stack[sp] = stack[sp] > stack[sp + 1];
            break;
        case geq:
            sp -= 1; stack[sp] = stack[sp] >= stack[sp + 1];
            break;
        default:
            // invalid stack operation was passed, exit and terminate
            printf("Terminating Error! Incorrect Stack Code: %d at line: %d\n", eSTAOP, codeLine);
            exit(EXIT_FAILURE);
    }
}

/**
 * int base(int level, int b);
 * Find base L levels down
 */
int base(int level, int b){
    while (level > 0){
        b = stack[b + 2];
        level--;
    }
    return b;
}

/**
 * "outCodeLinesToFile()"
 * Output the code list to file
 * assumes file does not exist
 * or that it will write to file first
 *
 */
void outCodeLinesToFile(){
    
    int i = 0;
    FILE *fpt;
    fpt = fopen(OFS, "w");
    
    if (!fpt) {
        printf("Warning!, Could not create or open output file %s\n", OFS);
        exit(EXIT_FAILURE);
    }
    
    // print head labels
    fprintf(fpt, "%-6s%-5s%-4s%-4s\n", LABELS[0], LABELS[1], LABELS[2], LABELS[3]);
    // print to file the code lines from the code array
    for (i = 0; i < codeCount; i++){
        fprintf(fpt, "%-6d%-5s%-4d%-4d\n", i, getOpCode(code[i].OP), code[i].L, code[i].M);
    }
    fprintf(fpt, "\n");
    // close the file
    fclose(fpt);
    return;
}
/**
 * "*getOpCode(int OPcode)"
 * return a string value of the
 * opcode, it the passed parameter
 * is invalid, return an empty string
 *
 */
char *getOpCode(int OPcode){
    // check if OPcode is out of range
    if (OPcode < 0 || OPcode > 9){
        return OPCODES[0];
    }
    // return the corresponding string to the OPcode value
    return OPCODES[OPcode];
}

/**
 * "outStackAndRegistersToFile()"
 * appends stack and registers values to file
 * file should already exist and contain code lines
 * on it
 */
void outStackAndRegistersToFile(){
    
    FILE *fpt;
    fpt = fopen(OFS, "a");
    int stackPrint = 1;
    int arplace = 0;
    
    if (!fpt) {
        // TO-DO, insert line number for warning!!!!
        printf("Warning!, Could not create or open output file %s\n", OFS);
        exit(EXIT_FAILURE);
    }
    
    
    if (codeLine < 0){
        // print the first row of labels
        fprintf(fpt, "%-19s%-4s%-4s%-4s%s\n", "",
                LABELS[4], LABELS[5], LABELS[6], LABELS[7]);
        
        // print the second row of labels and the values of the registers
        // %-19s is the width of the first 4 columns combined
        fprintf(fpt, "%-19s%-4d%-4d%-4d\n", LABELS[8], pc, bp, sp);
    }
    else {
        // else the value of the nLine is at least line 0 or greater
        // print the code lines
        
        fprintf(fpt, "%-6d%-5s%-4d%-4d", codeLine, getOpCode(code[codeLine].OP), code[codeLine].L, code[codeLine].M);
        // print the registers.
        fprintf(fpt, "%-4d%-4d%-4d", pc, bp, sp);
        // print the stack.
        while (stackPrint <= sp){
            fprintf(fpt, "%-4d", stack[stackPrint]);
            // check if the AR record separation is current
            // the AR place will never be 0
            if (ARdiv[arplace] == stackPrint && (stackPrint + 1) < sp){
                if (ARdiv[arplace] != 0){
                    fprintf(fpt, "%-4s", "|");
                }
                arplace++;
            }
            
            stackPrint++;
        }
        fprintf(fpt, "\n");
    }
    // close the file pointer
    fclose(fpt);
    
}


/**
 * "endOfProgram()"
 * it prints a end of program message to console
 * it lets user know that output file OFS has been created
 *
 */
void endOfProgram(){
    // not used in assigment 3
    //printf("%s has been successfully created with the output!\n", OFS);
}


int strsAreEqual(char * stt1, char *str2){
    
    return strcmp(stt1, str2) == 0;
    
}


/**
 * "outStackAndRegistersToConsole()"
 *  prints the stack trace to console
 */
void outStackAndRegistersToConsole(){
    
    
    int stackPrint = 1;
    int arplace = 0;
    
    
    if (codeLine < 0){
        // print the first row of labels
        printf("%-19s%-4s%-4s%-4s%s\n", "",
               LABELS[4], LABELS[5], LABELS[6], LABELS[7]);
        
        // print the second row of labels and the values of the registers
        // %-19s is the width of the first 4 columns combined
        printf("%-19s%-4d%-4d%-4d\n", LABELS[8], pc, bp, sp);
    }
    else {
        // else the value of the nLine is at least line 0 or greater
        // print the code lines
        
        printf("%-6d%-5s%-4d%-4d", codeLine, getOpCode(code[codeLine].OP), code[codeLine].L, code[codeLine].M);
        // print the registers.
        printf("%-4d%-4d%-4d", pc, bp, sp);
        // print the stack.
        while (stackPrint <= sp){
            printf("%-4d", stack[stackPrint]);
            // check if the AR record separation is current
            // the AR place will never be 0
            if (ARdiv[arplace] == stackPrint && (stackPrint + 1) < sp){
                if (ARdiv[arplace] != 0){
                    printf("%-4s", "|");
                }
                arplace++;
            }
            
            stackPrint++;
        }
        
    }
    printf("\n");
    
}


/**
 * "outCodeLinesToConsole()"
 *  print the code lines to console
 *
 */
void outCodeLinesToConsole(){
    
    int i = 0;
    
    // print head labels
    printf("%-6s%-5s%-4s%-4s\n", LABELS[0], LABELS[1], LABELS[2], LABELS[3]);
    // print to file the code lines from the code array
    for (i = 0; i < codeCount; i++){
        printf("%-6d%-5s%-4d%-4d\n", i, getOpCode(code[i].OP), code[i].L, code[i].M);
    }
    printf("\n");
    
    return;
}






