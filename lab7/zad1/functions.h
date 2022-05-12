#ifndef LAB07_LIB
#define LAB07_LIB

#include <stdlib.h>
#include <pwd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define SEM_ID 'S'
#define OVEN_ID 'O'
#define TABLE_ID 'T'
#define OVEN_PATH "cook.c"
#define TABLE_PATH "supplier.c"
#define MAX_PIZZAS 5

#define PREPARATION_TIME1 1
#define PREPARATION_TIME2 1
#define COOKING_TIME1 4
#define COOKING_TIME2 5
#define DELIVERY_TIME 4

#define TABLE_SEM 0
#define OVEN_SEM 1
#define FULL_TABLE_SEM 2
#define EMPTY_TABLE_SEM 3
#define FULL_OVEN_SEM 4

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

char* get_home_path(){
    char* path = getenv("HOME");
    if (path == NULL) 
        path = getpwuid(getuid())->pw_dir;

    return path;
}

void set_sem_value(int sem_id, int semnum, int cmd, int value){
    union semun arg = {.val = value};
    if (semctl(sem_id, semnum, cmd, arg) == -1){
        printf("ERROR: semctl for args {%d, %d, %d, %d} problem", sem_id, semnum, cmd, value);
        exit(1);
    }
}

key_t get_key(char* path, int id){
    key_t key;
    if ((key = ftok(path, id)) == -1){
        printf("ERROR: ftok for args {%s, %d} problem\n", path, id);
        exit(1);
    }

    return key;
}

int shmget_exec(key_t key, size_t size, int flags){
    int result;
    if((result = shmget(key, size, flags)) == -1){
        printf("ERROR: shmget for args {%d, %ld, %d} problem\n", key, size, flags);
        exit(1);
    }

    return result;
}

int semget_exec(key_t key, int nsems, int flag){
    int sem_id;
    if((sem_id = semget(key, nsems, flag)) == -1){
        printf("ERROR: semget for args {%d, %d, %d} problem\n", key, nsems, flag);
        exit(1);
    }

    return sem_id;
}

void shmat_exec(int shmid, const void *shmaddr, int shmflg){
    if (shmat(shmid, shmaddr, shmflg) == (void *) -1){
        printf("ERROR: shmctl for args {%d, %p, %d} problem\n", shmid, shmaddr, shmflg);
        exit(1);
    }
}

void shmctl_exec(int shmid, int cmd, struct shmid_ds *buf ){
    if (shmctl(shmid, cmd, buf) == -1){
        printf("ERROR: shmctl for args {%d, %d, shmid_ds here} problem\n", shmid, cmd);
        exit(1);
    }
}

void change_sem(int sem_id, int sem_num, int sem_op){
    struct sembuf s = {.sem_num = sem_num, .sem_op = sem_op};
    if (semop(sem_id, &s, 1) == -1){
        printf("ERROR: semop for args {%d, %d, %d} problem\n", sem_id, sem_num, sem_op);
        exit(1);
    }
}

int get_sem_id(){
    key_t key = get_key(get_home_path(), SEM_ID);
    return semget_exec(key, 5, 0);
}

int get_shm_oven_id(){
    key_t key_oven = get_key(OVEN_PATH, OVEN_ID);
    return shmget_exec(key_oven, sizeof(oven), 0);
}

int get_shm_table_id(){
    key_t key_table = ftok(TABLE_PATH, TABLE_ID);
    return shmget_exec(key_table, sizeof(table), 0);
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