CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude -std=c11 -D_GNU_SOURCE

opcp: main.o copy.o buffer_size_parser.o
	$(CC) $(CFLAGS) buffer_size_parser.o main.o copy.o -o opcp

main.o: src/main.c include/opcp.h 
	$(CC) $(CFLAGS) -c src/main.c -o main.o

copy.o: src/copy.c include/opcp.h
	$(CC) $(CFLAGS) -c src/copy.c -o copy.o

buffer_size_parser.o: src/buffer_size_parser.c include/opcp.h 
	$(CC) $(CFLAGS) -c src/buffer_size_parser.c -o buffer_size_parser.o 

test: opcp
	./tests/test_basic.sh

clean:
	rm -f *.o opcp 
