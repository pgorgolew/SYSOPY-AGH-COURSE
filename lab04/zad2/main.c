#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

int nodeferer_call = 0;
//special for SA_SIGINFO https://pubs.opengroup.org/onlinepubs/007904875/functions/sigaction.
// https://www.mkssoftware.com/docs/man5/siginfo_t.5.asp
//http://manpages.ubuntu.com/manpages/trusty/pl/man2/sigaction.2.html
void handler(int sig_no, siginfo_t *info, void *context){
    printf("> Signal number: %d\n", info->si_signo);
    printf("> Process ID of sending process %d\n", info->si_pid);
    printf("> Real user ID of sending process %d\n", info->si_uid);
    printf("> Exit value or signal for process termination %d\n", info->si_status);
    printf("> Signal value %d\n", info->si_value.sival_int);
}

void handler_nodefer(int sig_no){
    int id = nodeferer_call;
    nodeferer_call++;
    printf("[STARTED] Nodeferer number: %d\n", id);
    if(nodeferer_call < 5){
        raise(SIGUSR1);
        while(nodeferer_call < 5);
    }
    printf("[FINISHED] Nodeferer number: %d\n", id);
}

void sigcation_update(struct sigaction act, void (*func)(), int flag, int sig_no){
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = flag;
    sigaction(sig_no, &act, NULL); 
}

int main(int arg_len, char **args){
    struct sigaction act;
    printf("---- TESTING SA_SIGINFO ----\n");

    printf("\n1) Raise SIGUSR1\n"); //deafult
    sigcation_update(act, handler, SA_SIGINFO, SIGUSR1);
    raise(SIGUSR1);

    printf("\n2) Sigqueue SIGUSR2\n"); //expecting specific data
    sigcation_update(act, handler, SA_SIGINFO, SIGUSR2);
    sigval_t signal_value = {99};
    sigqueue(getpid(), SIGUSR2, signal_value);

    printf("\n3) Raise SIGCHLD \n");
    sigcation_update(act, handler, SA_SIGINFO, SIGCHLD);
    if (fork()==0){
        return 0;
    }
    wait(NULL);

    printf("\n---- TESTING SA_NODEFER ----\n\n");
    sigcation_update(act, handler_nodefer, SA_NODEFER, SIGUSR1);
    //expecting zagłębianie się wywołań
    raise(SIGUSR1);


    printf("\n---- TESTING SA_NOCLDSTOP ----\n\n");
    sigcation_update(act, handler, SA_NOCLDSTOP, SIGCHLD);
    //expecting żadnego printa -> nie można wait na dziecko bo będziemy czekać w infinity
    if (fork()==0){
        raise(SIGSTOP);
    }
    printf("Sleeping for 5s \n");
    sleep(5);
    printf("Should be no info from child\n");
}