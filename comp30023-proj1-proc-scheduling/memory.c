/*
    C file related to memory managements 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "process.h"
#include "list.h"
#include "memory.h"


// Initializes a memory block of one of a process, hole or page
memory_t *createMemoryBlock(int isAllocated, int memoryStart, int size, char *processId) {
    memory_t *newMemoryBlock = (memory_t*)malloc(sizeof(memory_t));

    if (newMemoryBlock == NULL) {
        fprintf(stderr, "Error initializing a memory block. Exiting.");
        exit(EXIT_FAILURE);
    }

    newMemoryBlock->isAllocated = isAllocated;
    newMemoryBlock->memoryStart = memoryStart;
    newMemoryBlock->size = size;
    strcpy(newMemoryBlock->processId, processId);

    return newMemoryBlock;
}

// Initialize the simulated computer memory with 2048 KB.
list_t *createSimulation() {
    list_t *simulationMemory = createList();
    memory_t *initialHole = createMemoryBlock(0, 0, AVAILABLE_KB, "\0"); // 2048 KB block of hole

    insertHead(simulationMemory, initialHole);

    return simulationMemory;
}


/* Allocates memory using first-fit strategy
    Returns 0 if no space is available.
    Returns 1 if allocation is successful.
*/
int allocateFirstFit(list_t *list, process_t *process) {
    listNode_t *curr = list->head;

    if (curr == NULL) {
        fprintf(stderr, "Error trying to allocate memory: Blocks not present in simulation.");
        exit(EXIT_FAILURE);
    }

    // Begin searching for open memory space
    for (int i = 0; i < list->size; i++) {

        memory_t *currentBlock = (memory_t *) curr->data;

        // allocate if a hole of equal size is found
        if ((currentBlock->isAllocated == 0) && (currentBlock->size == process->kbRequired)) {
            currentBlock->isAllocated = 1;
            strcpy(currentBlock->processId, process->processId);

            process->memoryBlock = currentBlock;

            return 1;
        }
        // split into a process and a hole if a larger hole is found
        else if ((currentBlock->isAllocated == 0) && (currentBlock->size >= process->kbRequired)) {

            memory_t *newProcessBlock = createMemoryBlock(1, currentBlock->memoryStart, process->kbRequired, process->processId);
            
            insertPrevious(list, curr, newProcessBlock);

            process->memoryBlock = newProcessBlock;

            // Cuts down the size of the hole (currentBlock)
            currentBlock->memoryStart += process->kbRequired;
            currentBlock->size -= process->kbRequired;

            return 1;
        }

        curr = curr->next;
    }
    return 0;
}


/* Combines all blocks of hole adjacent to a hole into one 
    (i.e. if there is a hole before or after (or both) next to the current hole, they will combine into one) */
void mergeAdjacentHoles(list_t* list, listNode_t *currNode) {
    memory_t *currBlock, *nextBlock, *prevBlock;

    currBlock = (memory_t *) currNode->data;

    if (currBlock->isAllocated == 1) { // current block is not a hole. no need to aggregate.
        return;
    }
    
    // If there is a block after current one
    if (currNode->next != NULL) {
        nextBlock = (memory_t *) currNode->next->data;

        // Checks if next block is a hole and merges if so.
        if (nextBlock->isAllocated == 0) {
            currBlock->size += nextBlock->size;

            deleteNode(list, currNode->next, true); // make sure this frees properly
        }
    }

    // If there is a block before current one
    if ((currNode->prev != NULL)) {
        prevBlock = (memory_t *) currNode->prev->data;

        // Checks if previous block is a hole and merges if so.
        if (prevBlock->isAllocated == 0) {
            prevBlock->size += currBlock->size; 

            deleteNode(list, currNode, true); 
        }
        
    }
}


// Linearly searches through all process for a specified processId
void deallocateContiguousProcess(list_t *list, char *processId) {
    listNode_t *currNode = list->head;
    memory_t *currBlock;

    for (int i = 0; i < list->size; i++) {
        currBlock = (memory_t *) currNode->data;

        // If we found the process block we want to deallocate.
        if (strcmp(currBlock->processId, processId) == 0) {
            currBlock->isAllocated = 0;
            strcpy(currBlock->processId, "\0");

            mergeAdjacentHoles(list, currNode);

            return;
        }
        currNode = currNode->next;
    }
}


// Linearly searches through simulation and deallocates specified memory block
void deallocateMemoryBlock(list_t *list, memory_t *memory) {
    listNode_t *currNode = list->head;

    for (int i = 0; i < list->size; i++) {
        memory_t *currBlock = (memory_t *) currNode->data;

        if (currBlock == memory) { // If we found the memory we want to deallocate.
            memory->isAllocated = 0;
            strcpy(memory->processId, "\0");

            mergeAdjacentHoles(list, currNode);

            return;
        }
        currNode = currNode->next;
    }
}


// Calculates percentage of memory used
int calculateMemoryUsage(list_t *memorySpace) {
    listNode_t *curr = memorySpace->head;
    double totalKBUsed = 0;
    int percentage;

    for (int i = 0; i < memorySpace->size; i++) {
        memory_t *currBlock = (memory_t *) curr->data;

        if (currBlock->isAllocated == 1) {
            totalKBUsed += currBlock->size;
        }

        curr = curr->next;
    }

    // Rounds up the percentage to an integer
    percentage = (int) ceil((totalKBUsed / (double) AVAILABLE_KB) * 100);
    
    return percentage;
}  


// Prints out the entire memory simulation
void printMemorySpace(list_t *simulation) {
    listNode_t *curr = simulation->head;

    fprintf(stderr, "START OF MEMORY\n-------------------\n");
    for (int i = 0; i < simulation->size; i++) {
        memory_t *block = (memory_t *) curr->data;

        fprintf(stderr, " %d | PID: %s | Has Process: %d | Start: %d | Size: %d | \n", i, block->processId, block->isAllocated, block->memoryStart, block->size);

        curr = curr->next;
    }
    fprintf(stderr, "\n-------------------\nEND OF MEMORY\n");
}

void freeMemorySpace(list_t *memorySpace) {
    listNode_t *current = memorySpace->head;
    while (current != NULL) {
        listNode_t *next = current->next;
        memory_t *memoryBlock = (memory_t *)current->data;
        free(memoryBlock);  
        free(current);      
        current = next;
    }
    free(memorySpace);    
}