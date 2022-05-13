CC = gcc -Wall

compile:
	$(CC) cook.c -o cook -lpthread -lrt
	$(CC) supplier.c -o supplier -lpthread -lrt
	$(CC) main.c -o main -lpthread -lrt

clean:
	rm -f main cook supplier
