#include "common.h"

int server_socket;
int is_client_O;
tic_tac_toe tic_tac_toe_board;
int current_state = START;
char buffer[MAX_MSG_LEN];
char *name, *command, *arg;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void draw(){
    char symbol;
    printf("-------------------------------------------------\n");
    for (int y = 0; y < 3; y++){
        printf("|\t \t|\t \t|\t \t|\n");
        for (int x = 0; x < 3; x++){
            if (tic_tac_toe_board.objects[y * 3 + x] == NONE) 
                symbol = y * 3 + x + 1 + '0';
            else if (tic_tac_toe_board.objects[y * 3 + x] == SIGN_O) 
                symbol = 'O';
            else 
                symbol = 'X';
            printf("|\t%c\t", symbol);
        }
        printf("|\n");
        printf("|\t \t|\t \t|\t \t|\n");
        printf("------------------------------------------------\n");
    }
}

void parse_command(char* msg){
    command = strtok(msg, "/");
    arg = strtok(NULL, "/");
}

tic_tac_toe create_board(){
    tic_tac_toe board = {.move = 1};
    for (int i=0; i<9; i++)
        board.objects[i] = NONE;

    return board;
}

void start() {
    if (strcmp(arg, "choosen_name_in_use") == 0){
        printf("ERROR: NAME CURRENTLY USED BY SOMEBODY ELSE");
        exit(-1);
    }
    else if (strcmp(arg, "wait_for_opponent") == 0) 
        current_state = WAIT_FOR_OPPONENT;
    else{
        tic_tac_toe_board = create_board();
        if (arg[0] == 'O')
            current_state = MOVE;
        else
            current_state = WAIT_FOR_MOVE;
        
        is_client_O = arg[0] == 'O';
    }
}

void sigint_handler(){
    sprintf(buffer, "end/NULL/%s", name);
    send(server_socket, buffer, MAX_MSG_LEN, 0);
    exit(0);
}

int check_column(tic_tac_toe* board) {
    int column = NONE;
    for (int x = 0; x < 3; x++){
        if (board->objects[x] == board->objects[x + 3] && board->objects[x] == board->objects[x + 6]
            && board->objects[x] != NONE)
            column = board->objects[x];
    }
    return column;
}

int check_row(tic_tac_toe* board) {
    int row = NONE;
    for (int y = 0; y < 3; y++){
        if (board->objects[3 * y] == board->objects[3 * y + 1] && board->objects[3 * y] == board->objects[3 * y + 2]
        && board->objects[3 * y] != NONE)
            row = board->objects[3 * y];
    }
    return row;
}

int check_diag(tic_tac_toe* board, int pos[]) {
    int diag = NONE;
    int first = board->objects[pos[0]];
    int second = board->objects[pos[1]];
    int third = board->objects[pos[2]];
    if (first == second && first == third && first != NONE) diag = first;
    return diag;
}

int check_winner(tic_tac_toe *board){
    if (check_column(board) != NONE)
        return check_column(board);
    if (check_row(board) != NONE)
        return check_row(board);

    int pos[] = {0, 4, 8};
    if (check_diag(board, pos) != NONE)
        return check_diag(board, pos);

    pos[0] = 2; pos[2] = 6;
    if (check_diag(board, pos) != NONE)
        return check_diag(board, pos);
    return NONE;
}

void check_game_status(){
    bool win = false;
    int winner = check_winner(&tic_tac_toe_board);

    if (winner != NONE){
        if ((is_client_O && winner == SIGN_O) || (!is_client_O && winner == SIGN_X)) 
            printf("You won!\n");
        else 
            printf("You lost!\n");
        win = true;
        current_state = QUIT;
        return;
    }
    
    bool draw = true;
    for (int i = 0; i < 9; i++){
        if (tic_tac_toe_board.objects[i] == NONE){
            draw = false;
            break;
        }
    }
    if (draw){
        printf("It's a draw!\n");
        current_state = QUIT;
    }
}

void wait_for_opponent() {
    pthread_mutex_lock(&mutex);
    while (current_state != START && current_state != QUIT) pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    tic_tac_toe_board = create_board();
    is_client_O = arg[0] == 'O';
    current_state = is_client_O ? MOVE : WAIT_FOR_MOVE;
}

bool move(tic_tac_toe *board, int position){
    if (position < 0 || position > 9 || board->objects[position] != NONE) 
        return false;

    board->objects[position] = board->move ? SIGN_O : SIGN_X;
    board->move = !board->move;
    return true;
}

void wait_for_move() {
    printf("Waiting for rivals move\n");
    pthread_mutex_lock(&mutex);
    while (current_state != OPPONENT_MOVE && current_state != QUIT)
        pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
}

void opponent_move() {
    int pos = atoi(arg);
    move(&tic_tac_toe_board, pos - 1);
    check_game_status();
    if (current_state != QUIT)
        current_state = MOVE;
}

void game_move() {
    draw();
    int pos;
    do{
        printf("Next move [YOUR SIGN %c] \n", is_client_O ? 'O' : 'X');
        scanf("%d", &pos);
        pos--;
    } while (!move(&tic_tac_toe_board, pos));
    draw();
    sprintf(buffer, "move/%d/%s", pos, name);
    send(server_socket, buffer, MAX_MSG_LEN, 0);
    check_game_status();
    if (current_state != QUIT)
        current_state = WAIT_FOR_MOVE;
}

void play_game(){
    while (true){
        if (current_state == START)
            start();
        else if (current_state == WAIT_FOR_OPPONENT)
            wait_for_opponent();
        else if (current_state == WAIT_FOR_MOVE)
            wait_for_move();
        else if (current_state == OPPONENT_MOVE)
            opponent_move();
        else if (current_state == MOVE)
            game_move();
        else if (current_state == QUIT)
            sigint_handler();
    }
}

void connect_locally(char* path){
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, path);
    connect(server_socket, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
}

void connect_by_network(char* port){
    struct addrinfo* info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    getaddrinfo("localhost", port, &hints, &info);
    server_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    connect(server_socket, info->ai_addr, info->ai_addrlen);
    freeaddrinfo(info);
}

void run_tic_tac_toe(bool game_thread_running) {
    current_state = START;
    if (!game_thread_running){
        pthread_t t;
        pthread_create(&t, NULL, (void *(*)(void *))play_game, NULL);
    }
}

void listen_server(){
    bool is_game_on = false;
    while (true)
    {
        recv(server_socket, buffer, MAX_MSG_LEN, 0);
        parse_command(buffer);

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0) {
            run_tic_tac_toe(is_game_on);
            is_game_on = true;
        }
        else if (strcmp(command, "move") == 0)
            current_state = OPPONENT_MOVE;
        else if (strcmp(command, "end") == 0){
            current_state = QUIT;
            exit(0);
        }
        else if (strcmp(command, "ping") == 0){
            sprintf(buffer, "pong/NULL/%s", name);
            send(server_socket, buffer, MAX_MSG_LEN, 0);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char* argv[]){
    if (argc != 4){
        printf("Command line arguments wrong. Must pass <name> <unix/inet> <path/port>\n");
        exit(1);
    }
    signal(SIGINT, sigint_handler);

    name = argv[1];
    char* socket = argv[3];
    if (strcmp(argv[2], "unix") == 0)
        connect_locally(socket);
    else if (strcmp(argv[2], "inet") == 0)
        connect_by_network(socket);

    char msg[MAX_MSG_LEN];
    sprintf(msg, "add/NULL/%s", name);
    send(server_socket, msg, MAX_MSG_LEN, 0);

    listen_server();
    return 0;
}