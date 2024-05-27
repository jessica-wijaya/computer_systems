#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "task4.h"
#define CHAR_LENGTH_OF_FIRST_FIT 10 // the length of the word "first-fit" + null byte
#define MAX_PROCESSES 100
#define PROCESS_ID_SIZE 9 // process IDs of length 8


int main(int argc, char *argv[]) {
    FILE *inFile = NULL;
    char *input_file = NULL;
    int count = 0;
    int quantum = 0;
    char *mem = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            input_file = argv[i + 1];
        } if (strcmp(argv[i], "-q") == 0) {
            quantum = atoi(argv[i + 1]);
        } if (strcmp(argv[i], "-m") == 0) {
            mem = argv[i + 1];
        }
        
    }

    if (!input_file) {
        printf("Usage: %s -f <input_file> -m <mode> -q <quantum>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open input file
    inFile = fopen(input_file, "r");
    if (!inFile) {
        printf("Error opening file: %s\n", input_file);
        return EXIT_FAILURE;
    }

    int estimatedProcess = MAX_PROCESSES; 

    process_t** processes = (process_t**)malloc(estimatedProcess * sizeof(process_t*));
    if (processes == NULL) {
        printf("Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    // Read file and store the details of each line in the respective arrays
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), inFile) && count < MAX_PROCESSES) {
        process_t* process = (process_t*)malloc(sizeof(process_t));
        processes[count] = process;

        if (sscanf(buffer, "%d %8s %d %d", &(processes[count]->arrivalTime), processes[count]->processId, &(processes[count]->burstTime), &(processes[count]->kbRequired)) == 4) {
            processes[count]->remainingTime = processes[count]->burstTime; // Initialize remainingTime
            processes[count]->memoryBlock = NULL;
            processes[count]->referenced = 0;
            processes[count]->isAllocated = 0;
            count++;
        } else {
            free(process);
            fprintf(stderr, "Error at line: %s\n", buffer);
        }

    }

    if (strcmp(mem, "infinite") == 0) {

        scheduleProgram(processes, count, quantum, mem);
    }
    if (strcmp(mem, "first-fit") == 0) {

        scheduleProgram2(processes, count, quantum, mem);
    }
    if (strcmp(mem, "paged") == 0) {

        scheduleProgram3(processes, count, quantum, mem);
    }
    if (strcmp(mem, "virtual") == 0) {

        scheduleProgram4(processes, count, quantum, mem);
    }
    
    // Free all allocated process structures
    for (int i = 0; i < count; i++) {
        free(processes[i]);  // Free each process
    }
    free(processes);
    fclose(inFile);


    return EXIT_SUCCESS;
}