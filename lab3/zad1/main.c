#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int arg_len, char **args){
    if (arg_len != 2){
        printf("ERROR: you have to give only number of processes\n");
        return 1;
    }
    char *ptr;
    int n = strtol(args[1], &ptr, 10);

    if (n == 0){
        printf("ERROR: Wrong number, pointer value: `%s`\n", ptr);
        return 1;
    }

    printf("Parent process with PID: %d\n", getpid());
    pid_t curr_pid;
    for (int i = 0; i<n; i++){
        curr_pid = fork();
        if (curr_pid == 0){
            printf("Child process (no. %d) with PID: %d\n", i+1, getpid());
            return 0;
        }
    }

    for (int i=0; i<n; i++) 
        wait(NULL);
    
    return 0;
}