#ifndef _TASK3_H_
#define _TASK3_H_


#define AVAILABLE_KB 2048


typedef struct process process_t;


double countUsage(int kbRequired);

void scheduleProgram3(process_t** processes, int numProcesses, int quantum, char *mem);


#endif