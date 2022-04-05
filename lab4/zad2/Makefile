CC = gcc -Wall

main:
	$(CC) -c main.c $(EXTRA)
	$(CC) main.o -o out

run: main
	./out

clean:
	rm -f *out *.o 