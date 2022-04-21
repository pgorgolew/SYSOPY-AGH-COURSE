#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define MAX_LINE_LEN 256
#define MAX_FILE_LINES 30
#define MAX_CMDS 10
#define MAX_CMD_ARGS 20
#define WRITE 1
#define READ 0

int real_file_lines = MAX_LINE_LEN;

int parse_file(char** file_lines, char* file_name){
    FILE* file = fopen(file_name, "r");
    int first_execute_index = 0, i = 0;
    char* line = (char*) calloc(MAX_LINE_LEN, sizeof(char));
    while(fgets(line, MAX_LINE_LEN * sizeof(char), file)){
        char* line_copy = (char*)calloc(MAX_LINE_LEN, sizeof(char));
        if (strchr(line, '=') == NULL){
            line[strlen(line)-1] = '\0';
            strcpy(line_copy, line);
            file_lines[i] = line_copy;
        }else{
            char* token = strtok(line, "=");
            token = strtok(NULL, "=");
            char* result = token+1;
            result[strlen(result)-1] = '\0';
            strcpy(line_copy, result);
            file_lines[i] = line_copy;
            first_execute_index = i+1;
        }
        printf("LINE NUMBER %d: %s\n", i, file_lines[i]);
        i++;
    }
    fclose(file);
    real_file_lines = i;
    return first_execute_index;
}   

char* get_num_from_cmd_str(char* str){
    char* nr_ptr = &str[7];
    return nr_ptr;
}

int parse_cmd(char* cmd, int* cmd_ids){
    char* token = strtok(cmd, "|");
    int i=0;
    while( token != NULL ) {
        if (i!=0) //tokens have empty spaces at the beginning
            token = token+1;
        const char* nr = get_num_from_cmd_str(token);
        cmd_ids[i] = atoi(nr) - 1; //all commads should have name command<number>
        token = strtok(NULL, "|");
        i++;
    }
    return i;
}

void execute_cmd(char** file_lines, int cmd_id){
    int* cmd_ids = (int*) calloc(MAX_CMDS, sizeof(int));
    int cmds_num = parse_cmd(file_lines[cmd_id], cmd_ids);
    int pipes[MAX_CMD_ARGS][2];
    for (int i = 0; i < cmds_num; i++) {
        if (pipe(pipes[i]) < 0) {
            printf("ERROR: Pipe problem\n");
        }
    }

    for (int i = 0; i < cmds_num; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i != cmds_num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            if (i != 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            for (int j = 0; j < cmds_num; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            //printf("\n%s\n%s\n%s\n%s\n", file_lines[0], file_lines[1], file_lines[2], file_lines[3]);
            char** cmd_args = (char**)calloc(MAX_CMD_ARGS, sizeof(char*));
            char* cmd_with_args = file_lines[cmd_ids[i]];
            char* token = strtok(cmd_with_args, " ");

            int arg_id = 0;
            while( token != NULL ) {
                cmd_args[arg_id] = token;
                token = strtok(NULL, " ");
                arg_id++;
            }
            cmd_args[arg_id] = NULL;

            execvp(cmd_args[0], cmd_args); 
            exit(0);
        }
    }

    for (int i = 0; i < cmds_num; i++) {
        close(pipes[i][1]);
    }

    for (int i = 0; i < cmds_num; i++) {
        wait(0);
    }
}

int execute_commands(char** file_lines, int first_execute_index){
    printf("----------------------------\n\n");
    int i=first_execute_index;
    while (i<real_file_lines){
        if (fork()==0){
            execute_cmd(file_lines, i);
            printf("ALREADY EXECUTED LINE: %d\n\n", i);
            exit(0);
        }
        i++;

    }

    int status;
    while (wait(&status) > 0);
    return 0;

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