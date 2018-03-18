CC = gcc
CFLAGS = -g
PROG = pcbsim
OBJS = list.o scheduler.o main.o

pcbsim: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

scheduler.o: scheduler.c scheduler.h list.h enum_priority.h
	$(CC) $(CFLAGS) -c scheduler.c

main.o: main.c main.h 
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o pcbsim
