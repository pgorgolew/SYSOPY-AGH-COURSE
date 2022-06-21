#ifndef FUNCTIONS_H 
#define FUNCTIONS_H
#include <signal.h>

int send_signal(int catcher_pid, char* mode, int flag);

void sigcation_update(struct sigaction act, void (*func)(), int flag, int sig_no);

void sigcation_set(struct sigaction act, void (*handler)(), char* mode);

sigset_t init_mask(char* mode);

#endif