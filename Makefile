CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude -std=c11

opcp: main.o copy.o 
	$(CC) $(CFLAGS) main.o copy.o -o opcp

main.o: src/main.c include/opcp.h 
	$(CC) $(CFLAGS) -c src/main.c -o main.o

copy.o: src/copy.c include/opcp.h
	$(CC) $(CFLAGS) -c src/copy.c -o copy.o

test: opcp
	./tests/test_basic.sh

clean:
	rm -f *.o opcp 
