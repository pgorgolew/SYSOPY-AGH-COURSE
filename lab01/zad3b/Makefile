CC = gcc -Wall
LIB_NAME = lib_wc
RAPORT = raport3b.txt
OPT = O

static: library.c 
	$(CC) -c library.c -o $(LIB_NAME).o
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o

shared: library.c
	$(CC) -c -fPIC library.c -o $(LIB_NAME).o
	$(CC) -shared -fPIC -o ./lib$(LIB_NAME).so $(LIB_NAME).o

main: main.c
	$(CC) -c main.c

clean_without_test_files:
	rm -f *.o *.so *.a file.tmp out

clean:
	rm -f *.o *.so *.a large*.txt medium*.txt small*.txt empty.txt file.tmp out

just_static: clean_without_test_files static main 
	$(CC) main.o lib$(LIB_NAME).a -o out -${OPT}

just_shared: clean_without_test_files shared main
	$(CC) main.o -o out -L. ./lib$(LIB_NAME).so -I . -${OPT}

just_ddl: clean_without_test_files shared 
	$(CC) -c main.c -D DDL
	$(CC) -o out main.o -ldl -I . -${OPT}

create_files:
	bash generate.sh

test_all:
	make create_files
	echo "SMALL -> 10M; MEDIUM -> 50M; LARGE -> 100M\n" > ${RAPORT}
	echo "-------------STATIC-------------\n" >> ${RAPORT}
	make test_one just_lib=just_static
	echo "\n-------------SHARED-------------\n" >> ${RAPORT}
	make test_one just_lib=just_shared
	echo "\n-------------DDL-------------\n" >> ${RAPORT}
	make test_one just_lib=just_ddl
	make clean
	
test_one:
	echo "-------------OPT=O0-------------\n" >> ${RAPORT}
	make ${just_lib} OPT=O0
	make tests
	echo "\n-------------OPT=O1-------------\n" >> ${RAPORT}
	make ${just_lib} OPT=O1
	make tests
	echo "\n-------------OPT=O2-------------\n" >> ${RAPORT}
	make ${just_lib} OPT=O2
	make tests
	echo "\n-------------OPT=O3-------------\n" >> ${RAPORT}
	make ${just_lib} OPT=O3
	make tests
	echo "\n-------------OPT=Os-------------\n" >> ${RAPORT}
	make ${just_lib} OPT=Os
	make tests

tests: 	
	echo "5 small files" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 5 wc_files small1.txt small2.txt small3.txt small4.txt small5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "1 small file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 wc_files small5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "5 medium files" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 5 wc_files medium1.txt medium2.txt medium3.txt medium4.txt medium5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}
	
	echo "1 medium file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 wc_files medium5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "5 large files" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 5 wc_files large1.txt large2.txt large3.txt large4.txt large5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}
	
	echo "1 large file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 wc_files large5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "Load and remove large file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 load_to_memory large1.txt remove_block 0 | tail -4 >> ${RAPORT}
	echo "\n" >> ${RAPORT}