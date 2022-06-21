#include "functions.h"

void sigint_handler(){
    sem_unlink_exec(OVEN_SEM);
    sem_unlink_exec(TABLE_SEM);
    sem_unlink_exec(FULL_OVEN_SEM);
    sem_unlink_exec(FULL_TABLE_SEM);
    sem_unlink_exec(EMPTY_TABLE_SEM);
    shm_unlink_exec(TABLE_PATH);
    shm_unlink_exec(OVEN_PATH);
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
    int shm_table_fd = shm_open_exec(TABLE_PATH, O_RDWR | O_CREAT, 0666);
    int shm_oven_fd = shm_open_exec(OVEN_PATH, O_RDWR | O_CREAT, 0666);

    ftruncate_exec(shm_table_fd, sizeof(table));
    ftruncate_exec(shm_oven_fd, sizeof(oven));

    oven* oven = mmap_exec(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, shm_oven_fd, 0);
    table* table = mmap_exec(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, shm_table_fd, 0);

    set_oven(oven);
    set_table(table);

    printf("INFO: Shared memory segment created\n");
}

void set_sem_values(){
    sem_open_exec(OVEN_SEM, O_CREAT, 0666, 1);
    sem_open_exec(TABLE_SEM, O_CREAT, 0666, 1);
    sem_open_exec(FULL_OVEN_SEM, O_CREAT, 0666, MAX_PIZZAS);
    sem_open_exec(FULL_TABLE_SEM, O_CREAT, 0666, MAX_PIZZAS);
    sem_open_exec(EMPTY_TABLE_SEM, O_CREAT, 0666, 0);
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