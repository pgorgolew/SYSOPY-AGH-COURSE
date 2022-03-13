CC = gcc -Wall
LIB_NAME = lib_wc

both:
	make static
	make shared

static: library.c 
	$(CC) -c library.c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o

shared: library.c
	$(CC) -c -fPIC library.c -o $(LIB_NAME).o
	$(CC) -fPIC -shared -o $(LIB_NAME).so $(LIB_NAME).o

clean:
	rm -f *.o *.so *.a file.tmp