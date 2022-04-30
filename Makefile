CC = gcc
CFLAGS = -Wall -pedantic -ansi 
BIN = flow.exe
OBJ = intQueue.o graph.o arrayList.o hashTable.o inputOutput.o main.o

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ 

$(BIN): $(OBJ)
	$(CC) $^ -o $@
