#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 256

void send_email(char* address, char* topic, char* content){
    char command[MAX_LINE_LEN];
    snprintf(command, sizeof(command), "echo %s | mail %s -s %s", content, address, topic);
    printf("%s\n", command);
    popen(command, "r");

    printf("Email details:\n");
    printf("To:\t\t%s\nTopic:\t\t%s\nCONTENT:\t%s\n", address, topic, content);
}

void show_mails(char* order_mode){
    char* command;

    if (strcmp(order_mode, "date") == 0)
        command = "echo | mail -H";
    else if (strcmp(order_mode, "sender") == 0)
        command = "echo | mail -H | sort -k 3";
    else{
        printf("ERROR: Urecognized mode.\n");
        exit(1);
    }

    FILE* file = popen(command, "r");
    printf("\nMAILS SORTED BY '%s':\n", order_mode);
    char* line = (char*)calloc(MAX_LINE_LEN, sizeof(char));
    while(fgets(line, MAX_LINE_LEN, file) != NULL){
        printf("%s\n", line);
    }
    free(line);
}

int main(int arg_len, char **args){
    if (arg_len == 1 || arg_len == 3 || arg_len > 4){
        printf("ERROR: must give <mail> <title> <content> or <sorting_mode>");
        return 1;
    }

    if (arg_len == 2){
        show_mails(args[1]);
        return 0;
    }

    char* address = args[1];
    char* topic = args[2];
    char* content = args[3];
    send_email(address, topic, content);
    return 0;
}