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
    is_pending("EXEC");
    if (strcmp(args[1], "pending") != 0){
        printf("RAISE AFTER EXEC\n");
        raise(SIGUSR1);
        is_pending("EXEC AFTER RAISE");
    }

    return 0;
}