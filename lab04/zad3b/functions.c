#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

int send_signal(int catcher_pid, char* mode, int flag){
    if (strcmp("kill", mode) == 0){
        kill(catcher_pid, flag);
    }
    else if (strcmp("sigqueue", mode) == 0){
        sigval_t signal_value = {99};
        sigqueue(catcher_pid, flag, signal_value);
    }
    else if (strcmp("sigrt", mode) == 0){
        kill(catcher_pid, flag);
    }
    else {
        printf("ERROR: Unrecognized mode!\n");
        return -1;
    }
    return 0;
}

void sigcation_update(struct sigaction act, void (*func)(), int flag, int sig_no){
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = flag;
    sigaction(sig_no, &act, NULL); 
}

void sigcation_set(struct sigaction act, void (*handler)(), char* mode){
    if (strcmp("sigrt", mode) == 0){
        sigcation_update(act, handler, SA_SIGINFO, SIGRTMIN);
        sigcation_update(act, handler, SA_SIGINFO, SIGRTMIN+1);
    }
    else{
        sigcation_update(act, handler, SA_SIGINFO, SIGUSR1);
        sigcation_update(act, handler, SA_SIGINFO, SIGUSR2);
    }
}

sigset_t init_mask(char* mode){
    sigset_t mask;
    sigfillset(&mask);
    if (strcmp("sigrt", mode) == 0){
        sigdelset(&mask, SIGRTMIN);
        sigdelset(&mask, SIGRTMIN+1);
    }
    else{
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
    }
    return mask;
}

void block_signals(char* mode){
	sigset_t block_sigset;

    if (strcmp("sigrt", mode) == 0){
        sigaddset(&block_sigset, SIGRTMIN);
        sigaddset(&block_sigset, SIGRTMIN+1);
    }
    else {
        sigaddset(&block_sigset, SIGUSR2);
        sigaddset(&block_sigset, SIGUSR1);
    }
    
	sigprocmask(SIG_BLOCK, &block_sigset, NULL);
}