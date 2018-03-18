CC = gcc
CFLAGS = -g
PROG = pcbsim
OBJS = list.o main.o

pcbsim: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

main.o: main.c main.h enum_priority.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm *.o pcbsim
