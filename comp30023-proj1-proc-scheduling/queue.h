/*
    C file header related to queue 
*/


#ifndef QUEUE_H
#define QUEUE_H

typedef struct process process_t;

typedef struct node {
    process_t *proc;
    struct node* next;
} node_t;

typedef struct {
    node_t* head;
    node_t* tail;
} queue_t;

// Create queue
void createQueue(queue_t* q);

// Enqueue process to queue
void enqueue(queue_t* q, process_t *proc);

// Dequeue process from queue
process_t *dequeue(queue_t* q);

// Check if queue is empty
int isEmpty(queue_t* q);

#endif

