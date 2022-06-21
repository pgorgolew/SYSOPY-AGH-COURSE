#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include "functions.h"

int received_signals = -1;
int sigusr1_sent = 1;
pid_t sender_pid;

void handler(int sig_no, siginfo_t *info, void *context){
    if (sig_no == SIGUSR2 || sig_no == SIGRTMIN + 1){
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

    printf("[CATCHER] Catcher PID: %d\n", getpid());

    struct sigaction act;
    sigcation_set(act, handler, mode);

    sigset_t mask = init_mask(mode);
    while(sigusr1_sent){
        sigsuspend(&mask);
    }

    int sending_flag = SIGUSR1;
    int finishing_flag = SIGUSR2;
    if (strcmp(mode, "sigrt") == 0){
        sending_flag = SIGRTMIN; 
        finishing_flag = SIGRTMIN+1;
    }
        
    printf("[CATCHER] Already received %d signals\n", received_signals);

    for (int i=0; i<received_signals; i++){
        send_signal(sender_pid, mode, sending_flag);
    }

    printf("[CATCHER] Already sent %d signals\n", received_signals);
    send_signal(sender_pid, mode, finishing_flag);
    printf("[CATCHER] Finish sending with SIGUSR2/SIGRTMIN+1\n");
    printf("[CATCHER] Turn off\n");
    return 0;
}