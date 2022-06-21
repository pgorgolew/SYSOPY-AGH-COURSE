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

void work(int sem_id, table* table){
    int pizza_type;
    while(true) {
        change_sem(sem_id, EMPTY_TABLE_SEM, -1); // decrement value before taking pizza from table to block if table is empty (for 0 it blocks)

        change_sem(sem_id, TABLE_SEM, -1);
        pizza_type = take_pizza(table);
        printf("[SUPPLIER]\t pid: %d time: %s\tinfo: Pobieram pizze: %d. Liczba pizz na stole: %d.\n", getpid(), get_current_time(), pizza_type, table->pizza_count);
        change_sem(sem_id, TABLE_SEM, 1);

        change_sem(sem_id, FULL_TABLE_SEM, 1); // increment value of full_table just to enable cook to place new pizza (for 0 it blocks)

        sleep(DELIVERY_TIME);
        printf("[SUPPLIER]\t pid: %d time: %s\tinfo: Dostarczam pizze: %d.\n", getpid(), get_current_time(), pizza_type);
        sleep(DELIVERY_TIME);
    }
}
int main(){
    atexit(shutdown_handler);
    signal(SIGINT, sigint_handler);

    int sem_id = get_sem_id();
    int shm_table_id = get_shm_table_id();

    table* table = shmat(shm_table_id, NULL, 0);

    work(sem_id, table);
}