/* 
    READ ME:

    For now, we will be using a static array approach for task 2. 
    The linked list implementation will be on hold.
*/





/*
    C file header related to memory blocks (a chunk of memory 
    representing either a process or hole)
*/

#ifndef BLOCK_H
#define BLOCK_H

#include <stdlib.h>

#include "list.h"
#include "task2.h"


#define AVAILABLE_KB 2048
#define PROCESS_ID_SIZE 9

// Forward declerations
typedef struct process process_t;
typedef struct list list_t;
typedef struct listNode listNode_t;


/*
    list_t -> listNode_t -> memory_t
*/
typedef struct memory {
    int isAllocated;
    int memoryStart; // The memory "address" at which the block starts.
    int size;
    char processId[PROCESS_ID_SIZE]; // \0 if it is a hole
} memory_t;

memory_t *createMemoryBlock(int isAllocated, int memoryStart, int size, char *processId);

list_t *createSimulation();

void mergeAdjacentHoles(list_t* list, listNode_t *currNode);

int allocateFirstFit(list_t *list, process_t *process);

void deallocateContiguousProcess(list_t *list, char *processId);

void deallocateMemoryBlock(list_t *list, memory_t *memory);

int calculateMemoryUsage(list_t *memorySpace);

void printMemorySpace(list_t *simulation);

void freeMemorySpace(list_t *memorySpace);

#endif