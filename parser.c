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
#include "globals.h"    // for this demo because I am using MAX_VAR_LEN from global.h

int inputTokens = 0;    // used to count the amount of tokens in lexemelist.txt

//--------------------local data structures ---------------


typedef struct NODE{
    char token[MAX_VAR_LEN + 1];
    struct NODE *next;
} NODE;

// function declaration look ahead
int sumEveryOtherNode(NODE *head);
NODE *NewNode(char str[]);
NODE *InsertAtTail(NODE *head, char str[]);
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
        inputTokens++;
        
    }
    
    // ----------test print ------------- //
    printf("%d tokens \n", inputTokens);
    
    NODE *temp = NULL;
    temp = ListHead;
    for (i =0; i < inputTokens; i++) {
        printf("token %s\n", temp->token);
        temp =  temp->next != NULL ? temp->next : temp;
        
    }
    // ----------test print ------------- //
    
    // clean up after using the read tokens, you need to free the calloc spaced
    // when you are done with it
    if(ListHead != NULL) { FreeMemoryAllocFront_to_Tail(ListHead); ListHead = NULL;}
    
    // close the input file
    fclose(ifp);
    
    
    return 0;
}

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
    
}
