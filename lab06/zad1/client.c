#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "functions.h"

int id;
int server_queue;
int client_queue;
pid_t pid;

void handle_delete_queue(){
    delete_queue(client_queue);
    exit(0);
}

void send_LIST(){
    printf("INFO: sending message LIST\n");
    time_t now = time(NULL);
    message msg = {.type = LIST, .time=*localtime(&now)};
    send_msg(server_queue, &msg);
}

void send_STOP(){
    printf("INFO: sending message STOP\n");
    time_t now = time(NULL);
    message msg = {.sender_id = id, .type = STOP, .time=*localtime(&now)};
    send_msg(server_queue, &msg);
    handle_delete_queue();
}

void parse_msg_text(message* msg){
    printf("Enter msg:\n");
    char* msg_text = calloc(MAX_LEN, sizeof(char));
    fgets(msg_text, MAX_LEN, stdin);
    strcpy(msg->text, msg_text);
    free(msg_text);
}

void parse_to_client_queue_id(message* msg){
    printf("Enter client queue_id:\n");
    char* readed_q_id = calloc(MAX_LEN, sizeof(char));
    fgets(readed_q_id, MAX_LEN, stdin);
    msg->queue_id = atoi(readed_q_id);
    free(readed_q_id);
}

void sent_TO_ALL(){
    printf("INFO: sending message TO_ALL\n");

    time_t now = time(NULL);
    message msg = {.sender_id = id, .type = TO_ALL, .time=*localtime(&now)};
    
    parse_msg_text(&msg);

    send_msg(server_queue, &msg);
    printf("INFO: message sent TO_ALL\n");
}

void sent_TO_ONE(){
    printf("INFO: sending message TO_ONE\n");

    time_t now = time(NULL);
    message msg = {.sender_id = id, .type = TO_ONE, .time=*localtime(&now)};
    
    parse_to_client_queue_id(&msg);
    parse_msg_text(&msg);

    send_msg(server_queue, &msg);
    printf("INFO: message sent TO_ONE\n");
}

void handle_STOP(){
    printf("INFO: STOP received \n");
    time_t now = time(NULL);
    message msg = {.type = STOP, .time=*localtime(&now)};
    send_msg(server_queue, &msg);
    handle_delete_queue();
}

void handle_new_message(message* msg){
    printf("INFO: new message received!\n");

    char buff[100];
    strftime(buff, 100, "%Y-%m-%d %H:%M:%S.000", &msg->time);

    printf("FROM: %d\nTIME: %s\nTEXT: %s\n", msg->sender_id, buff, msg->text);
}

void handle_SIGINT(){
    send_STOP();
}

void initialize(){
    time_t now = time(NULL);
    message msg = {.queue_id = client_queue, .type = INIT, .time=*localtime(&now)};
    send_msg(server_queue, &msg);

    message received;
    receive_msg(client_queue, &received, INIT);

    id = received.sender_id;
    printf("INFO: Client id: %d\n", received.sender_id);
}

bool is_queue_empty(int q){
    struct msqid_ds buf;
    msgctl(q, IPC_STAT, &buf);

    if (buf.msg_qnum != 0) return false;
    return true;
}

void catcher(){
    while (!is_queue_empty(client_queue)){
        message msg;
        receive_msg(client_queue, &msg, 0);

        switch (msg.type)
        {
            case STOP:
                handle_STOP();
            case TO_ALL:
            case TO_ONE:
                handle_new_message(&msg);
                break;
            default:
                break;
        }
    }
}

// check if there is any input available in terminal
bool is_input_available(){
    fd_set set;
    struct timeval time;

    time.tv_sec = 1;
    time.tv_usec = 0;

    FD_ZERO(&set); //clean set
    FD_SET(STDIN_FILENO, &set); //add stdin descriptor
    select(STDIN_FILENO + 1, &set, NULL, NULL, &time);

    return (FD_ISSET(0, &set));
}


int main(){
    printf("---- CLIENT ----\n");

    pid = getpid();
    signal(SIGINT, handle_SIGINT);
    atexit(handle_delete_queue);

    key_t key_s = ftok(get_home_path(), SERVER_ID);
    if (key_s == -1){
        printf("ERROR: cannot generate key!\n");
        return -1;
    }

    server_queue = msgget(key_s, 0);
    if (server_queue == -1){
        printf("ERROR: cannot create server queue!\n");
        return -1;
    }

    printf("INFO: Server queue id: %d\n", server_queue);

    key_t key_c = ftok(get_home_path(), pid);
    if (key_c == -1){
        printf("ERROR: cannot generate key!\n");
        return -1;
    }

    client_queue = msgget(key_c, IPC_CREAT | IPC_EXCL | 0666);
    if (client_queue == -1){
        printf("ERROR: cannot create client queue!\n");
        return -1;
    }

    printf("INFO: Client queue id: %d\n", client_queue);


    initialize();
    char buffer[MAX_LEN];
    while(true){
        if (is_input_available()) {
            fgets(buffer, MAX_LEN, stdin);

            if (strcmp("LIST\n", buffer) == 0)
                send_LIST();
            else if (strcmp("STOP\n", buffer) == 0)
                send_STOP();
            else if (strcmp("TO_ONE\n", buffer) == 0)
                sent_TO_ONE();
            else if (strcmp("TO_ALL\n", buffer) == 0)
                sent_TO_ALL();
            else
                printf("WARNING: Unknown command: %s\n", buffer);
            
        }

        catcher();
    }

}