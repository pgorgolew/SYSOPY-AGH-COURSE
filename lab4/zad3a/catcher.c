#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

int received_signals = 0;
int sigusr1_sent = 1;
pid_t sender_pid;

void handler(int sig_no, siginfo_t *info, void *context){
    if (sig_no == SIGUSR2 || sig_no == SIGRTMIN + 2){
        sigusr1_sent = 0;
        sender_pid = info->si_pid;
    }
    received_signals++;
}

int main(int arg_len, char **args){
    if (arg_len != 2){
        printf("ERROR: you have to give: <kill|sigqueue|sigrt>\n");
        return 1;
    }

    char* mode = args[1];

    printf("[CATCHER] Catcher PID: %d", getpid());

    struct sigaction act;
    sigcation_set(act, handler, mode);

    while(sigusr1_sent){
        sigsuspend
    }

    //wysylaj

    //naura
}