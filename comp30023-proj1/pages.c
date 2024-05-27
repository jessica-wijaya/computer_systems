/*
    C file related to pages and frames
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "process.h"
#include "queue.h"
#include "list.h"
#include "memory.h"
#include "pages.h"

/*
    FRAME MANAGEMENT
*/

// Calculate how much page is needed for a process
int calculatePageNeeded(process_t *process) {
    double process_size = (double) process->kbRequired;

    return (int) ceil((process_size) / (PAGE_SIZE));
}


// Initializes the pages of a process. Perform this function for every process - allocated or not.
void initializeProcessPages(process_t *process) {
    
    int pageNeeded = calculatePageNeeded(process);
    page_t **pageList = (page_t**)malloc(sizeof(page_t*) * pageNeeded);

    for (int i = 0; i < pageNeeded; i++) {
        pageList[i] = (page_t*)malloc(sizeof(page_t));
        pageList[i]->parentProcess = process;
        pageList[i]->frame = NULL;
        pageList[i]->referenced = 0;
    }

    process->pages = pageList; 
    process->pageAllocated = 0;
}


// 
frame_t *createFrameBlock(int frameNumber, int isAllocated, int memoryStart, char *processId) {
    frame_t *newFrameBlock = (frame_t*)malloc(sizeof(frame_t));

    if (newFrameBlock == NULL) {
        fprintf(stderr, "Error initializing a frame block. Exiting.");
        exit(EXIT_FAILURE);
    }

    newFrameBlock->frameNumber = frameNumber;
    newFrameBlock->isAllocated = isAllocated;
    newFrameBlock->memoryStart = memoryStart;
    newFrameBlock->size = FRAME_SIZE;
    strcpy(newFrameBlock->processId, processId);

    return newFrameBlock;
}


// Initializes a list of pages - each 4 KB - totalling 2048 KB
list_t *createPagedSimulation() {
    list_t *simulationMemory = createList();
    frame_t *frame;

    for (int i = 0; i < PAGE_COUNT; i++) {
        frame = createFrameBlock(i, 0, i * FRAME_SIZE, "\0");
        insertTail(simulationMemory, frame);
    }

    return simulationMemory;
}


// Allocates a page to the lowest numbered frame
int allocatePageFirstFit(list_t *simulation, page_t *page) {
    listNode_t *curr = simulation->head;
    frame_t *currentFrame;
    char *parentProcessId = page->parentProcess->processId;

    if (curr == NULL) {
        fprintf(stderr, "Error trying to allocate memory: Blocks not present in simulation.");
        exit(EXIT_FAILURE);
    }

    // Begin searching for open frame
    for (int i = 0; i < PAGE_COUNT; i++) {

        currentFrame = (frame_t *) curr->data;

        // Allocate if an open frame is found
        if (currentFrame->isAllocated == 0) {
            
            currentFrame->isAllocated = 1;
            strcpy(currentFrame->processId, parentProcessId);
            page->frame = currentFrame;
            
            (page->parentProcess->pageAllocated)++;

            return 1;
        }

        curr = curr->next;
    }
    return 0;
}


// Checks how much frames are available to allocate
int calculateAvailableFrames(list_t *frameSimulation) {
    listNode_t *currNode = frameSimulation->head;
    frame_t *currFrame;
    int totalOpenFrame = 0;

    for (int i = 0; i < PAGE_COUNT; i++) {
        currFrame = (frame_t *)currNode->data;

        if (!(currFrame->isAllocated)) {
            totalOpenFrame++;
        }

        currNode = currNode->next;
    }

    return totalOpenFrame;
}


