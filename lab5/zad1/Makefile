CC = gcc -Wall

main:
	$(CC) -c main.c $(EXTRA)
	$(CC) main.o -o out

clean:
	rm -f *out *.o 

run: main
	./out cmds.txt