#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#define ELVES 10
#define REINDEERS 9
#define MAX_ELVES_WAITING 3
#define DELIVERIES_TO_DO 3

struct santas_employee{
    pthread_t thread;
    int id;
};

pthread_t santa_thread;
struct santas_employee elves[ELVES];
struct santas_employee reindeers[REINDEERS];

int deliveries_done = 0;

int ready_reindeers = 0;
bool reindeers_can_go_holiday = true;

int elves_waiting = 0;
int queue_to_santa[MAX_ELVES_WAITING];

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond   = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_wait_cond   = PTHREAD_COND_INITIALIZER;

pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_wait_cond   = PTHREAD_COND_INITIALIZER;

void check_if_deliveries_done(){
    if (deliveries_done == DELIVERIES_TO_DO)
        exit(0);
}

int get_random_int(int min_value, int max_value){
    int diff = max_value - min_value + 1;
    return rand() % diff + min_value;
}

void santa_wait_to_be_woken(){
    pthread_mutex_lock(&santa_mutex);
    if (elves_waiting < MAX_ELVES_WAITING && ready_reindeers < REINDEERS){
        printf("[SANTA]:\tI'M SLEEPING\n");
        pthread_cond_wait(&santa_cond, &santa_mutex);
    }
    pthread_mutex_unlock(&santa_mutex);

    printf("[SANTA]:\tWAKING UP, ELVES: %d, REINDEERS %d\n", elves_waiting, ready_reindeers);
}

void santa_deliver_toys(){
    pthread_mutex_lock(&reindeer_mutex);
    if (ready_reindeers == REINDEERS) {
        deliveries_done++;
        printf("[SANTA]:\tDELIVERING TOYS FOR GOOD STUDENTS (delivery number: %d)\n", deliveries_done);
        sleep(get_random_int(2,4));
        ready_reindeers = 0;

        // enable reindeers to go holiday
        pthread_mutex_lock(&reindeer_wait_mutex);
        reindeers_can_go_holiday = true;
        pthread_cond_broadcast(&reindeer_wait_cond);
        pthread_mutex_unlock(&reindeer_wait_mutex);
    }
    pthread_mutex_unlock(&reindeer_mutex);
}

void santa_solve_elves_problems(){
    pthread_mutex_lock(&elf_mutex);
    if (elves_waiting == MAX_ELVES_WAITING) {
        printf("[SANTA]:\tSOLVING ELVES PROBLEMS (ELVES IDS: %d, %d, %d)\n", queue_to_santa[0], queue_to_santa[1], queue_to_santa[2]);
        sleep(get_random_int(1,2));

        // enable other elves to come
        pthread_mutex_lock(&elf_wait_mutex);
        elves_waiting = 0;
        pthread_cond_broadcast(&elf_wait_cond);
        pthread_mutex_unlock(&elf_wait_mutex);

    }
    pthread_mutex_unlock(&elf_mutex);
}

void reindeer_wait_to_go_holiday(){
    check_if_deliveries_done();
    pthread_mutex_lock(&reindeer_wait_mutex);
    while (!reindeers_can_go_holiday) {
        pthread_cond_wait(&reindeer_wait_cond, &reindeer_wait_mutex);
    }
    pthread_mutex_unlock(&reindeer_wait_mutex);
}

void reindeer_wait_for_others(int id){
    check_if_deliveries_done();
    pthread_mutex_lock(&reindeer_mutex);
    ready_reindeers++;
    printf("[REINDEER]:\tWAITING %d REINDEERS FOR SANTA, %d\n", ready_reindeers, id);
    reindeers_can_go_holiday = false;

    if (ready_reindeers == REINDEERS) {
        printf("[REINDEER]:]\tWAKING UP SANTA, %d\n", id);
        pthread_mutex_lock(&santa_mutex);
        pthread_cond_broadcast(&santa_cond);
        pthread_mutex_unlock(&santa_mutex);
    }
    pthread_mutex_unlock(&reindeer_mutex);
}

void elf_waiting_for_others_to_return(int id){
    check_if_deliveries_done();
    pthread_mutex_lock(&elf_wait_mutex);
    while (elves_waiting == MAX_ELVES_WAITING) {
        printf("[ELF]:\t\tWAITING FOR ELVES TO RETURN, %d\n", id);
        pthread_cond_wait(&elf_wait_cond, &elf_wait_mutex);
    }
    pthread_mutex_unlock(&elf_wait_mutex);  
}

void elf_waiting_for_santa_in_queue(int id){
    check_if_deliveries_done();
    pthread_mutex_lock(&elf_mutex);
    if (elves_waiting < MAX_ELVES_WAITING){
        queue_to_santa[elves_waiting] = id;
        elves_waiting++;
        printf("[ELF]:\t\tWAITING %d ELVES FOR SANTA, %d\n", elves_waiting, id);

        if (elves_waiting == MAX_ELVES_WAITING){
            printf("[ELF]:\t\tWAKING UP SANTA, %d\n", id);
            pthread_mutex_lock(&santa_mutex);
            pthread_cond_broadcast(&santa_cond);
            pthread_mutex_unlock(&santa_mutex);
        }
    }
    pthread_mutex_unlock(&elf_mutex);
}

void elf_problem_solved_by_santa(int id){
    pthread_mutex_lock(&elf_wait_mutex);
    pthread_cond_wait(&elf_wait_cond, &elf_wait_mutex);
    check_if_deliveries_done();
    printf("[ELF]:\t\tSANTA SOLVED PROBLEM, %d\n", id);
    pthread_mutex_unlock(&elf_wait_mutex);  
}

void* elf_func(void* arg){
    int id = *((int *) arg);

    while(true){
        sleep(get_random_int(2,5));
        elf_waiting_for_others_to_return(id);
        elf_waiting_for_santa_in_queue(id);
        elf_problem_solved_by_santa(id);
    }
}

void* santa_func(void* arg){
    while (true){
        santa_wait_to_be_woken();
        santa_deliver_toys();
        check_if_deliveries_done();
        santa_solve_elves_problems();
    }
}

void* reindeer_func(void* arg){
    int id = *((int *) arg);
    while(true){
        reindeer_wait_to_go_holiday();
        sleep(get_random_int(5,10));
        reindeer_wait_for_others(id);
    }
}

void create_threads(struct santas_employee* table, int len, void* func){
    for (int i = 0; i < len; i++){
        table[i].id = i;
        pthread_create(&table[i].thread, NULL, func, &table[i].id);
    }
}

void wait_for_threads(){
    pthread_join(santa_thread, NULL);

    for (int i = 0; i < ELVES; i++)
        pthread_join(elves[i].thread, NULL);

    for (int i = 0; i < REINDEERS; i++)
        pthread_join(reindeers[i].thread, NULL);
}

int main(){
    srand(time(NULL));

    pthread_create(&santa_thread, NULL, &santa_func, NULL);

    create_threads(&reindeers[0], REINDEERS, reindeer_func);
    create_threads(&elves[0], ELVES, elf_func);

    wait_for_threads();

    return 0;
}