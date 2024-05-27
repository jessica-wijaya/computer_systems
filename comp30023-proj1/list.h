#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>



typedef struct listNode {
    void* data; 
    struct listNode* next;
    struct listNode* prev;
} listNode_t;


typedef struct list {
    listNode_t* head;
    listNode_t* tail;
    int size;
} list_t;


list_t *createList();

void insertHead(list_t* list, void* data);

void insertTail(list_t* list, void* data);

void insertPrevious(list_t* list, listNode_t* node, void* data);

void freeNode(listNode_t *curr);

void deleteNode(list_t *list, listNode_t *node, bool freeData);

void freeList(list_t* list);

#endif

