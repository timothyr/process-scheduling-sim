#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "list.h"
#include "scheduler.h"

PCB * runningProcess;
LIST * readyQueue;
LIST * blockedProcesses;

struct PCB *initProcess;
int pidCounter = 0;
SEMAPHORE semaphores[5];

void print_process_info(PCB * process) {
    printf("[%d ", process->pid);
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
            case PRIORITY_INIT:
                printf("(init)");
                break;
            default:
                printf("(UNKNOWN_PRIORITY)");
                break;
        }
    printf("]");
}

int comparator(void* item, void* comparisonArg) {
    if(item == NULL) {
        return 0;
    }

    if(comparisonArg == NULL) {
        return 0;
    }

    PCB * currentItemPCB = (struct PCB *) item;
    int targetPid = *(int *) comparisonArg;

    // If pid == pid then the item is a match
    if(currentItemPCB->pid == targetPid) {
        return 1;
    }

    return 0;
}

void set_message(PCB * process, char * msg) {
    process->proc_message = msg;
}

char * get_message(PCB * process) {
    char * msg;
    msg = process->proc_message;
    // Set back to null
    process->proc_message = NULL;
    return msg;
}

PCB * searchListForPid(LIST * list, int pid, int action) {
    ListFirst(list);
    if(ListSearch(list, comparator, &pid) == NULL) {
        return NULL;
    } else {
        if(action == ACTION_DELETE) {
            return (struct PCB *) ListRemove(list);
        }
        if(action == ACTION_KEEP) {
            return (struct PCB *) ListCurr(list);
        }
    }
}

// Helper
PCB * findProcessByPid(int pid, int action) {
    // Search running process
    if(runningProcess->pid == pid) {
        if(action == ACTION_DELETE) {
            // TO:DO dequeue?
            return runningProcess;
        }
        if(action == ACTION_KEEP) {
            return runningProcess;
        }
        return NULL;
    }

    PCB * result;

    // Search Ready queue
    result = searchListForPid(readyQueue, pid, action);
    if(result != NULL) {return result;}

    // Search blocked list
    result = searchListForPid(blockedProcesses, pid, action);
    if(result != NULL) {return result;}

    // Search semaphores blocked lists
    for(int i = 0; i < 5; i++) {
        SEMAPHORE * semaphore = &semaphores[i];
        if(semaphore->init) {
            result = searchListForPid(semaphore->blockedProcesses, pid, action);
            if(result != NULL) {return result;}
        }
    }

    return NULL;
}

void enqueue_process(PCB * process) {
    ListAppend(readyQueue, process);
    if(runningProcess->pid == INIT_PCB_PID) {
        quantum();
    }
}

void remove_process_from_CPU() {
    runningProcess = initProcess;
}

void remove_process_from_CPU_add_to_ready_queue() {
    if(runningProcess->pid != INIT_PCB_PID) {
        enqueue_process(runningProcess);
    }
    remove_process_from_CPU();
}

int run_next_process_on_CPU() {
    remove_process_from_CPU_add_to_ready_queue();
    if(ListCount(readyQueue) > 0) {
        ListFirst(readyQueue);
        runningProcess = ListRemove(readyQueue);
        printf("Process %d is now running.\n", runningProcess->pid);
        // Get message if it was waiting
        if(runningProcess->waitingForMsg == MESSAGE_WAITING) {
            printf("Process ");
            print_process_info(runningProcess);
            printf(" received message: \"%s\"\n", get_message(runningProcess));
            runningProcess->waitingForMsg = MESSAGE_IDLE;
        }
    } else {
        printf("System is idle.\n");
        runningProcess = initProcess;
    }
    return runningProcess->pid;
}

void block_current_process(LIST * list) {
    printf("Process ");
    print_process_info(runningProcess);
    printf(" blocked.\n");
    // Add current process to block list
    ListAppend(list, runningProcess);
    
    // Take current process off of CPU
    // and do not put it back on the ready queue
    remove_process_from_CPU();
    // Let CPU get next process
    quantum();
}

// TO:DO init message buffer?
PCB * malloc_PCB() {
    return (struct PCB *) malloc(sizeof(struct PCB));
}

PCB * make_PCB() {
    pidCounter++;
    struct PCB *newProcess = malloc_PCB();
    newProcess->pid = pidCounter;
    newProcess->waitingForMsg = MESSAGE_IDLE;
    newProcess->proc_message = NULL;
    return newProcess;
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
    initProcess = malloc_PCB();

    initProcess->pid = INIT_PCB_PID;
    initProcess->priority = PRIORITY_INIT;

    runningProcess = initProcess;
    
}

