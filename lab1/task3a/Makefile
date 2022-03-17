CC = gcc -Wall
LIB_NAME = lib_wc
RAPORT = raport3a.txt

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
	$(CC) main.o lib$(LIB_NAME).a -o out

just_shared: clean_without_test_files shared main
	$(CC) main.o -o out -L. ./lib$(LIB_NAME).so -I .

just_ddl: clean_without_test_files shared 
	$(CC) -c main.c -D DDL
	$(CC) -o out main.o -ldl -I . 

create_files:
	bash generate.sh
	
valgrind_elo:
	make just_static
	valgrind --leak-check=yes -s ./out create_table 10 wc_files large5.txt medium10.txt small1.txt remove_block 0 

test_all:
	make create_files
	echo "SMALL -> 10M; MEDIUM -> 50M; LARGE -> 100M\n" > ${RAPORT}
	echo "-------------STATIC-------------\n" >> ${RAPORT}
	make just_static
	make tests
	echo "\n-------------SHARED-------------\n" >> ${RAPORT}
	make just_shared
	make tests
	echo "\n-------------DDL-------------\n" >> ${RAPORT}
	make just_ddl
	make tests
	make clean
	

tests: 	
	echo "10 small files" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 10 wc_files small1.txt small2.txt small3.txt small4.txt small5.txt small6.txt small7.txt small8.txt small9.txt small10.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "5 small files" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 5 wc_files small1.txt small2.txt small3.txt small4.txt small5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "1 small file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 wc_files small5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "10 medium files" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 10 wc_files medium1.txt medium2.txt medium3.txt medium4.txt medium5.txt medium6.txt medium7.txt medium8.txt medium9.txt medium10.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "5 medium files" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 5 wc_files medium1.txt medium2.txt medium3.txt medium4.txt medium5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}
	
	echo "1 medium file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 wc_files medium5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "10 large files" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 10 wc_files large1.txt large2.txt large3.txt large4.txt large5.txt large6.txt large7.txt large8.txt large9.txt large10.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "5 large files" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 5 wc_files large1.txt large2.txt large3.txt large4.txt large5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}
	
	echo "1 large file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 wc_files large5.txt | tail -3 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "Loading and remove small file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 load_to_memory small1.txt remove_block 0 | tail -4 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "Loading and remove medium file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 load_to_memory medium1.txt remove_block 0 | tail -4 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "Load and remove large file" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 1 load_to_memory large1.txt remove_block 0 | tail -4 >> ${RAPORT}
	echo "\n" >> ${RAPORT}

	echo "3x(3xload then 3xremove)" >> ${RAPORT}
	LD_LIB_PATH=${PWD}; ./out create_table 9 load_to_memory empty.txt load_to_memory empty.txt load_to_memory empty.txt remove_block 0 remove_block 1 remove_block 2 load_to_memory empty.txt load_to_memory empty.txt load_to_memory empty.txt remove_block 3 remove_block 4 remove_block 5 load_to_memory empty.txt load_to_memory empty.txt load_to_memory empty.txt remove_block 6 remove_block 7 remove_block 8 | tail -20 >> ${RAPORT}
	
