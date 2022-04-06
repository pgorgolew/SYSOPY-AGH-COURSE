#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include "functions.h"

int received_signals = 0;
int sigusr1_sent = 1;

void handler(int sig_no, siginfo_t *info, void *context){
    if (sig_no == SIGUSR2 || sig_no == SIGRTMIN + 2){
        sigusr1_sent = 0;
    }
    received_signals++;
}

int main(int arg_len, char **args){
    if (arg_len != 4){
        printf("[SENDER] ERROR: you have to give: <CATCHER_PID> <signals_number> <kill|sigqueue|sigrt>\n");
        return 1;
    }

    int catcher_pid = atoi(args[1]);
    int signals_num = atoi(args[2]);
    char* mode = args[3];

    printf("[SENDER] Sender PID: %d\n", getpid());

    for (int i=0; i<signals_num; i++){
        send_signal(catcher_pid, mode, SIGUSR1);
    }

    printf("[SENDER] Already sent %d signals\n", signals_num);
    send_signal(catcher_pid, mode, SIGUSR2);
    printf("[SENDER] Finish sending with SIGUSR2\n");

    struct sigaction act;
    sigcation_set(act, handler, mode);

    sigset_t mask = init_mask(mode);
    while(sigusr1_sent){
        sigsuspend(&mask);
    }
    printf("[SENDER] Already catched %d signals from catcher\n", received_signals);
    
    printf("[SENDER] Turn off\n");
    return 0;
}