#ifndef _TASK4_H_
#define _TASK4_H_


#define AVAILABLE_KB 2048


typedef struct process process_t;


double countUsage(int kbRequired);

void scheduleProgram4(process_t** processes, int numProcesses, int quantum, char *mem);


#endif