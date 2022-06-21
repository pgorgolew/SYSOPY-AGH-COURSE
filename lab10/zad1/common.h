#ifndef LAB10_COMMON_H
#define LAB10_COMMON_H


#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define MAX_MSG_LEN 256

#define START 101
#define WAIT_FOR_OPPONENT 102
#define WAIT_FOR_MOVE 103
#define OPPONENT_MOVE 104
#define MOVE 105
#define QUIT 106

#define NONE 11
#define SIGN_O 12 
#define SIGN_X 13

typedef struct{
    char* name;
    int fd;
    bool available;
} client;


typedef struct{
    int move;
    int objects[9];
} tic_tac_toe;

#endif //LAB10_COMMON_H