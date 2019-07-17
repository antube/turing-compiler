#Compiler
CC = g++ -g

#Options
CFLAGS = -c -Wall

main: main.o
	$(CC) main.o -o turing-interpreter

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -rf *.o turing-interpreter