/**
 * Assignment 4 stores commands in memory in an array and linked list. goal is to have no memory leaks
 * Vivian Letran
 * vivian.letran@sjsu.edu
 * Creation : 04/02/23
 *  Modified: 04/10/23
 */
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define MAX_COMMANDS 100
#define MAX_LENGTH 31

typedef struct node node;

struct node {
    int lineNum;
    char* string;
    node* next;
};

void insertNode(node** head, node** tail, node* new);
void printNodes(node* head);
void freeList(node* head);

/**
*CS149 assignment#4 helper code.
* See the TODO's in the comments below! You need to implement those.
**/
/**
* TRACE_NODE_STRUCT is a linked list of
* pointers to function identifiers
* TRACE_TOP is the head of the list is the top of the stack
**/
struct TRACE_NODE_STRUCT {
    char* functionid; // ptr to function identifier (a function name)
    struct TRACE_NODE_STRUCT* next; // ptr to next frama
};
typedef struct TRACE_NODE_STRUCT TRACE_NODE;
static TRACE_NODE* TRACE_TOP = NULL; // ptr to the top of the stack

/* --------------------------------*/
/* function PUSH_TRACE */
/*
* The purpose of this stack is to trace the sequence of function calls,
* just like the stack in your computer would do.
* The "global" string denotes the start of the function call trace.
* The char *p parameter is the name of the new function that is added to the call
trace.
* See the examples of calling PUSH_TRACE and POP_TRACE below
* in the main, make_extend_array, add_column functions.
**/
void PUSH_TRACE(char* p) // push p on the stack
{
    TRACE_NODE* tnode;
    static char glob[]="global";
    if (TRACE_TOP==NULL) {
        // initialize the stack with "global" identifier
        TRACE_TOP=(TRACE_NODE*) malloc(sizeof(TRACE_NODE));
        
        // no recovery needed if allocation failed, this is only
        // used in debugging, not in production
        if (TRACE_TOP==NULL) {
            printf("PUSH_TRACE: memory allocation error\n");
            exit(1);
        }
        TRACE_TOP->functionid = glob;
        TRACE_TOP->next=NULL;
    }//if

    // create the node for p
    tnode = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));
    
    // no recovery needed if allocation failed, this is only
    // used in debugging, not in production
    if (tnode==NULL) {
        printf("PUSH_TRACE: memory allocation error\n");
        exit(1);
    }//if
    tnode->functionid=p;
    tnode->next = TRACE_TOP; // insert fnode as the first in the list
    TRACE_TOP=tnode; // point TRACE_TOP to the first node
}/*end PUSH_TRACE*/

/* --------------------------------*/
/* function POP_TRACE */
/* Pop a function call from the stack */
void POP_TRACE() // remove the op of the stack
{
    TRACE_NODE* tnode;
    tnode = TRACE_TOP;
    TRACE_TOP = tnode->next;
    free(tnode);
}/*end POP_TRACE*/

/* ---------------------------------------------- */
/* function PRINT_TRACE prints out the sequence of function calls that are on the
stack at this instance */
/* For example, it returns a string that looks like: global:funcA:funcB:funcC. */
/* Printing the function call sequence the other way around is also ok:
funcC:funcB:funcA:global */
char* PRINT_TRACE()
{
    int depth = 50; //A max of 50 levels in the stack will be combined in a string for printing out.
    int i, length, j;
    TRACE_NODE* tnode;
    static char buf[100];
    if (TRACE_TOP==NULL) { // stack not initialized yet, so we are
        strcpy(buf,"global"); // still in the `global' area
        return buf;
    }
    /* peek at the depth(50) top entries on the stack, but do not
    go over 100 chars and do not go over the bottom of the
    stack */
    sprintf(buf,"%s",TRACE_TOP->functionid);
    length = strlen(buf); // length of the string so far
    for(i=1, tnode=TRACE_TOP->next; tnode!=NULL && i < depth; i++,tnode=tnode->next) {
        j = strlen(tnode->functionid); // length of what we want to add
        if (length+j+1 < 100) { // total length is ok
            sprintf(buf+length,":%s",tnode->functionid);
            length += j+1;
        }else // it would be too long
        break;
    }
    return buf;
} /*end PRINT_TRACE*/

// -----------------------------------------
// function REALLOC calls realloc
// REALLOC should also print info about memory usage.
// For this purpose, you need to add a few lines to this function.
// For instance, example of print out:
// "File mem_tracer.c, line X, function F reallocated the memory segment at address A to a new size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* REALLOC(void* p,int t,char* file,int line)
{
    p = realloc(p,t);
    fprintf(stdout, "File %s, line %d, function %s reallocated the memory segment at address %p to a new size %d.\n", 
        file, line, PRINT_TRACE(), p, t);
    return p;
}

