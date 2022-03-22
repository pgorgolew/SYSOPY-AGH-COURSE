CC = gcc -Wall

main:
	$(CC) -c main.c 
	$(CC) main.o -o out

clean:
	rm -f out main

run:
	make main
	./out test.txt result.txt