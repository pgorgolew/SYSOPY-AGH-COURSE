CC = gcc -Wall
LIB_NAME = libfunctions

reset: clean lib catcher sender

sender:
	$(CC) -c sender.c
	$(CC) sender.o $(LIB_NAME).a -o sender

catcher:
	$(CC) -c catcher.c
	$(CC) catcher.o $(LIB_NAME).a -o catcher

lib:
	$(CC) -c functions.c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o

clean:
	rm -f catcher sender *.o  *.a