// -------------------------------------------
// function MALLOC calls malloc
// MALLOC should also print info about memory usage.
// For this purpose, you need to add a few lines to this function.
// For instance, example of print out:
// "File mem_tracer.c, line X, function F allocated new memory segment at address A to size S"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void* MALLOC(int t,char* file,int line)
{
    void* p;
    p = malloc(t);
    fprintf(stdout, "File %s, line %d, function %s allocated the memory segment at address %p to size %d.\n", 
        file, line, PRINT_TRACE(), p, t);
    return p;
}

// ----------------------------------------------
// function FREE calls free
// FREE should also print info about memory usage.
// For this purpose, you need to add a few lines to this function.
// For instance, example of print out:
// "File mem_tracer.c, line X, function F deallocated the memory segment at address A"
// Information about the function F should be printed by printing the stack (use PRINT_TRACE)
void FREE(void* p,char* file,int line)
{
    free(p);
    fprintf(stdout, "File %s, line %d, function %s deallocated the memory segment at address %p.\n", 
        file, line, PRINT_TRACE(), p);
}

#define realloc(a,b) REALLOC(a,b,__FILE__,__LINE__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__)
#define free(a) FREE(a,__FILE__,__LINE__)

/**
 * function insertNode inserts a node at the end(tail) of the linked list
 * linked list is represented by its head and tail pointers which are double pointers
 * node & node**. Function checks if head pointer is null. if empty then head and tail
 * pointers are set to point to a new node. else, new node is appended to end of list
**/
void insertNode(node** head, node** tail, node* new) {
    if(!*head) {
        *head = new;
        *tail = new;
    }

    else {
        (*tail)->next = new;
        *tail = new;
    }
}

/**
 * function printNodes takes pointer to the head of linked list as argument.
 * checks if head node has valid string assigned else error msg
 * if valid string, then function stdout which line then recursively calls itself
 * with next node of current head
**/
void printNodes(node* head) {
    if(!head->string) {
        fprintf(stderr, "String not assigned to node %d\n", head->lineNum);
        return;
    }
    fprintf(stdout, "Line %d: %s\n", head->lineNum, head->string);

    if(head->next) {
        printNodes(head->next);
    }
}

/**
 * function freeList free memory occupied by linked list.
 * We use push_trace and pop_tracer from stack trace.
 * Function recursively calls itself with the next node of current head. 
**/
void freeList(node* head) {
    PUSH_TRACE("freeList");
    if(head->next) {
        freeList(head->next);
    }
    free(head);
    POP_TRACE();
}

/**
 * main function reads cmd from standard input and stores into linked list
 * and 2d array. Then prints cmd in list to stdout. main uses dup2() to redirect
 * standard output(1) to fd_out. 2d array is defined by character pointers
 * and uses memset(). input is then stored in array input and appended to commands
 * array. after input is read and stored, function freeList() frees memory
 * occupied by linked list and then pop the stack twice returning 0 for success
**/
int main( int argc, char *argv[] ) {
    int fd_out = open("memtrace.out", O_RDWR | O_CREAT | O_APPEND, 0777);
    if(dup2(fd_out, 1) != 1) {
        fprintf(stdout, "dup2 didn't work!");
    }

    PUSH_TRACE("main");

    // Create 2D array to track all commands
    int count = 0;
    int size= 10;

    char** commands;
    commands = (char**) malloc(size * sizeof(char*));
    memset(commands, 0, size * sizeof(char*));
    
    // List for linked list storage model
    node* listHead = NULL, *listTail = NULL;


    // Get commands from stdin, store in list and array
    while(1) {
        // Expand commands array as needed
        if(count >= size) {
            size += 10;
            commands = (char**) realloc(commands, size*sizeof(char *));
        }

        // This memory should be freed when commands is freed (list only frees nodes)
        char* input = (char*) malloc(MAX_LENGTH * sizeof(char));
        memset(input, 0, MAX_LENGTH * sizeof(char));

        if( fgets(input, MAX_LENGTH, stdin) == NULL) {
            free(input);
            break;
        }
        // For null characters and blanks
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        node* newNode = (node*) malloc(sizeof(node));
        memset(newNode, 0, sizeof(node));

        newNode->lineNum = count + 1;
        newNode->string = input;
        insertNode(&listHead, &listTail, newNode);

        commands[count] = input;
        count++;
        
    }

    printNodes(listHead);

    freeList(listHead);

    // Free all commands in the array
    for(int i = 0; i < count; i++) {
        free(commands[i]);
    }
    free(commands);
    POP_TRACE();
    // Second pop is needed to clean up global entry in stack. Returns 0 for success
    POP_TRACE();
    return 0;
}
