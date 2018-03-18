#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "list.h"

void create_process_routine() {
    printf("Not yet implemented.\n");
}

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

void new_semaphore_routine() {
    printf("Not yet implemented.\n");
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

char get_command() {
    printf("Enter next command: ");
    char command;
    while((command = getchar()) == EOF || command == '\n'); // ignore EOF and \n
    return toupper(command);
}

int main (int argc, char *argv[])
{
    printf("\nProcess Control Block Simulation\n");

    print_help();
    
    int shutdownSignal = 0;
    while(shutdownSignal == 0) {
        switch(get_command()) {
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
    }

}

