CC = $(CROSS_COMPILE)gcc

all: writer

writer: writer.o
	$(CC) writer.o -o writer

writer.o: writer.c
	$(CC) -c writer.c -o writer.o

.PHONY: clean
clean:
	rm -rf writer
	rm -rf *.o