// Allocates all pages of a process. Evicts another process or pages if no space available.
void allocateProcess(list_t *frameSimulation, process_t *process, list_t *queueLRU, int currentTime, char *mem) {
    page_t *page;
    int availableFrames = calculateAvailableFrames(frameSimulation);
    int pageCount = calculatePageNeeded(process);

    // Eviction procedure for "paged" mode
    if (strcmp(mem, "paged") == 0) {
        while (availableFrames < pageCount) {
            evictProcess(queueLRU, currentTime);

            availableFrames = calculateAvailableFrames(frameSimulation);
        }
    }
    // Eviction procedure for "virtual" mode
    if (strcmp(mem, "virtual") == 0) {
        
        int evictedFrameNumber[PAGE_COUNT];
        int j = 0, needsEviction = 0;

        if ((availableFrames < pageCount) && (availableFrames < 4)) {
            printf("%d,EVICTED,evicted-frames=[", currentTime);
            needsEviction = 1;
        }

        while ((availableFrames < pageCount) && (availableFrames < 4)) {
            evictPage(queueLRU, currentTime, evictedFrameNumber, j);
            
            j++;

            availableFrames = calculateAvailableFrames(frameSimulation);
        }

        if (needsEviction) {
            int k;
            for (k = 0; k < (j-1); k++) {
                printf("%d,", evictedFrameNumber[k]);
            }
            printf("%d]\n", evictedFrameNumber[k]);
        }
    }
    for (int i = 0; (i < availableFrames) && (i < pageCount); i++) { 
        page = (process->pages)[i]; 
           
        allocatePageFirstFit(frameSimulation, page);
    }
}


// Deallocates a page
void deallocatePage(page_t *page) {
    frame_t *pageFrame = page->frame; 

    pageFrame->isAllocated = 0;
    strcpy(pageFrame->processId, "\0");
    page->frame = NULL;
    
    (page->parentProcess->pageAllocated)--;
}


// Deallocates a process and its pages
void deallocateProcessPages(process_t *process, int currentTime) {
    page_t **pageList = process->pages;
    int pageCount = calculatePageNeeded(process);

    printf("%d,EVICTED,evicted-frames=", currentTime);
    printProcessFrameList(process);
    printf("\n");

    for (int i = 0; i < pageCount; i++) {
        if (((pageList[i])->frame) != NULL) {
            deallocatePage(pageList[i]);
        }
    }

    process->isAllocated = 0;
}



/* Initializes a Least Recently Used (LRU) queue
    (The head of the queue indicates MOST recently used,
    and the tail indicates LEAST recently used)
*/
list_t *initializeLRU() {
    list_t *newQueueLRU = createList();

    return newQueueLRU;
}


// When a process is referenced, send the process to head of queue (most recently used).
void referenceProcess(list_t *queueLRU, process_t *process, char *mem) {
    if (strcmp(mem, "paged") == 0) {
        /*
        Linearly searches the queue for the process and DELETES it.
        */
        if (process->referenced == 0) { // if process not yet referenced before
            process->referenced = 1;
        }
        else { // if referenced previously, search through list and delete
            listNode_t *currListNode = queueLRU->head;

            int i;
            for (i = 0; i < queueLRU->size; i++) {

                if (((process_t *) currListNode->data) == (process)) {
                    deleteNode(queueLRU, currListNode, false);
                    break;
                }
            }
        }

        // Append the memory to head of list
        insertHead(queueLRU, process);
    }
    else if (strcmp(mem, "virtual") == 0) {

        int pageNeeded = calculatePageNeeded(process);
        
        for (int i = 0; i < pageNeeded; i++) {
            if (((process->pages)[i]->frame) != NULL) {
                referencePage(queueLRU, (process->pages)[i]);
            }
        }
       // printf("\n");
    }
    
    
}


// Evicts the least recently used process (i.e. the tail of the LRU queue)
void evictProcess(list_t *queueLRU, int currentTime) {
    process_t *lruProcess = (process_t *) queueLRU->tail->data; // the least recently used process
    
    // Deallocates all pages of the LRU process
    deallocateProcessPages(lruProcess, currentTime);

    deleteNode(queueLRU,queueLRU->tail, false);
}


// When a page is referenced, send the page to head of queue (most recently used).
void referencePage(list_t *queueLRU, page_t *page) {

    /*
        Linearly searches the queue for the page and pops it.
        And then append it to head of the list.
    */
    if (page->referenced == 0) { // if page not yet referenced before
        page->referenced = 1;
    }
    else { // if referenced previously, search through list and delete
        listNode_t *currListNode = queueLRU->head;

        int i;
        for (i = 0; i < queueLRU->size; i++) {
            if (currListNode != NULL) {
                if (page == ((page_t *)currListNode->data)) {
                
                    deleteNode(queueLRU, currListNode, false);
                    break;
                }

                currListNode = currListNode->next;
            }
            else { break; }
        }
    }
    
    // Append the memory to head of list
    insertHead(queueLRU, page);
}


