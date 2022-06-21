CC = gcc
CFLAGS = -Wall -g

compile: clean
	$(CC) $(FLAGS) server.c -o server -lpthread -lm
	$(CC) $(FLAGS) client.c -o client -lpthread -lm

run_client:
	./client $(PLAYER_NAME) unix socket

run_server:
	./server 7001 socket

clean:
	rm -f server client socket