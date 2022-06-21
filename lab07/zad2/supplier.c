#include "functions.h"

void sigint_handler(){
    exit(0);
}

void shutdown_handler(){
    printf("INFO: Supplier is going down\n");
}

int take_pizza(table* table){
    int type = table->array[table->id_to_deliver_pizza];
    table->array[table->id_to_deliver_pizza] = -1;
    table->id_to_deliver_pizza = (table->id_to_deliver_pizza + 1) % MAX_PIZZAS;
    table->pizza_count--;
    return type;
}

void work(table* table, sem_t* table_sem, sem_t* full_table_sem, sem_t* empty_table_sem){
    int pizza_type;
    while(true) {
        change_sem(empty_table_sem, sem_wait); // decrement value before taking pizza from table to block if table is empty (for 0 it blocks)

        change_sem(table_sem, sem_wait);
        pizza_type = take_pizza(table);
        printf("[SUPPLIER]\t pid: %d time: %s\tinfo: Pobieram pizze: %d. Liczba pizz na stole: %d.\n", getpid(), get_current_time(), pizza_type, table->pizza_count);
        change_sem(table_sem, sem_post);

        change_sem(full_table_sem, sem_post); // increment value of full_table just to enable cook to place new pizza (for 0 it blocks)

        sleep(DELIVERY_TIME);
        printf("[SUPPLIER]\t pid: %d time: %s\tinfo: Dostarczam pizze: %d.\n", getpid(), get_current_time(), pizza_type);
        sleep(DELIVERY_TIME);
    }
}

int main(){
    atexit(shutdown_handler);
    signal(SIGINT, sigint_handler);

    sem_t* table_sem = sem_open_exec(TABLE_SEM, O_RDWR, 0, 1);
    sem_t* full_table_sem = sem_open_exec(FULL_TABLE_SEM, O_RDWR, 0, MAX_PIZZAS);
    sem_t* empty_table_sem = sem_open_exec(EMPTY_TABLE_SEM, O_RDWR, 0, 0);

    table* table = mmap_exec(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_open_exec(TABLE_PATH, O_RDWR, 0666), 0);

    work(table, table_sem, full_table_sem, empty_table_sem);
}