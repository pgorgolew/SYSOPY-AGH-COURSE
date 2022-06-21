CC = gcc -Wall

main:
	$(CC) -c main.c $(EXTRA)
	$(CC) main.o -o out

clean:
	rm -f out main.o

run_default:
	make main
	./out ../..

run_nftw:
	make main EXTRA="-D NFTW"
	./out ../..