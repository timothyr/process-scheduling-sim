
# PCB SIM

Simulate a Process Control Block

![PCB SIM](https://user-images.githubusercontent.com/21153307/54008545-04bbcf00-4135-11e9-9aec-cdeae7bef75c.png)

## How to run

```
git clone https://github.com/timothyr/process-scheduling-sim.git
cd process-scheduling-sim
make
./pcbsim
```

## Acronyms
- pcb = Process Control Block
- pid = Process ID

## Controls

- (C)reate - Create a new process
- (F)ork - Fork the current process (duplicate)
- (K)ill - Enter a pid of the process to kill
- (E)xit - Exit the PCB SIM
- (Q)uantum - The next process will be selected from the ready queue
- (S)end - Send a process a message to a specified pid. The message will be added to the message queue
- (R)eceive - Receive a message for the current pid from the message queue
- Repl(Y) - Send a reply message to a specified pid
- (N)ew Semaphore - Add a semaphore (maximum 4)
- Semaphore (P) - Decrement the value of a sempahore
- Semaphore (V) - Increment the value of a semaphore
- Proc(I)nfo - Print info on a specified pid
- (T)otalInfo - Print info about currently running process, ready queue, blocked process list, and semaphore statuses
- (H)elp - Print help menu

## Design decisions

* Send/Receive/Reply inbox can only contain 1 message. If empty, the new message is stored. If full, the new message is destroyed.

* Ready queue is fair. Higher priority new processes go before lower priority, but ONLY when they are added to the ready queue from a block or create. This means if a high priority process is expired using (Q)uantum, it is still put to the back of the ready queue behind the low priority processes. However, if the ready queue is 100% low priority processes and a high priority is (C)reated, the high priority process will "skip the line" and go to the front of the ready queue (but not pre-empt the current running process).
