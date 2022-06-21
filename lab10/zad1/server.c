#include "common.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int socket_local;
int socket_network;
client* clients[MAX_CLIENTS];
int clients_cnt = 0;

int get_opponent(int idx){
    if (idx % 2 == 0)
        return idx+1;
        
    return idx-1;
}

int get_index_for_new_client() {
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] == NULL)
            return i;
    }
    return -1;
}

int get_idx_of_client_name(char* name){
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0) 
            return i;

    return -1;
}

void init_client(char* name, int fd, int index_for_new_client) {
    client *new_client = calloc(1, sizeof(client));
    new_client->name = calloc(MAX_MSG_LEN, sizeof(char));
    strcpy(new_client->name, name);
    new_client->fd = fd;
    new_client->available = true;

    clients[index_for_new_client] = new_client;
    clients_cnt++;
}

int add_client(char* name, int fd){
    if (get_idx_of_client_name(name) != -1)
            return -1;

    int client_idx = get_index_for_new_client();
    if (client_idx != -1)
        init_client(name, fd, client_idx);
    
    return client_idx;
}

void free_player(int index) {
    clients[index] = NULL;
    clients_cnt--;
}

void free_client(int index){
    free_player(index);
    int opponent = get_opponent(index);
    if (clients[get_opponent(index)] != NULL)
        free_player(opponent);
}

void remove_client_from_name(char* name){
    int index = get_idx_of_client_name(name);
    if (index != -1)
        free_client(index);
}

void delete_not_available_clients(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] && !(clients[i]->available))
            free_client(i);
    }
}

void ping_clients(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != NULL){
            send(clients[i]->fd, "ping/NULL", MAX_MSG_LEN, 0);
            clients[i]->available = false;
        }
    }
}

void* ping(){
    while (true){
        printf("[INFO] Server pinging\n");
        pthread_mutex_lock(&mutex);
        delete_not_available_clients();
        ping_clients();
        pthread_mutex_unlock(&mutex);
        sleep(5);
    }
}

void pong(char* name) {
    int player = get_idx_of_client_name(name);
    if (player != -1) 
        clients[player]->available = true;
}

struct sockaddr_un create_addr_un(char* socket_path){
    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, socket_path);
    return sock_addr;
}

struct sockaddr_in create_addr_in(char* port){
    struct sockaddr_in sock_addr;
    sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(atoi(port));
    return sock_addr;
}

int create_socket(int domain, int local, char* arg_to_addr_create){
    int sock_fd = socket(domain , SOCK_STREAM, 0);

    if (local==1){
        struct sockaddr_un sock_addr = create_addr_un(arg_to_addr_create);
        bind(sock_fd, (struct sockaddr*) &sock_addr, sizeof(sock_addr));
    }
    else{
        struct sockaddr_in sock_addr = create_addr_in(arg_to_addr_create);
        bind(sock_fd, (struct sockaddr*) &sock_addr, sizeof(sock_addr));
    }

    listen(sock_fd, MAX_CLIENTS);
    return sock_fd;
}

void create_local_and_network_socket(char* socket_path, char* port){
    socket_local = create_socket(AF_UNIX, 1, socket_path);
    socket_network = create_socket(AF_INET, 0, port);
}

struct pollfd* set_fds(int local_socket, int network_socket) {
    struct pollfd *fds = calloc(2 + clients_cnt, sizeof(struct pollfd));
    fds[0].fd = local_socket;
    fds[0].events = POLLIN;
    fds[1].fd = network_socket;
    fds[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clients_cnt; i++){
        fds[i + 2].fd = clients[i]->fd;
        fds[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);

    poll(fds, clients_cnt + 2, -1);
    return fds;
}

int check_messages(){
    struct pollfd *fds = set_fds(socket_local, socket_network);
    int retval = -1;
    for (int i = 0; i < clients_cnt + 2; i++){
        if (fds[i].revents & POLLIN){
            retval = fds[i].fd;
            break;
        }
    }
    if (retval == socket_local || retval == socket_network){
        retval = accept(retval, NULL, NULL);
    }
    free(fds);
    return retval;
}

void add(char* name, int client_fd) {
    int index = add_client(name, client_fd);

    if (index == -1){
        send(client_fd, "add/choosen_name_in_use", MAX_MSG_LEN, 0);
        close(client_fd);
    }
    else if (index % 2 == 0)
        send(client_fd, "add/wait_for_opponent", MAX_MSG_LEN, 0);
    else{
        int random_num = rand() % 100;
        int first, second;
        if (random_num % 2 == 0){
            first = index;
            second = get_opponent(index);
        }
        else{
            first = get_opponent(index);
            second = index;
        }

        send(clients[first]->fd, "add/O", MAX_MSG_LEN, 0);
        send(clients[second]->fd, "add/X", MAX_MSG_LEN, 0);
    }
}

void move(char* arg, char* name, char buffer[]) {
    int move = atoi(arg) + 1;
    int player_index = get_idx_of_client_name(name);
    sprintf(buffer, "move/%d", move);
    send(clients[get_opponent(player_index)]->fd, buffer, MAX_MSG_LEN, 0);
}

void client_routine() {
    while (true){
        int client_fd = check_messages();
        char buffer[MAX_MSG_LEN];
        recv(client_fd, buffer, MAX_MSG_LEN, 0);

        printf("%s\n", buffer);
        char *command = strtok(buffer, "/");
        char *arg = strtok(NULL, "/");
        char *name = strtok(NULL, "/");

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
            add(name, client_fd);
        else if (strcmp(command, "move") == 0)
            move(arg, name, buffer);
        else if (strcmp(command, "end") == 0)
            remove_client_from_name(name);
        else if (strcmp(command, "pong") == 0)
            pong(name);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("Command line arguments wrong. Must pass <port> <path>\n");
        exit(1);
    }
    srand(time(NULL));

    char *port = argv[1];
    char *socket_path = argv[2];
    create_local_and_network_socket(socket_path, port);

    pthread_t t;
    pthread_create(&t, NULL, &ping, NULL);

    client_routine();
}