// Evicts least recently used pages. evictedFrames array will be used to print out the evicted-frames statistics.
void evictPage(list_t *queueLRU, int currentTime, int *evictedFrames, int j) {

    page_t *lruPage = (page_t *) queueLRU->tail->data;
    
    evictedFrames[j] = lruPage->frame->frameNumber;

    deallocatePage(lruPage);

    deleteNode(queueLRU,queueLRU->tail, false);
}


// Calculates percentage of frames used
int calculateFrameUsage(list_t *memorySpace) {
    listNode_t *curr = memorySpace->head;
    double totalFrameUsed = 0;
    int percentage;

    for (int i = 0; i < memorySpace->size; i++) {
        frame_t *currBlock = (frame_t *) curr->data;

        if (currBlock->isAllocated == 1) {
            totalFrameUsed++;
        }

        curr = curr->next;
    }

    // Rounds up the percentage to an integer
    percentage = (int) ceil((totalFrameUsed / (double) PAGE_COUNT) * 100);
    
    return percentage;
}  


// Prints out the entire memory simulation
void printFrameSpace(list_t *simulation) {
    listNode_t *curr = simulation->head;

    fprintf(stderr, "START OF MEMORY\n-------------------\n");
    for (int i = 0; i < simulation->size; i++) {
        frame_t *block = (frame_t *) curr->data;

        fprintf(stderr, " %d | PID: %s | Has Process: %d |\n", i, block->processId, block->isAllocated);

        curr = curr->next;
    }
    fprintf(stderr, "\n-------------------\nEND OF MEMORY\n");
}


// Prints out which is frame is used by a process: e.g. [0,1,2]
void printProcessFrameList(process_t *process) {
    page_t **pageList = process->pages;
    int pageNeeded = calculatePageNeeded(process);

    /*
        TODO: THIS PART IS BROKEN.
    */
    int i;
    
    printf("[");
    for (i = 0; i < pageNeeded; i++) {
        if (((pageList[i])->frame) != NULL) {
            printf("%d", (pageList[i])->frame->frameNumber);
            i++;
            break;
        }
    }
    for (; i < pageNeeded; i++) {
        if (((pageList[i])->frame) != NULL) {
            printf(",%d", (pageList[i])->frame->frameNumber);
        }
    }
    printf("]");
}


void printProcessQueue(list_t *queue) {
    listNode_t *curr = queue->head;
    

    printf("Queue: [");
    for (int i = 0; i < queue->size; i++) {
        if ((curr->data) != NULL) {
            printf(" %s ", ((process_t *)curr->data)->processId);
        }
        
        if (curr->next != NULL) {
            curr = curr->next;
        }
    }
    printf("]\n");
}


void printPageQueue(list_t *queue) {
    listNode_t *curr = queue->head;
    int frameNumber;

    printf("Queue: [");
    for (int i = 0; i < queue->size; i++) {
        if ((curr->data) != NULL) {
            if (((page_t *)curr->data)->frame != NULL) {
                frameNumber = ((page_t *)curr->data)->frame->frameNumber;
                printf(" %d ", frameNumber);
            }
        }
        
        if (curr->next != NULL) {
            printf(" ! ");
            curr = curr->next;
        }
    }
    printf("]\n");
}


void freeProcessPages(process_t *process) {
    if (process->pages != NULL) {
        int pageNeeded = calculatePageNeeded(process);
        for (int i = 0; i < pageNeeded; i++) {
            if (process->pages[i] != NULL) {
                free(process->pages[i]);  
            }
        }
        free(process->pages); 
    }
}

void freeLRUQueue(list_t *list) {
    listNode_t *current = list->head;
    while (current != NULL) {
        listNode_t *next = current->next;
        free(current); 
        current = next;
    }
    free(list);
}


