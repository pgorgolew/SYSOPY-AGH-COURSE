#include "functions.h"

void sigint_handler(){
    exit(0);
}

void shutdown_handler(){
    printf("INFO: Cook is going down\n");
}

void put_in_oven(oven* oven, int type){
    oven->pizza_count++;
    oven->array[oven->id_to_add_pizza] = type;
    oven->id_to_add_pizza = (oven->id_to_add_pizza + 1) % MAX_PIZZAS;
}

int take_out_from_oven(oven* oven){
    int type = oven->array[oven->id_to_take_out_pizza];
    oven->pizza_count--;
    oven->array[oven->id_to_take_out_pizza] = -1;
    oven->id_to_take_out_pizza = (oven->id_to_take_out_pizza + 1) % MAX_PIZZAS;
    return type;
}

void put_on_table(table* table, int type){
    table->pizza_count++;
    table->array[table->id_to_add_pizza] = type;
    table->id_to_add_pizza = (table->id_to_add_pizza + 1) % MAX_PIZZAS;
}

void work(oven* oven, table* table, sem_t* oven_sem, sem_t* table_sem, sem_t* full_oven_sem, sem_t* full_table_sem, sem_t* empty_table_sem){
    int pizza_type;
    srand(getpid()); //getpid to have different pizzas for different cooks
    while(true){
        pizza_type = rand() % 10;
        int preparation_time = PREPARATION_TIME1;
        int cooking_time = COOKING_TIME1;
        if (pizza_type > 5){ //means pizza is more complicated
            preparation_time = PREPARATION_TIME2;
            cooking_time = COOKING_TIME2;
        }

        printf("[COOK]\t\t pid: %d time: %s\tinfo: Przygotowuje pizze: %d\n", getpid(), get_current_time(), pizza_type);
        sleep(preparation_time);

        change_sem(full_oven_sem, sem_wait); // decrement value of full_oven to block baking if value is 0  

        change_sem(oven_sem, &sem_wait);
        put_in_oven(oven, pizza_type);
        printf("[COOK]\t\t pid: %d time: %s\tinfo: Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), get_current_time(), pizza_type, oven->pizza_count);
        change_sem(oven_sem, &sem_post);
        sleep(cooking_time);

        change_sem(oven_sem, &sem_wait);
        pizza_type = take_out_from_oven(oven);
        printf("[COOK]\t\t pid: %d time: %s\tinfo: Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), get_current_time(), pizza_type, oven->pizza_count, table->pizza_count);
        change_sem(oven_sem, &sem_post);
        change_sem(full_oven_sem, &sem_post);

        change_sem(full_table_sem, &sem_wait); // decrement to block placing pizza if table is full (when full table is 0 it blocks)

        change_sem(table_sem, &sem_wait);
        put_on_table(table, pizza_type);
        printf("[COOK]\t\t pid: %d time: %s\tinfo: Umieszczam pizze na stole: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), get_current_time(), pizza_type, oven->pizza_count, table->pizza_count);
        change_sem(table_sem, &sem_post);

        change_sem(empty_table_sem, &sem_post); //increment empty table to enable supplier to get pizza from table
    }
}

int main(){
    atexit(shutdown_handler);
    signal(SIGINT, sigint_handler);

    sem_t* table_sem = sem_open_exec(TABLE_SEM, O_RDWR, 0, 1);
    sem_t* oven_sem = sem_open_exec(OVEN_SEM, O_RDWR, 0, 1);
    sem_t* full_table_sem = sem_open_exec(FULL_TABLE_SEM, O_RDWR, 0, MAX_PIZZAS);
    sem_t* empty_table_sem = sem_open_exec(EMPTY_TABLE_SEM, O_RDWR, 0, 0);
    sem_t* full_oven_sem = sem_open_exec(FULL_OVEN_SEM ,O_RDWR, 0, MAX_PIZZAS);

    oven* oven = mmap_exec(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, shm_open_exec(OVEN_PATH, O_RDWR, 0666), 0);
    table* table = mmap_exec(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_open_exec(TABLE_PATH, O_RDWR, 0666), 0);

    work(oven, table, oven_sem, table_sem, full_oven_sem, full_table_sem, empty_table_sem);
}