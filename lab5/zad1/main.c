#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define MAX_LINE_LEN 256
#define MAX_FILE_LINES 30
#define MAX_CMDS 10

int real_file_lines = MAX_LINE_LEN;

int parse_file(char** file_lines, char* file_name){
    FILE* file = fopen("r", file_name);
    char* line = calloc(256, sizeof(char));
    int first_execute_index = 0, i = 0;
    while(fgets(file_lines[i], MAX_LINE_LEN, file)){
        if (first_execute_index == 0 && strchr('=', file_lines[i]))
            first_execute_index = i;
        i++;
    }

    real_file_lines = i;
    return first_execute_index;
}   

void execute_commands(char** file_lines, int first_execute_index){
   int i=first_execute_index;
   
    while (i<real_file_lines){
        if (fork()==0)
            execute_cmd(file_lines, i);
    }

    int status;
    while (wait(&status) > 0);
    return 0;

}

void execute_cmd(char** file_lines, int cmd_id){
    int* cmd_ids = (int*) calloc(MAX_CMDS, sizeof(int));
    char* cmd = file_lines[cmd_id];
    int cmds_num = parse_cmd(cmd, cmd_ids);
    for (int i=0; i<cmds_num; i++){
        if (i+1 == cmds_num){
            //last 
        } else if(i == 0){
            //first
        } else{
            //middle
        }
    }
    free(cmd_ids);
}

int parse_cmd(char* cmd, int* cmd_ids){
    char* token = strtok(cmd, "|");
    
    int i=0;
    while( token != NULL ) {
        cmd_ids[i] = atoi(token[7]) - 1; //all commads should have name command<number>
        token = strtok(NULL, "|");
        i++;
    }

    return i;
}

int main(int arg_len, char **args){
    if (arg_len != 2){
        printf("ERROR: you have to add filename with commands!\n");
        return 1;
    }

    char* file_name = args[1];
    char** file_lines = (char**)calloc(MAX_FILE_LINES, sizeof(char*));
    int first_execute_index = parse_file(file_lines, file_name);
    execute_commands(file_lines, first_execute_index);

    free(file_lines);
    return 0;
}