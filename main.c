#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "list.h"
#include "main.h"

#define FAILED -1

SEMAPHORE semaphores[5];

void init_sim() {
    SEMAPHORE nullSemaphore;
    nullSemaphore.init = 0;
    // Make all semaphores are uninitialized
    for(int i = 0; i < 5; i++) {
        semaphores[i] = nullSemaphore;
    }


    
}

int get_input_unbounded_int() {
    char command;
    while((command = getchar()) == EOF || command == '\n'); // ignore EOF and \n
    const char * char_input = &command;
    return atoi(char_input);
}

int get_input_bounded_int(int min, int max) {
    int num = get_input_unbounded_int();
    if(num >= min && num <= max) {
        return num;
    } else {
        return FAILED;
    }
}

Priority get_input_priority() {
    printf("Priority types: 0 (high), 1 (norm), 2 (low)\n");
    printf("Enter the process priority: ");
    char command;
    while((command = getchar()) == EOF || command == '\n'); // ignore EOF and \n
    switch(command) {
        case '0':
            return PRIORITY_HIGH;
        case '1':
            return PRIORITY_NORM;
        case '2':
            return PRIORITY_LOW;
        default:
            return FAILED;
    }
}

// ****** Create

void create_process_routine() {
    enum Priority newProcessPriority = get_input_priority();
    create_process(newProcessPriority);
}

int create_process(Priority priority) {
    if(priority == FAILED) {
        return FAILED;
    }
    // TO:DO add process to list
}

// ****** Fork

void fork_process_routine() {
    printf("Not yet implemented.\n");
}

void kill_process_routine() {
    printf("Not yet implemented.\n");
}

void exit_routine() {
    printf("Not yet implemented.\n");
}

void quantum_routine() {
    printf("Not yet implemented.\n");
}

void send_routine() {
    printf("Not yet implemented.\n");
}

void receive_routine() {
    printf("Not yet implemented.\n");
}

void reply_routine() {
    printf("Not yet implemented.\n");
}

// ********** New Semaphore
void new_semaphore_routine() {
    // Get new semaphore id
    printf("Enter a new semaphore id between %d and %d: ", 0, 4);
    int newSemaphoreId = get_input_bounded_int(0, 4);
    
    // Get initial value
    printf("Enter the new semaphore's initial value: ");
    int initValue = get_input_unbounded_int();

    // Create 
    int result = new_semaphore(newSemaphoreId, initValue);

    if(result != FAILED) {
        printf("Semaphore %d added.\n", result);
    }
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

void semaphore_p_routine() {
    printf("Not yet implemented.\n");
}

void semaphore_v_routine() {
    printf("Not yet implemented.\n");
}

void procinfo_routine() {
    printf("Not yet implemented.\n");
}

void totalinfo_routine() {
    printf("Not yet implemented.\n");
}

void print_help() {
    printf("(C)reate, (F)ork, (K)ill, (E)xit, (Q)uantum,\n");
    printf("(S)end, (R)eceive, Repl(Y), (H)elp,\n");
    printf("(N)ew Semaphore, Semaphore (P), Semaphore (V), Proc(I)nfo, (T)otalInfo\n");
}

char get_input_command() {
    printf("Enter next command: ");
    char command;
    while((command = getchar()) == EOF || command == '\n'); // ignore EOF and \n
    return toupper(command);
}

int main (int argc, char *argv[])
{
    printf("\nProcess Control Block Simulation\n");

    init_sim();

    print_help();
    
    int shutdownSignal = 0;
    while(shutdownSignal == 0) {
        switch(get_input_command()) {
            case 'C':
                create_process_routine();
                break;
            case 'F':
                fork_process_routine();
                break;
            case 'K':
                kill_process_routine();
                break;
            case 'E':
                exit_routine();
                break;
            case 'Q':
                quantum_routine();
                break;
            case 'S':
                send_routine();
                break;
            case 'R':
                receive_routine();
                break;
            case 'Y':
                reply_routine();
                break;
            case 'N':
                new_semaphore_routine();
                break;
            case 'P':
                semaphore_p_routine();
                break;
            case 'V':
                semaphore_v_routine();
                break;
            case 'I':
                procinfo_routine();
                break;
            case 'T':
                totalinfo_routine();
                break;
            case 'H':
                print_help();
                break;
            default:
                printf("Command not recognized.\n");
                break;
        }
    } // end of while
}

