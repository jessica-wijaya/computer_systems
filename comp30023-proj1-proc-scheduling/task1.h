#ifndef _TASK1_H_
#define _TASK1_H_

#include "process.h"
#include "queue.h"
#include "list.h"
#include "memory.h"

int calculateTurnaround(int completionTime, int arrivalTime);

double calculateOverhead(double duration, int serviceTime);

void scheduleProgram(process_t **processes, int numProcesses, int quantum, char *mem);

#endif