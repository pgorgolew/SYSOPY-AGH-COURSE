#ifndef LAB06_LIB
#define LAB06_LIB

#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STOP 1
#define LIST 2
#define INIT 3
#define TO_ONE 4
#define TO_ALL 5

#define SERVER_ID 'S'
#define MAX_CLIENTS 10
#define MAX_LEN 128

typedef struct {
    long type;
    char text[MAX_LEN];
    int queue_id;
    int sender_id;
    struct tm time;
} message;

typedef struct {
    int id;
    int queue_id;
    bool is_running;
} client;

void delete_queue(int q) {
    msgctl(q, IPC_RMID, NULL);
}

void send_msg(int queue_id, message* msg){
    if(msgsnd(queue_id, msg, sizeof(message) - sizeof(msg->type), 0) == -1){
        printf("ERROR: problem with sending msg\n");
        exit(1);
    }
}

void receive_msg(int queue_id, message* msg, long type){
    if(msgrcv(queue_id, msg, sizeof(message) - sizeof(msg->type), type, 0) == -1){
        printf("ERROR: problem with receiving msg\n");
        exit(1);
    }
}

char* get_home_path(){
    char* path = getenv("HOME");
    if (path == NULL) 
        path = getpwuid(getuid())->pw_dir;

    return path;
}

#endif