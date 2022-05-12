#include "functions.h"

int shm_oven_id, shm_table_id, sem_id;

void sigint_handler(int signum){
    set_sem_value(sem_id, 0, IPC_RMID, 0);
    shmctl_exec(shm_oven_id, IPC_RMID, NULL);
    shmctl_exec(shm_table_id, IPC_RMID, NULL);
}

void set_oven(oven* oven){
    for (int i = 0; i < MAX_PIZZAS; i++)
        oven->array[i] = -1;
    
    oven->pizza_count = 0;
    oven->id_to_add_pizza = 0;
    oven->id_to_take_out_pizza = 0;
}

void set_table(table* table){
    for (int i = 0; i < MAX_PIZZAS; i++)
        table->array[i] = -1;
        
    table->pizza_count = 0;
    table->id_to_add_pizza = 0;
    table->id_to_deliver_pizza = 0;
}

void create_shared_memory(){
    key_t oven_key = get_key(OVEN_PATH, OVEN_ID);
    key_t table_key = get_key(TABLE_PATH, TABLE_ID);

    shm_oven_id = shmget_exec(oven_key, sizeof(oven), IPC_CREAT | 0666);
    shm_table_id = shmget_exec(table_key, sizeof(table), IPC_CREAT | 0666);

    oven* oven = shmat(shm_oven_id, NULL, 0);
    table* table = shmat(shm_table_id, NULL, 0);

    set_oven(oven);
    set_table(table);

    printf("INFO: Shared memory segment created\n");
}

void set_sem_values(){
    key_t key = get_key(get_home_path(), SEM_ID);
    sem_id = semget_exec(key, 5, IPC_CREAT | 0666);

    set_sem_value(sem_id, OVEN_SEM, SETVAL, 1); // 1 to enable only one cook to do something (when cook do sth, he sets it to 0)
    set_sem_value(sem_id, TABLE_SEM, SETVAL, 1); // 1 to enable only one worker to do something (when worker do sth, he sets it to 0)
    set_sem_value(sem_id, FULL_OVEN_SEM, SETVAL, MAX_PIZZAS); // if oven is full (val is 0), block cooks to place new pizzas
    set_sem_value(sem_id, FULL_TABLE_SEM, SETVAL, MAX_PIZZAS); // if table is full (val is 0), block cooks to take out pizza
    set_sem_value(sem_id, EMPTY_TABLE_SEM, SETVAL, 0); // if table is empty (val is 0), block suppliers to get and deliver pizza
}

void start_workers(int amount_of_cooks, int amount_of_suppliers){
    for (int i = 0; i < amount_of_cooks; i++){
        pid_t pid = fork();
        if (pid == 0)
            execl("./cook", "./cook", NULL);
    }

    for (int i = 0; i < amount_of_suppliers; i++){
        pid_t pid = fork();
        if (pid == 0)
            execl("./supplier", "./supplier", NULL);
    }

}

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("ERROR: need to be run with arguments: '<cooks_number> <suppliers_number>'\n");
        return 1;
    }

    int amount_of_cooks = atoi(argv[1]);
    int amount_of_suppliers = atoi(argv[2]);

    if (amount_of_cooks <= 0 || amount_of_suppliers <= 0){
        printf("ERROR: arguments must be positive!\n");
        return 1;
    }

    signal(SIGINT, sigint_handler);
    create_shared_memory();
    set_sem_values();
    start_workers(amount_of_cooks, amount_of_suppliers);

    for(int i = 0; i < amount_of_cooks + amount_of_suppliers; i++)
        wait(NULL);

    return 0;
}