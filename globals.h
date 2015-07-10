// TEAM

// First team member name  : Robert Valladares

// Second team member name : Danish Waheed

/**
 *  scanner.c
 *  Lexical Analizer
 *
 *  Created by Robert Valladares and Danish Waheed on 06/01/15.
 *  Copyright (c) 2015 ROBERT VALLADARES and DANISH WAHEED. All rights reserved.
 */
#ifndef __GLOBALS_H
#define __GLOBALS_H

// START vm.c parser.c globals ----------------------------------------------

struct instructions {
    int OP; /* OP_code */
    int L;	/* L_code */
    int M;	/* M_code */
};

char *OPCODES[] = {"---","lit","opr","lod","sto","cal","inc","jmp","jpc","sio"};
typedef enum {lit = 1,opr,lod,sto,cal,inc,jmp,jpc,sio} eOPCODE;

char *STACKOPS[] = {"ret","neg","add","sub","mul","div","odd","mod","eql","neq","lss","leq","gtr","geq"};
// div is a reserved word from the compiler in c, can not be redeclared, use div_ instead for enum
typedef enum {ret, neg, add, sub, mul, div_, odd, mod, eql, neq, lss, leq, gtr, geq} eSTACKOPS; // div is reserved word

// END vm.c parser.c globals ----------------------------------------------

// START scanner.c parser.c globals ----------------------------------------------


#define MAX_NUMBER_DIGITS 5                 // Defines how many digits an integer can have
#define MAX_IDENTIFIER_LENGTH 11            // Defines how long an identifier string can be
#define MAX_FILES   4 //6                   // Defines the count of files for input output
int m_nCleanCount = 0;                      // global variable to track count of characters in input
int m_nNameRecords = 0;                     // global variable to track count of NameRecord in input
#define MAX_PUNCT   13                      // maximum amount of special symbols
#define MAX_STR     256                     // maximum length of strings
int m_nCleanInputTokens = 0;                // global variable to track count of clean tokens in input
#define MAX_WORDS 15                        // define the number of reserved words
#define INVALID_INT 2147483647              // define the long_max int value in case number string is invalid
#define MAX_VAR_LEN 11                      // defines the masx length of a normal variable
#define MAX_ERROR 45                        // defines the max amount of error messages
#define MAX_ASCII 128                       // defines the max amount of ascii characters


//global strings for input output file names
char *LBLS[] = {"lexeme", "token type"};
char *FNS[] = {"input.txt", "cleaninput.txt", "lexemetable.txt", "lexemelist.txt"};
typedef enum {input_txt, cleaninput_txt, lexemetable_txt, lexemelist_txt} eFNS;
// file pointer array
FILE *m_FPS[MAX_FILES];

//------------------------- global data structures ------------------------

// string representation of the symbols, char array starts at index 0, compensate with offset of 1
// with lexeme values
char *m_sa_token_type[] = {
    "nulsym", "identsym", "numbersym", "plussym", "minussym", "multsym",
    "slashsym", "oddsym", "eqlsym", "neqsym", "lessym", "leqsym",
    "gtrsym", "geqsym", "lparentsym", "rparentsym", "commasym", "semicolonsym",
    "periodsym", "becomessym", "beginsym", "endsym", "ifsym", "thensym",
    "whilesym", "dosym", "callsym", "constsym", "varsym", "procsym",
    "writesym", "readsym", "elsesym" };


// enumerator of the symbols
typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym, multsym,
    slashsym, oddsym, eqlsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym,
    writesym, readsym, elsesym } token_type;

typedef enum {lexConstant = 1, lexVar, lexProc} eLexemeKind;

//structure of the symbol table record
typedef struct {
    
    int TokenType;      // token type
    int kind;           // constant = 1; var = 2, proc = 3
    char name[12];      // name up to 11 characters long, 11 + 1 for \0
    int val;            // number (ASCII value)
    int level;          // L level
    int adr;            // M address
    
} namerecord_t;


//  reserved words numerical representation from the token_type enum
char *saWsym[] = { "nulsym", "beginsym", "callsym", "constsym", "dosym", "elsesym", "endsym", "ifsym", "oddsym", "procsym", "readsym", "thensym", "varsym", "whilesym", "writesym"};

