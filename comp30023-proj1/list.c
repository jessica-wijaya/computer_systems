#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"



/*
    Memory will be represented by a linked list, each node holding 
    a "block" data (block.c or block.h for reference)

    Example
        1.) Simulation beginss

        { Node 1 }
        Here node 1 is a 2048 KB hole.

        2.) A new process is added of size 48 KB to the simulation

        { Node 1 } -> { Node 2 }
        Node 1: A process of size 48 KB
        Node 2: A hole of size 2000 KB (2048 - 48)

        And so on...
*/

list_t *createList() {
    list_t *newList = (list_t*)malloc(sizeof(list_t));

    if (newList == NULL) {
        fprintf(stderr, "An error occured when creating a list. Exiting.");
        exit(EXIT_FAILURE);
    }

    newList->head = newList->tail = NULL;
    newList->size = 0;
    
    return newList;
};

void insertHead(list_t* list, void* data) {
    listNode_t *newListNode = (listNode_t*)malloc(sizeof(listNode_t));

    if (newListNode == NULL) {
        fprintf(stderr, "An error occured when creating a node for list. Exiting.");
        exit(EXIT_FAILURE);
    }

    newListNode->data = data;

    if (list->size == 0) { // if list empty
        list->head = list->tail = newListNode;
        newListNode->next = NULL;
    }
    else {
        list->head->prev = newListNode;
        newListNode->next = list->head;
        list->head = newListNode;
    }
    newListNode->prev = NULL;
    
    (list->size)++;
};

void insertTail(list_t* list, void* data) {
    listNode_t *newListNode = (listNode_t*)malloc(sizeof(listNode_t));

    if (newListNode == NULL) {
        fprintf(stderr, "An error occured when creating a node for list. Exiting.");
        exit(EXIT_FAILURE);
    }

    newListNode->data = data;

    if (list->size == 0) { // if list empty
        list->head = list->tail = newListNode;
        newListNode->prev = NULL;
    }
    else {
        list->tail->next = newListNode;
        newListNode->prev = list->tail;
        list->tail = newListNode;
    }
    newListNode->next = NULL;

    (list->size)++;
};

// Insert a node right before a specified node
void insertPrevious(list_t* list, listNode_t* node, void* data) {

    // fprintf(stderr, "test %d\n", ((memory_t *) data)->size);

    if (node->prev == NULL) {
        insertHead(list, data);
        return;
    }

    listNode_t *newListNode = (listNode_t*)malloc(sizeof(listNode_t));

    if (newListNode == NULL) {
        fprintf(stderr, "An error occured when creating a node for list. Exiting.");
        exit(EXIT_FAILURE);
    }

    newListNode->data = data;
    newListNode->next = node;
    newListNode->prev = node->prev;
    node->prev->next = newListNode;
    node->prev = newListNode;

    (list->size)++;
}

void freeNode(listNode_t *node) {
    free(node->data);
    
    free(node);
}

// Deletes a node from a list
void deleteNode(list_t *list, listNode_t *node, bool freeData) {
    if (list->size != 0 && node != NULL) {
        if (node->prev == NULL) {
            list->head = node->next;
        }
        else {
            node->prev->next = node->next;
        }

        if (node->next == NULL) {
            list->tail = node->prev;
        }
        else {
            node->next->prev = node->prev;
        }
        (list->size)--;
        if (freeData) {
            freeNode(node);
        } else {
            free(node);
        }
    }
    else {
        fprintf(stderr, "Attempted to delete node from empty list or delete an invalid node.");
    }
}

// Frees list as well as its contents
void freeList(list_t *list) {
    listNode_t *curr = list->head;
    listNode_t *temp;

    for (int i = 0; i < list->size; i++) {
        free(curr->data);
        temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(list);
}