#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define BUFF 256

int width, height, max_pixel_val, threads_num;
char* method;
FILE* result_file;
int** pgm;
int** negative_pgm;
long unsigned int *times;

void header_log() {
    printf("EXECUTION INFO:\n");
    printf("\tThreads number:\t%d\n", threads_num);
    printf("\tMethod:\t\t%s\n", method);
    printf("TIMES INFO:\n");

    fprintf(result_file, "EXECUTION INFO:\n");
    fprintf(result_file, "\tNumber of threads:\t%d\n", threads_num);
    fprintf(result_file, "\tMethod:\t\t%s\n", method);
    fprintf(result_file, "TIMES INFO:\n");
}

void total_time_log(struct timeval start, struct timeval stop) {
    gettimeofday(&stop, NULL);
    long unsigned int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    printf("\tTotal time: %5lu [μs]\n", time);
    fprintf(result_file, "\tTotal time: %5lu [μs]\n\n\n", time);
}

void* numbers_method(void* arg){
    int index = *((int*) arg);
    struct timeval stop, start;
    
    int start_color = (max_pixel_val + 1) / threads_num * index;

    int end_color;
    if (index == threads_num - 1) 
        end_color = max_pixel_val + 1;
    else
        end_color = (max_pixel_val + 1) / threads_num * (index + 1);

    gettimeofday(&start, NULL);

    for (int h = 0; h < height; h++){
        for (int w = index; w < width; w++){
            if (start_color <= pgm[h][w] && pgm[h][w] < end_color){
                negative_pgm[h][w] = max_pixel_val - pgm[h][w];
            }
        }
    }

    gettimeofday(&stop, NULL);
    times[index] = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(&times[index]);
}

void* block_method(void* arg) {
    int index = *((int *) arg);
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    int width_start = index * ceil(width / threads_num);
    int width_end = ((index + 1)* ceil(width / threads_num) - 1);
    if (index == threads_num - 1) width_end = width - 1;

    for (int i = 0; i < height; i++){
        for (int j = width_start; j <= width_end; j++)
            negative_pgm[i][j] = max_pixel_val - pgm[i][j];
    }

    gettimeofday(&stop, NULL);

    times[index]= (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(&times[index]);
}

int** init_array2d(int num1, int size1, int num2, int size2) {
    int** array2d = (int **) calloc(num1, size1);
    for (int h = 0; h < height; h++)
        array2d[h] = calloc(num2, size2);
    
    return array2d;
}

void load_pgm(char* filename){
    FILE* file = fopen(filename, "r");
    char* buffer = calloc(BUFF, sizeof(char));

    for (int i = 0; i < 3; i++)  // skipping unnecessary lines
        fgets(buffer, BUFF, file);
    
    sscanf(buffer, "%d %d\n", &width, &height);
    fgets(buffer, BUFF, file);
    sscanf(buffer, "%d\n", &max_pixel_val);
    
    pgm = init_array2d(height, sizeof(int*), width, sizeof(int));
    for (int h = 0; h < height; h++){
        for (int w = 0; w < width; w++){
            fscanf(file, "%d", &pgm[h][w]);
        }
    }

    fclose(file);
    free(buffer);
}

void save_negative_pgm(char* filename) {
    FILE *file = fopen(filename, "w");

    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", max_pixel_val);
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++)
            fprintf(file, "%d ", negative_pgm[h][w]);
        fprintf(file, "\n");
    }

    fclose(file);
}

pthread_t* init_threads() {
    pthread_t* threads = calloc(threads_num, sizeof(pthread_t));
    int* threads_indexes = calloc(threads_num, sizeof(int));

    for(int i = 0; i < threads_num; i++){
        threads_indexes[i] = i;
        if (strcmp(method, "block") == 0)
            pthread_create(&threads[i], NULL, &block_method, &threads_indexes[i]);
        else
            pthread_create(&threads[i], NULL, &numbers_method, &threads_indexes[i]);    
    }
    return threads;
}

void wait_for_threads(pthread_t* threads) {
    long unsigned int *time;
    for(int i = 0; i < threads_num; i++) {
        pthread_join(threads[i], (void**) &time);
        printf("\tThread index: %3d\t\tTime: %5lu [μs]\n", i, *time);
        fprintf(result_file, "\tThread index: %3d\t\tTime: %5lu [μs]\n", i, *time);
    }
}

int main(int argc, char* argv[]){
    if (argc != 5){
        printf("ERROR: need to be run with arguments: '<threads_num> <method> <input_pgm> <output pgm>'\n");
        return 1;
    }

    if ((threads_num = atoi(argv[1])) == 0){
        printf("ERROR: Threads num must be a positive int! \n");
        return 1;
    }

    method = argv[2];
    if (strcmp(method, "numbers") != 0 && strcmp(method, "block") != 0){
        printf("ERROR: method must be 'numbers' or 'block' \n");
        return 1;
    }
    
    char* pgm_filename = argv[3];
    char* pgm_out_filename = argv[4];
    struct timeval stop, start;

    times = calloc(threads_num, sizeof(long unsigned int));
    load_pgm(pgm_filename);
    negative_pgm = init_array2d(height, sizeof(int*), width, sizeof(int));
    result_file = fopen("Times.txt", "a");

    header_log();
    gettimeofday(&start, NULL);
    pthread_t* threads = init_threads();
    wait_for_threads(threads);
    total_time_log(start, stop);
    save_negative_pgm(pgm_out_filename);

    fclose(result_file);
    free(times);
    free(pgm);
    free(negative_pgm);
    return 0;
}