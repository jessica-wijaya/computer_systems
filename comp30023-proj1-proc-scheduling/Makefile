CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm
EXE = allocate
EXE_TASK1 = task1 
OBJ_TASK1 = task1.o task2.o task3.o task4.o queue.o list.o memory.o process.o pages.o
# EXE_TASK1 = roundrobin 
# BJ_TASK1 = roundrobin.o task1.o queue.o task2.o

$(EXE): main.c $(OBJ_TASK1) 
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ_TASK1) $< $(LDFLAGS)

%.o: %.c %.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f *.o $(EXE) $(EXE_TASK1)

format:
	clang-format -i *.c *.h
