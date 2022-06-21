CC = gcc -Wall
FILE = raport.txt
main:
	$(CC) -c main.c $(EXTRA)
	$(CC) main.o -o out

clean:
	rm -f out main.o w*.txt

run:
	./out ${processes} | tail -3 >> ${FILE}
	echo "" >> ${FILE}

tests: main
	echo "FULL TESTS\n" > ${FILE}
	make single_test processes=100
	make single_test processes=200
	make single_test processes=400
	make single_test processes=600
	make single_test processes=800
	make single_test processes=1000
	make single_test processes=1500
	make single_test processes=2000
	make single_test processes=3000
	make single_test processes=5000
	make clean

single_test:
	make run processes=${processes}