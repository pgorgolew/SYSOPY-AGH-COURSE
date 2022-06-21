CC = gcc -Wall -g

compile:
	$(CC) main.c -o main -lpthread

run_tests: clean compile
	./main 1 numbers baboon.pgm baboon_negative.pgm
	./main 1 block baboon.pgm baboon_negative.pgm
	./main 2 numbers baboon.pgm baboon_negative.pgm
	./main 2 block baboon.pgm baboon_negative.pgm
	./main 4 numbers baboon.pgm baboon_negative.pgm
	./main 4 block baboon.pgm baboon_negative.pgm
	./main 8 numbers baboon.pgm baboon_negative.pgm
	./main 8 block baboon.pgm baboon_negative.pgm
	./main 16 numbers baboon.pgm baboon_negative.pgm
	./main 16 block baboon.pgm baboon_negative.pgm

clean:
	rm -f main Times.txt baboon_negative.pgm