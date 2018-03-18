
# PCB SIM

## Design decisions

* Send/Receive/Reply inbox can only contain 1 message. If empty, the new message is stored. If full, the new message is destroyed.

* Ready queue is fair. Higher priority new processes go before lower priority, but ONLY when they are added to the ready queue from a block or create. This means if a high priority process is expired using (Q)uantum, it is still put to the back of the ready queue behind the low priority processes. However, if the ready queue is 100% low priority processes and a high priority is (C)reated, the high priority process will "skip the line" and go to the front of the ready queue (but not pre-empt the current running process).