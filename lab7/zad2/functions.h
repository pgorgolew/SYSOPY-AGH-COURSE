#ifndef LAB07_LIB
#define LAB07_LIB

#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdbool.h>

#define OVEN_PATH "/oven_path"
#define TABLE_PATH "/table_path"
#define MAX_PIZZAS 5

#define PREPARATION_TIME1 1
#define PREPARATION_TIME2 1
#define COOKING_TIME1 4
#define COOKING_TIME2 5
#define DELIVERY_TIME 4

#define TABLE_SEM "/table"
#define OVEN_SEM "/oven"
#define FULL_TABLE_SEM "/full_table"
#define EMPTY_TABLE_SEM "/empty_table"
#define FULL_OVEN_SEM "/full_oven"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} arg;

typedef struct{
    int array[MAX_PIZZAS];
    int id_to_add_pizza;
    int id_to_deliver_pizza;
    int pizza_count;
} table;

typedef struct{
    int array[MAX_PIZZAS];
    int id_to_add_pizza;
    int id_to_take_out_pizza;
    int pizza_count;
} oven;

void change_sem(sem_t* sem, int (*func)(sem_t* sem)){
    if ((func(sem)) == -1){
        printf("ERROR: sem_wait or sem_post problem");
        exit(1);
    }
}

sem_t* sem_open_exec(const char *name, int oflag, mode_t mode, unsigned int value){
    sem_t* sem;
    if((sem = sem_open(name, oflag, mode, value)) == SEM_FAILED){
        printf("ERROR: sem_open for args {%s, %d} problem", name, O_RDWR);
        exit(1);
    }
    return sem;
}

void sem_unlink_exec(const char *name){
    if ((sem_unlink(name)) == -1){
        printf("ERROR: sem_unlink for args {%s} problem", name);
        exit(1);
    }
}

void shm_unlink_exec(const char *name){
    if ((shm_unlink(name)) == -1){
        printf("ERROR: shm_unlink for args {%s} problem", name);
        exit(1);
    }
}

int shm_open_exec(const char *name, int oflag, mode_t mode){
    int shm_table_fd;
    if ((shm_table_fd = shm_open(name, oflag , mode)) == -1){
        printf("ERROR: shm_open for args {%s, %d, %d} problem", name, O_RDWR, 0666);
        exit(1);
    }

    return shm_table_fd;
}

void* mmap_exec(void *addr, size_t len, int prot, int flags, int fd, off_t offset){
    void * shm_table_fd;
    if ((shm_table_fd = mmap(addr, len , prot, flags, fd, offset)) == (void *) -1){
        printf("ERROR: mmap for args {addr, %ld, %d, %d, %d, %ld} problem", len, prot, flags, fd, offset);
        exit(1);
    }

    return shm_table_fd;
}

void ftruncate_exec(int fd, int size){
    if (ftruncate(fd, size) == -1){
        printf("ERROR: ftruncate for args {%d, %d} problem", fd, size);
        exit(1);
    }
}

void set_sem_value(sem_t* sem, int (*p)(sem_t*)){
    if (p(sem) == -1){
        printf("ERROR: sem_post or sem_wait problem\n");
        exit(1);
    }
}


char* get_current_time(){
    struct timeval tval;
    gettimeofday(&tval, NULL);

    char* buff = calloc(100, sizeof(char));
    strftime(buff, 100, "%Y-%m-%d %H:%M:%S", localtime(&tval.tv_sec));
    
    int msec = tval.tv_usec / 1000;
    char* current_time = calloc(100, sizeof(char));
    sprintf(current_time, "%s.%03d", buff, msec);
    
    free(buff);
    return current_time;
}

#endif