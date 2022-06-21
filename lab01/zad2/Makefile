CC = gcc -Wall
LIB_NAME = lib_wc

both:
	make static
	make shared

static: library.c 
	$(CC) -g -c library.c -o $(LIB_NAME).o
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o

shared: library.c
	$(CC) -g -c -fPIC library.c -o $(LIB_NAME).o
	$(CC) -g -shared -o lib$(LIB_NAME).so $(LIB_NAME).o

main: main.c
	$(CC) -c main.c

out: main.o library.o
	$(CC) main.o lib$(LIB_NAME).a -o out

clean:
	rm -f *.o *.so *.a large*.txt medium*.txt small*.txt empty.txt file.tmp out

just_static: clean static main out

create_files:
	bash generate.sh

get_raport_2: clean just_static create_files
	echo "SMALL -> 10M; MEDIUM -> 50M; LARGE -> 100M\n" > raport2.txt
	
	echo "10 small files" >> raport2.txt
	./out create_table 10 wc_files small1.txt small2.txt small3.txt small4.txt small5.txt small6.txt small7.txt small8.txt small9.txt small10.txt | tail -3 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "5 small files" >> raport2.txt
	./out create_table 5 wc_files small1.txt small2.txt small3.txt small4.txt small5.txt | tail -3 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "1 small file" >> raport2.txt
	./out create_table 1 wc_files small5.txt | tail -3 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "10 medium files" >> raport2.txt
	./out create_table 10 wc_files medium1.txt medium2.txt medium3.txt medium4.txt medium5.txt medium6.txt medium7.txt medium8.txt medium9.txt medium10.txt | tail -3 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "5 medium files" >> raport2.txt
	./out create_table 5 wc_files medium1.txt medium2.txt medium3.txt medium4.txt medium5.txt | tail -3 >> raport2.txt
	echo "\n" >> raport2.txt
	
	echo "1 medium file" >> raport2.txt
	./out create_table 1 wc_files medium5.txt | tail -3 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "10 large files" >> raport2.txt
	./out create_table 10 wc_files large1.txt large2.txt large3.txt large4.txt large5.txt large6.txt large7.txt large8.txt large9.txt large10.txt | tail -3 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "5 large files" >> raport2.txt
	./out create_table 5 wc_files large1.txt large2.txt large3.txt large4.txt large5.txt | tail -3 >> raport2.txt
	echo "\n" >> raport2.txt
	
	echo "1 large file" >> raport2.txt
	./out create_table 1 wc_files large5.txt | tail -3 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "Loading and remove small file" >> raport2.txt
	./out create_table 1 load_to_memory small1.txt remove_block 0 | tail -4 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "Loading and remove medium file" >> raport2.txt
	./out create_table 1 load_to_memory medium1.txt remove_block 0 | tail -4 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "Load and remove large file" >> raport2.txt
	./out create_table 1 load_to_memory large1.txt remove_block 0 | tail -4 >> raport2.txt
	echo "\n" >> raport2.txt

	echo "3x(3xload then 3xremove)" >> raport2.txt
	./out create_table 9 load_to_memory empty.txt load_to_memory empty.txt load_to_memory empty.txt remove_block 0 remove_block 1 remove_block 2 load_to_memory empty.txt load_to_memory empty.txt load_to_memory empty.txt remove_block 3 remove_block 4 remove_block 5 load_to_memory empty.txt load_to_memory empty.txt load_to_memory empty.txt remove_block 6 remove_block 7 remove_block 8 | tail -20 >> raport2.txt
	make clean
