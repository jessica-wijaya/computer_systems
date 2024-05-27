/*
    QUEUE MANAGEMENT
*/

#include <stdlib.h>
#include <stdio.h>

#include "process.h"
#include "queue.h"

// Create Queue
void createQueue(queue_t* q) {
    q->head = NULL;
    q->tail = NULL;
}

// Enqueue process to queue
void enqueue(queue_t* q, process_t *proc) {
    node_t* newNode = (node_t*)malloc(sizeof(node_t));

    if (newNode == NULL) {


        fprintf(stderr, "Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }

    newNode->proc = proc;
    newNode->next = NULL;

    if (q->tail == NULL) {
        q->head = newNode;
        q->tail = newNode;
    } else {
        q->tail->next = newNode;
        q->tail = newNode;
    }
}

// Dequeue process to queue
process_t *dequeue(queue_t* q) {
    if (q->head == NULL) {
        fprintf(stderr, "Queue is empty, cannot dequeue\n");
        return NULL;
    }

    node_t* temp = q->head;
    process_t* proc = temp->proc;
    q->head = q->head->next;

    if (q->head == NULL) {
        q->tail = NULL;
    }

    free(temp); 

    return proc;
}

// Check if queue is empty
int isEmpty(queue_t* q) {
    return q->head == NULL;
}