//  reserved words
char *word[] = {"null", "begin", "call", "const", "do", "else", "end", "if", "odd", "procedure", "read", "then", "var", "while", "write" };
//  reserved words numerical representation from the token_type enum
int m_naWsym[] = { nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym, ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym};

// special punctuation symbols
char m_caSpecialSymbols[] = {'(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>'};

// special punctuation symbols enumerator values
int m_naSpecialSymbols[] = {lparentsym, rparentsym, multsym, plussym, commasym, minussym, periodsym, slashsym,  becomessym,  semicolonsym, lessym, eqlsym, gtrsym};

// special punctuation ascii values
int m_naSpecSymPunt[] = {40, 41, 42, 43, 44, 45, 46, 47, 58, 59, 60, 61, 62};

// --------------ctype enums and structs --------------
typedef enum {AlphaTop = 52, PunctTop = 33, NumberTop = 10, SpecialCharTop = 13} eCharTops;

int m_naAlphaChars[] = {65, 66,67,68,69,70,71,72,73,74,
    75,76,77,78,79,80,81,82,83,84,
    85,86,87,88,89,90,97,98,99,100,
    101,102,103,104,105,106,107,108,109,110,
    111,112,113,114,115,116,117,118,119,120,
    121,122};

int m_naPunctChars[] = {9, 33,34,35,36,37,38,39,40,41,42,
    43,44,45,46,47, 58,59,60, 61,62,
    63,64,91,92,93,94,95,96,123,124,
    125,126};

int m_naNumericalChars[] = {48,49,50,51,52,53,54,55,56,57};

int g_naIsAlphaChar[MAX_ASCII];
int g_naIsPunctChar[MAX_ASCII];
int g_naIsNumerical[MAX_ASCII];
int g_naIsSpecialChar[MAX_ASCII];

// enumerator of the error numbers
typedef enum {
    err1 = 1, err2, err3, err4, err5, err6,
    err7, err8, err9, err10, err11, err12,
    err13, err14, err15, err16, err17, err18,
    err19, err20, err21, err22, err23, err24,
    err25, err26, err27, err28, err29, err30,
    err31, err32, err33, err34, err35, err36,
    err37, err38, err39, err40, err41, err42
    
} g_eErrorMsgs;

// error strings array, mostly used in parser for token sequence checking
char *g_caErrorMsgs[MAX_ERROR] = {
    
    "Use = instead of :=.",//1
    "= must be followed by a number.",//2
    "Identifier must be followed by =.",//3
    "const, var, procedure must be followed by identifier.",//4
    "Semicolon or comma missing.",//5
    "Incorrect symbol after procedure declaration.",//6
    "Statement expected.",//7
    "Incorrect symbol after statement part in block.",//8
    "Period expected.",//9
    "Semicolon between statements missing.",//10
    "Undeclared identifier.",//11
    "Assignment to constant or procedure is not allowed.",//12
    "Assignment operator expected.",//13
    "call must be followed by an identifier.",//14
    "Call of a constant or variable is meaningless.",//15
    "if condition must be followed by then",//  16
    "Semicolon or } expected.",//17
    "while condition must be followed by do",//18
    "Incorrect symbol following statement.",//19
    "Relational operator expected.",//20
    "Expression must not contain a procedure identifier.",//21
    "Right parenthesis missing.",//22
    "The preceding factor cannot begin with this symbol.",//23
    "An expression cannot begin with this symbol.",//24
    "This number is too large.",//25
    "Can not open file",//26
    "Can not read file",//27
    "Can not write to file",//28
    "*/ end of comment-block missing",//29
    "failed to allocate space using calloc",//30
    "invalid identsym",//31
    "invalid pair of symbols",//32
    "failed to initialized Namerecord_table",//33
    "failed to store lexeme in Namerecord_table"//34
    "failed to read lexemelist "//35
    "missing identifier"//36
    "declaration must end with ;"//37
    "missing procedure declaration ;"//38
    "procedure declaration must end with ;"//39
    "no ; at the end of block"//40
    ":= missing in statement" //41
    "begin must end with end" // 42
    "left parenthesis ( has not been closed" // 43
    "identifier, or number expected" // 44
    
    
};

// global function declaration
void printError(int ErrorNumber, char *strToken);
void gInitGlobalintIsCharArrays();

// END scanner.c parser.c globals ----------------------------------------------



#endif

