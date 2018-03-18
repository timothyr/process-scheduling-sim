#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "list.h"
#include "scheduler.h"

struct PCB *initProcess;
int pidCounter = 0;
SEMAPHORE semaphores[5];



// Helper
void enqueue_process(PCB * process) {
    ListAppend(readyQueue, process);
}

void dequeue_current_process() {

}

void init_sim() {
    // Make all semaphores are uninitialized
    SEMAPHORE nullSemaphore;
    nullSemaphore.init = 0;
    for(int i = 0; i < 5; i++) {
        semaphores[i] = nullSemaphore;
    }

    // Create lists
    readyQueue = ListCreate();
    blockedProcesses = ListCreate();

    // Add init process
    initProcess = (struct PCB *) malloc(sizeof(struct PCB));

    initProcess->pid = INIT_PCB_PID;
    initProcess->priority = PRIORITY_LOW;

    runningProcess = initProcess;
    
}

int create_process(Priority priority) {
    if(priority == FAILED) {
        return FAILED;
    }
    
    // Create a new process
    pidCounter++;

    struct PCB *newProcess = (struct PCB *) malloc(sizeof(struct PCB));

    newProcess->pid = pidCounter;
    newProcess->priority = priority;

    // Put new process on ready queue
    enqueue_process(newProcess);
}


int new_semaphore(int semaphoreId, int initValue) {
    // Check if number was out of bounds
    if(semaphoreId == FAILED) {
        printf("Invalid semaphore id.\n");
        return FAILED;
    } 

    SEMAPHORE semaphore = semaphores[semaphoreId];

    // Check if semaphore has already been added
    if(semaphore.init) {
        printf("Semaphore %d already exists.\n", semaphoreId);
        return FAILED;
    }

    // Create the new semaphore
    SEMAPHORE newSemaphore;
    newSemaphore.init = 1;
    newSemaphore.value = initValue;
    newSemaphore.blockedProcesses = ListCreate();

    // Add to the array 
    semaphores[semaphoreId] = newSemaphore;

    return semaphoreId;
}


SEMAPHORE_OUTPUT semaphore_p(int semaphoreId) {
    SEMAPHORE_OUTPUT output;
    output.pid_affected = FAILED;
    output.status = FAILED;
    output.result = FAILED;

    // Check if number was out of bounds
    if(semaphoreId == FAILED) {
        printf("Invalid semaphore id.\n");
        return output;
    }

    SEMAPHORE semaphore = semaphores[semaphoreId];

    // Check if semaphore has already been added
    if(semaphore.init == 0) {
        printf("Semaphore %d does not exist.\n", semaphoreId);
        return output;
    }

    // Decrease semaphore value by 1
    semaphore.value--;

    if(semaphore.value < 0) {
        //TO:DO check that runningProcess is not init


        // Add current process to block list
        ListAppend(semaphore.blockedProcesses, runningProcess);
        // Block the current process
        //TO:DO let next process take its turn

        //output.pid_affected = runningProcess.pid;
        output.status = BLOCKED;
        output.result = SUCCESS;
        return output;
    }
    else {
        output.status = NOT_BLOCKED;
        output.result = SUCCESS;
        return output;
    }
}

SEMAPHORE_OUTPUT semaphore_v(int semaphoreId) {
    SEMAPHORE_OUTPUT output;
    output.pid_affected = FAILED;
    output.status = FAILED;
    output.result = FAILED;

    // Check if number was out of bounds
    if(semaphoreId == FAILED) {
        printf("Invalid semaphore id.\n");
        return output;
    }

    SEMAPHORE semaphore = semaphores[semaphoreId];

    // Check if semaphore has already been added
    if(semaphore.init == 0) {
        printf("Semaphore %d does not exist.\n", semaphoreId);
        return output;
    }

    // Increase semaphore value by 1
    semaphore.value++;

    // Unblock and ready a process
    if(semaphore.value <= 0) {
        // Check if there are any blocked processes
        if(ListCount(semaphore.blockedProcesses) > 0) {
            PCB * unblockedProcess = ListFirst(semaphore.blockedProcesses);
            // TO:DO enqueue unblockedProcess
            output.pid_affected = unblockedProcess->pid;
            output.status = READIED;
            output.result = SUCCESS;
            return output;
        } 
    }
    
    output.status = NOT_READIED;
    output.result = SUCCESS;
    return output;
}

void print_process_info(PCB * process) {
    printf("%d ", process->pid);
        Priority priority = process->priority;
        switch(priority) {
            case PRIORITY_HIGH:
                printf("(high)");
                break;
            case PRIORITY_NORM:
                printf("(norm)");
                break;
            case PRIORITY_LOW:
                printf("(low)");
                break;
            default:
                printf("(UNKNOWN_PRIORITY)");
                break;
        }
}

void totalinfo() {
    // Running process
    printf("Currently running process pid = ");
    print_process_info(runningProcess);
    printf("\n");
    // Ready queue
    if(ListCount(readyQueue) > 0) {
        printf("Ready queue pid's = ");
        PCB * process = ListFirst(readyQueue); 
        while(process != NULL) {
            print_process_info(process);
            process = ListNext(readyQueue);
            if(process != NULL) {
                printf(", ");
            }
        }
        printf(".\n");
        
    } else {
        printf("Ready queue is empty.\n");
    }
    // Blocked list
    if(ListCount(blockedProcesses) > 0) {
        printf("Blocked processes pid's = ");
        PCB * process = ListFirst(blockedProcesses); 
        while(process != NULL) {
            print_process_info(process);
            process = ListNext(blockedProcesses);
            if(process != NULL) {
                printf(", ");
            }
        }
        printf(".\n");
        
    } else {
        printf("Blocked process list is empty.\n");
    }
}

