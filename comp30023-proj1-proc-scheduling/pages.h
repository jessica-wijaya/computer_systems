/*
    C header related to pages and frames
*/
#ifndef _PAGES_H
#define _PAGES_H

#define PAGE_SIZE 4
#define FRAME_SIZE 4
#define PAGE_COUNT 512
#define PROCESS_ID_SIZE 9

// Forward declarations
typedef struct memory memory_t;
typedef struct process process_t;


// 
typedef struct frame {
    int frameNumber;
    int memoryStart;
    int isAllocated;
    int size;
    char processId[PROCESS_ID_SIZE];
} frame_t;


//
typedef struct page {
    process_t *parentProcess; // the process that owns this page node
    frame_t *frame;
    int referenced;
    char pageId[PROCESS_ID_SIZE + PAGE_COUNT]; // Id format: P1_23 represents 24th page of process P1 (starts at 0)
} page_t;


int calculatePageNeeded(process_t *process);

void initializeProcessPages(process_t *process);

frame_t *createFrameBlock(int frameNumber, int isAllocated, int memoryStart, char *processId);

list_t *createPagedSimulation();

int allocatePageFirstFit(list_t *simulation, page_t *page);

int calculateAvailableFrames(list_t *frameSimulation);

void allocateProcess(list_t *frameSimulation, process_t *process, list_t *queueLRU, int currentTime, char *mem);

void deallocatePage(page_t *page);

void deallocateProcessPages(process_t *process, int currentTime);

list_t *initializeLRU();

void referenceProcess(list_t *queueLRU, process_t *process, char *mem);

void evictProcess(list_t *queueLRU, int currentTime);

void referencePage(list_t *queueLRU, page_t *page);

void evictPage(list_t *queueLRU, int currentTime, int *evictedFrames, int j);

int calculateFrameUsage(list_t *memorySpace);

void printFrameSpace(list_t *simulation);

void printProcessFrameList(process_t *process);

void printPageQueue(list_t *queue);

void printProcessQueue(list_t *queue);

void freeProcessPages(process_t *process);

void freeLRUQueue(list_t *list);


#endif