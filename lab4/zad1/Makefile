CC = gcc -Wall

main:
	$(CC) -c main.c $(EXTRA)
	$(CC) main.o -o out

for_exec:
	$(CC) -c for_exec.c $(EXTRA)
	$(CC) for_exec.o -o exec_out

clean:
	rm -f *out *.o 

run: main
	./out ignore fork
	./out handler fork
	./out mask fork
	./out pending fork

run_exec: main for_exec
	./out ignore exec
	./out mask exec
	./out pending exec