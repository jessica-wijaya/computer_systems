#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>


#include "task1.h"

int calculateTurnaround(int completionTime, int arrivalTime) {
    return completionTime - arrivalTime;;
}

double calculateOverhead(double duration, int serviceTime) {
    return duration / serviceTime;
}

void scheduleProgram(process_t** processes, int numProcesses, int quantum, char *mem) {
    
    queue_t readyQueue;
    createQueue(&readyQueue);
    
    int currentTime = 0;
    int queueSize = 0;
    int finished = 0;

    process_t *prevProcess = NULL;
    process_t *currentProcess = NULL;

    double turnaround = 0;
    double overhead = 0;
    int contextSwitch = 0; 
    int makeSpan = 0;
    double totalTurnaround = 0;
    double maxOverhead = 0;
    double totalOverhead = 0;

    // Enqueue processes that have arrived by currentTime=0
    int i;
    for (i = 0; i < numProcesses; i++) {
        if (processes[i]->arrivalTime == 0) {
            enqueue(&readyQueue, processes[i]);
            queueSize += 1;
        } else {
            break;
        }
    }
    
    
    while (finished < numProcesses) {

        if (!isEmpty(&readyQueue)) {
            currentProcess = dequeue(&readyQueue);
            queueSize -= 1;
            if ((prevProcess != NULL && (strcmp(prevProcess->processId, currentProcess->processId) != 0 )) || currentProcess->remainingTime == currentProcess->burstTime) {
                contextSwitch++;
                printf("%d,RUNNING,process-name=%s,remaining-time=%d", currentTime, currentProcess->processId,currentProcess->remainingTime);
                
                printf("\n");
            } 
        } 

        if (0 < currentProcess->remainingTime && currentProcess->remainingTime < quantum ) {
                currentTime += quantum;
                currentProcess->remainingTime = 0;
        } else {
                currentProcess->remainingTime -= quantum;
                currentTime += quantum;
        }
        
        // Queues all unqueued process that arrived up to currentTime
        for (int j = i; j < numProcesses; j++) {
            if (processes[j]->arrivalTime <= currentTime) {
                enqueue(&readyQueue, processes[j]);
                queueSize += 1;
                i++;
            } else {
                break;
            }
        }
        
        if (currentProcess->remainingTime > 0) {
            // Reenqueue if the process is not finished
            enqueue(&readyQueue, currentProcess);
            queueSize += 1;

            prevProcess = currentProcess;

        } else {
            if (currentProcess->remainingTime == 0) {
                currentProcess->kbRequired = 0;
                prevProcess = currentProcess;

               
                printf("%d,FINISHED,process-name=%s,proc-remaining=%d\n", currentTime, currentProcess->processId, queueSize);
                // calculate turnaround time and time overhead
                turnaround = calculateTurnaround(currentTime, currentProcess->arrivalTime);
                totalTurnaround += turnaround;

                overhead = calculateOverhead(turnaround, currentProcess->burstTime);
                totalOverhead += overhead;

                if (maxOverhead < overhead) {
                    maxOverhead = overhead;
                }

                finished++;
            }
        }
    }

    // get average turnaround time and time overhead
    totalTurnaround = totalTurnaround / numProcesses;
    
    totalOverhead = totalOverhead / numProcesses;
    double averageOverhead = round(totalOverhead * 100) / 100.0;

    makeSpan = currentTime;
    
    printf("Turnaround time %d\nTime overhead %.2f %.2f\nMakespan %d", (int)round(totalTurnaround), maxOverhead, averageOverhead, makeSpan);

}   