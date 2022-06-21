CC = gcc -Wall
REPORT = pomiar_zad_1.txt

main:
	$(CC) -c main.c $(EXTRA)
	$(CC) main.o -o out

clean:
	rm -f out result.txt test* 50_kb.txt main.o

sys:
	make main

lib:
	make main EXTRA="-D LIB"

run_with_time:
	time -p ./out ${TEST} result.txt
	
test:
	bash generate.sh
	make sys
	echo "-----SYS TEST-----" > ${REPORT}
	make run_tests
	make lib
	echo "\n\n-----LIB TEST-----" >> ${REPORT}
	make run_tests
	make clean

run_tests:
	echo "\nFile with 300KB\n" >> ${REPORT}
	make run_with_time TEST=test_300.txt 2>> ${REPORT}
	echo "\nFile with 600KB\n" >> ${REPORT}
	make run_with_time TEST=test_600.txt 2>> ${REPORT}
	echo "\nFile with 900KB\n" >> ${REPORT}
	make run_with_time TEST=test_900.txt 2>> ${REPORT}