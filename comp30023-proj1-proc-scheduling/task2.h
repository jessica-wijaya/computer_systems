#ifndef _TASK2_H_
#define _TASK2_H_


#define AVAILABLE_KB 2048

#include "queue.h"
#include "process.h"
#include "list.h"
#include "memory.h"
#include "task1.h"

typedef struct process process_t;


double countUsage(int kbRequired);

void scheduleProgram2(process_t** processes, int numProcesses, int quantum, char *mem);


#endif