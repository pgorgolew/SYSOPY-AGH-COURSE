#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include "functions.h"

FILE* server_logs;
client clients[MAX_CLIENTS];
int server_queue;

void delete_server_queue(){
    delete_queue(server_queue);
}

void stop_server(){
    printf("INFO: STOPPING SERVER\n");
    message msg;
    int client_queue;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].is_running) {
            printf("INFO: Sending STOP to client with id: %d\n", i);
            
            client_queue = clients[i].queue_id;
            msg.type = STOP;
            send_msg(client_queue, &msg);

            receive_msg(server_queue, &msg, STOP);
            printf("INFO: STOP received\n");
        }
    }

    delete_server_queue();
    fclose(server_logs);
    printf("INFO: SERVER STOPPED\n");
}

int assign_id(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].is_running == false){
            printf("INFO: id for client: %d\n", i);
            return i;
        }
    }
    return -1;
}

// ^C handling
void handle_SIGINT(){
    printf("INFO: SIGINT received\n");
    exit(0);
}

void add_log(message* msg){
    char buff[100];
    strftime(buff, 100, "time: %Y-%m-%d %H:%M:%S.000", &msg->time);

    char log_msg[MAX_LEN];
    sprintf(log_msg, "%s; sender_id: %d; msg_type: %ld\n", buff, msg->sender_id, msg->type);

    fputs(log_msg, server_logs);
}

void list_clients(message* msg){
    add_log(msg);
    printf("INFO: LIST received\n");
    printf("CLIENTS:\n");
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].is_running)
            printf("\tid: %d    queue_id: %d\n", clients[i].id, clients[i].queue_id);
    }
}

void send_to_one(message* msg){
    add_log(msg);
    int queue_id = msg->queue_id;
    message msg_to_send = {.type = TO_ONE, .time=msg->time};
    strcpy(msg_to_send.text, msg->text);
    send_msg(queue_id, &msg_to_send);
}

void send_to_all(message* msg){
    add_log(msg);
    for(int i=0; i<MAX_CLIENTS; i++){
        if (clients[i].is_running){
            message msg_to_send = {.type = TO_ALL, .time=msg->time};
            strcpy(msg_to_send.text, msg->text);
            send_msg(clients[i].queue_id, &msg_to_send);
        }
    }
}

void stop_sender(message* msg){
    add_log(msg);
    int client_id = msg->sender_id;
    clients[client_id].is_running = false;
    printf("INFO: STOP received from: %d\n", client_id);
}

void init_client(message* msg){
    add_log(msg);
    printf("INFO: INIT received\n");

    int queue_id = msg->queue_id;
    int id = assign_id();

    if (id == -1){
        printf("ERROR: Clients limit!");
        exit(1);
    }

    client new_client = {id, queue_id, true};
    clients[id] = new_client;

    message msg_back = {.type = INIT, .sender_id = id};
    send_msg(queue_id, &msg_back);
}

void handle_msg(message* msg){
    switch (msg->type) {
        case STOP:
            stop_sender(msg);
            break;

        case LIST:
            list_clients(msg);
            break;

        case INIT:
            init_client(msg);
            break;

        case TO_ONE:
            send_to_one(msg);
            break;

        case TO_ALL:
            send_to_all(msg);
            break;

        default:
            printf("WARNING: wrong type of message!");
    }
}

void init_clients(){
    printf("INFO: initialize clients\n");
    for(int i = 0; i < MAX_CLIENTS; i++)
        clients[i].is_running = false;
}

int main(){
    printf("---- SERVER ----\n");

    atexit(stop_server);

    key_t key = ftok(get_home_path(), SERVER_ID);
    if (key == -1){
        printf("ERROR: Generating key problem!\n");
        return -1;
    }

    server_queue = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (server_queue == -1){
        printf("ERROR: Creating queue problem!\n");
        return -1;
    }
    server_logs = fopen("server_logs.txt", "w");
    printf("INFO: Server queue with id %d\n", server_queue);

    signal(SIGINT, handle_SIGINT);

    init_clients();
    message msg;
    while(true){
        int msgsz = sizeof(message) - sizeof(msg.type);
        if (msgrcv(server_queue, &msg, msgsz, -TO_ALL-1, 0) != -1){ //-TO_ALL-1 to be able to handle all types of messages
            handle_msg(&msg);
        }
        
    }
}