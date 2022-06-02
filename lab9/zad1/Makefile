CC = gcc -Wall -g

run: compile
	./main
	make clean

compile:
	$(CC) main.c -o main -lpthread

clean:
	rm -f main