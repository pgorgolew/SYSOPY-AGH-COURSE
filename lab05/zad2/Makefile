CC = gcc -Wall

main:
	$(CC) -c main.c $(EXTRA)
	$(CC) main.o -o out

clean:
	rm -f *out *.o 

run_order: main
	./out date

run_send: main
	./out gorgolew@debian-sysopy Lab5 TEST