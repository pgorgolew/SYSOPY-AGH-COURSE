CC = gcc -Wall
LIB_NAME = lib_wc

both:
	make static
	make shared

static: library.c 
	$(CC) -g -c library.c -o $(LIB_NAME).o
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o

shared: library.c
	$(CC) -g -c -fPIC library.c -o $(LIB_NAME).o
	$(CC) -g -shared -o lib$(LIB_NAME).so $(LIB_NAME).o

main: main.c
	$(CC) -c main.c

out: main.o library.o
	$(CC) main.o lib$(LIB_NAME).a -o out

just_static: clean static main out

clean:
	rm -f *.o *.so *.a file.tmp out