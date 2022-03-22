CC = gcc -Wall

main:
	$(CC) -c main.c $(EXTRA)
	$(CC) main.o -o out

clean:
	rm -f out result.txt

run_sys:
	make clean
	make main
	./out test.txt result.txt

run_lib:
	make clean
	make main EXTRA="-D LIB"
	./out test.txt result.txt