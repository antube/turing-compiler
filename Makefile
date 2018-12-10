#Compiler
CC = g++ -g

#Options
CFLAGS = -c -Wall

main: main.o
	$(CC) main.o -o TuringMachine

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -rf *o TuringMachine