int create_process(Priority priority) {
    if(priority == FAILED) {
        return FAILED;
    }
    
    // Create a new process
    PCB * newProcess = make_PCB();
    newProcess->priority = priority;

    // Put new process on ready queue
    enqueue_process(newProcess);
    return newProcess->pid;
}

int quantum() {
    return run_next_process_on_CPU();
}

// TO:DO change return from int to struct
// Send a message to another process 
// Blocks process until Reply
// Returns pid, -1 on fail
// Returns reply source
// Returns text (once reply arrives)
int send(int pid, char *msg) {
    // Check if process is init
    if(runningProcess->pid == INIT_PCB_PID) {
        printf("Cannot send message from init process.\n");
        return FAILED;
    }

    // Check if process is init
    if(runningProcess->pid == pid) {
        printf("Cannot send message to the process sending.\n");
        return FAILED;
    }

    PCB * process = findProcessByPid(pid, ACTION_KEEP);
    if(process != NULL) {
        // Set message
        set_message(process, "M#WE#SAGE");

        printf("Sent %s to process: ", msg);
        print_process_info(process);
        printf("\n");

        // Set the waiting for message flag
        runningProcess->waitingForMsg = MESSAGE_WAITING;

        // Add current process to blocked list and remove from CPU
        block_current_process(blockedProcesses);

        // If process is blocked, unblock it
        PCB * result = searchListForPid(blockedProcesses, pid, ACTION_DELETE);
        if(result != NULL) {
            enqueue_process(result);
        }
    } else {
        printf("Message not sent - process with pid %d does not exist.\n", pid);
    }

    return pid;
}

int receive() {
    // Set the waiting for message flag
    runningProcess->waitingForMsg = MESSAGE_WAITING;

    // Add current process to blocked list and remove from CPU
    block_current_process(blockedProcesses);

    // Return the new running process' pid
    return runningProcess->pid;
}

int new_semaphore(int semaphoreId, int initValue) {
    // Check if number was out of bounds
    if(semaphoreId == FAILED) {
        printf("Invalid semaphore id.\n");
        return FAILED;
    } 

    SEMAPHORE * semaphore = &semaphores[semaphoreId];

    // Check if semaphore has already been added
    if(semaphore->init) {
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

    SEMAPHORE * semaphore = &semaphores[semaphoreId];

    // Check if semaphore has already been added
    if(semaphore->init == 0) {
        printf("Semaphore %d does not exist.\n", semaphoreId);
        return output;
    }

    // Check if process is init
    if(runningProcess->pid == INIT_PCB_PID) {
        printf("Cannot execute semaphore actions from init process.\n");
        return output;
    }

    // Decrease semaphore value by 1
    semaphore->value--;

    if(semaphore->value < 0) {
        output.pid_affected = runningProcess->pid;
        
        block_current_process(semaphore->blockedProcesses);

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

    SEMAPHORE * semaphore = &semaphores[semaphoreId];

    // Check if semaphore has already been added
    if(semaphore->init == 0) {
        printf("Semaphore %d does not exist.\n", semaphoreId);
        return output;
    }

    // Check if process is init
    if(runningProcess->pid == INIT_PCB_PID) {
        printf("Cannot execute semaphore actions from init process.\n");
        return output;
    }

    // Increase semaphore value by 1
    semaphore->value++;

    // Unblock and ready a process
    if(semaphore->value <= 0) {
        // Check if there are any blocked processes
        if(ListCount(semaphore->blockedProcesses) > 0) {
            // Get next blocked process
            ListFirst(semaphore->blockedProcesses);

            // Unblock process
            PCB * unblockedProcess = ListRemove(semaphore->blockedProcesses);
            
            // Add back to the ready queue
            enqueue_process(unblockedProcess);

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
    // Semaphore blocked lists
    for(int i = 0; i < 5; i++) {
        SEMAPHORE * semaphore = &semaphores[i];

        if(semaphore->init) {
            if(ListCount(semaphore->blockedProcesses) > 0) {
                printf("Semaphore %d blocked processes pid's = ", i);
                PCB * process = ListFirst(semaphore->blockedProcesses); 
                while(process != NULL) {
                    print_process_info(process);
                    process = ListNext(semaphore->blockedProcesses);
                    if(process != NULL) {
                        printf(", ");
                    }
                }
                printf(".\n");
            }
            
            else {
                printf("Semaphore %d blocked process list is empty.\n", i);
            }
        }
    }
}

