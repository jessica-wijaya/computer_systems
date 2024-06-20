#ifndef _PROCESS_H_
#define _PROCESS_H_

#define PROCESS_ID_SIZE 9 // For process IDs of max length 8 + 1 for null terminator

// Forward declaration
typedef struct memory memory_t;
typedef struct process process_t;
typedef struct page page_t;


typedef struct process {
    char processId[PROCESS_ID_SIZE];
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int kbRequired;

    // This will be used for task 2 and onwards
    memory_t *memoryBlock; 

    // This will be used for task 3 and onwards
    int referenced;
    int isAllocated;
    page_t **pages; // pages are treated as a subset of the process

    // This will be used for task 4
    int pageAllocated;

} process_t;

#endif