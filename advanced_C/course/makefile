CC = gcc
CFLAGS = -c -std=c99 -Wall
OBJECTS = calc.o 
EXEC = calc.exe

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) -o $(EXEC) $(OBJECTS)

calc.o: calc.c
	$(CC) $(CFLAGS) -o calc.o calc.c

.PHONY: clean

clean:
	C:\msys64\usr\bin\rm.exe -f $(OBJECTS) $(EXEC)