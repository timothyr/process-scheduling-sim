#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "list.h"
#include "enum_priority.h"

// General
#define FAILED -1
#define SUCCESS 1

#define MAX_MESSAGE_SIZE 40

// Semaphore P
#define BLOCKED 2
#define NOT_BLOCKED 3

// Semaphore V
#define READIED 4
#define NOT_READIED 5

// Search
#define ACTION_KEEP 0
#define ACTION_DELETE 1

// Send/Receive/Reply flag
#define MESSAGE_WAITING 1
#define MESSAGE_IDLE 2

// The init process pid
#define INIT_PCB_PID 0

typedef struct PCB {
    int pid;
    Priority priority;
    int waitingForMsg;
    char * proc_message;
} PCB;

typedef struct {
    int init;
    int value;
    LIST * blockedProcesses;
} SEMAPHORE;

typedef struct {
    int pid_affected;
    int status;
    int result;
} SEMAPHORE_OUTPUT;

// Initialize
void init_sim();

// Creates a new process and puts it on the appropriate ready queue.
// Priority = 0 (high), 1 (norm), 2 (low)
// Returns pid on success, -1 on fail
int create_process(Priority priority);

// Copy the currently running process and put it on the ready queue w/ original priority
// Attempting to Fork the 'init' process should fail
// Returns pid on success, -1 on fail
int fork_process();

// Kill the process specified and remove it from the system
// pid = process id
// Returns action taken, -1 on fail
int kill_process(int pid);

// Kill the currently running process
// Returns pid of the process that gets control of CPU after
int exit_process();

// Expire the time quantum of the running process
// The current process will return to end of ready queue
// The next ready process gets control of CPU
// Returns pid of the process that gets control of CPU after
int quantum();

// TO:DO change return from int to struct
// Send a message to another process 
// Blocks process until Reply
// Returns pid, -1 on fail
// Returns reply source
// Returns text (once reply arrives)
int send(int pid, char *msg);

// TO:DO change return from int to struct
// Receive a message
// Blocks until message arrives
// Returns pid, -1 on fail
// Returns message source
// Returns text
int receive();

// TO:DO change return from int to struct
// Unblocks sender and delivers a reply
// Returns 1 on success, -1 on fail
int reply(int pid, char *msg);

// Create a new semaphore
// Initialize semaphore with new Id
// Constraint: int semaphore must be between 0 and 4
// Constraint: int initValue must be >= 0
// semaphore id must be unique (no duplicates)
// Returns semaphore id, -1 on fail
int new_semaphore(int semaphoreId, int initValue);

// Execute semaphore P on behalf of current running process
// Returns blocked status (blocked or not)
// Returns 1 on success, -1 on fail
SEMAPHORE_OUTPUT semaphore_p(int semaphoreId);

// Execute semaphore V on behalf of current running process
// Returns (whether/which) process was readied
// Returns 1 on success, -1 on fail
SEMAPHORE_OUTPUT semaphore_v(int semaphoreId);

// Dump complete state information of process to screen
// Includes process status, etc
void procinfo(int pid);

// Dump all process queues and their contents
void totalinfo();

#endif