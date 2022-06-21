#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>


void handler(int sig){
    printf("SIGNAL: %d RECEIVED\n", sig);
}

void is_pending(char* child_or_parent){
    sigset_t signals;
    sigpending(&signals);

    printf("SIGNAL: %d %s PENDING IN %s.\n", SIGUSR1, (sigismember(&signals, SIGUSR1)) ? "IS" : "ISN'T", child_or_parent);
}

int main(int arg_len, char **args){
    if (arg_len != 3){
        printf("ERROR: you have to give: <ignore|handler|mask|pending> <fork|exec>\n");
        return 1;
    }

    if (strcmp(args[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(args[1], "handler") == 0){
        signal(SIGUSR1, handler);
    }
    else if (strcmp(args[1], "mask") == 0){
        sigset_t blocked_set;
        sigemptyset(&blocked_set);
        sigaddset(&blocked_set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &blocked_set, NULL);
    }
    else if (strcmp(args[1], "pending") == 0){
        sigset_t blocked_set;
        sigemptyset(&blocked_set);
        sigaddset(&blocked_set, SIGUSR1);
        sigprocmask(SIG_BLOCK, &blocked_set, NULL);
    }
    else{
        printf("ERROR: unrecognized mode. Please use one of <ignore|handler|mask|pending>\n");
        return 1;
    }

    printf("RAISE IN PARENT\n");
    raise(SIGUSR1);
    is_pending("PARENT");

    if(strcmp(args[2], "fork") == 0){
		pid_t pid = fork();
		if(pid == 0){
            if (strcmp(args[1], "pending") != 0){
                printf("RAISE IN CHILD\n");
                raise(SIGUSR1);
            }

            is_pending("CHILD");
		}
		else if(pid > 0){

		}
    }
    else if(strcmp(args[2], "exec") == 0){
        if (strcmp(args[1], "handler") == 0){
            printf("ERROR: CANNOT USE 'exec' AND 'handler' TOGETHER!\n");
            return 1;
        }

        execl("./exec_out", "./exec_out", args[1], NULL);
    }
    else{
        printf("ERROR: unrecognized mode. Please use one of <fork|exec>\n");
        return 1;
    }

    int status;
    while (wait(&status) > 0);
    return 0;
}