#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/times.h>
#include <fcntl.h>
#include <ctype.h>

int BUFFER_SIZE = 516;

double time_in_s(clock_t clock_start, clock_t clock_end){
    return (double)(clock_end - clock_start) / sysconf(_SC_CLK_TCK);
}

double sum_results(int n){
    double sum = 0;
    char* file_name = calloc(BUFFER_SIZE, sizeof(char));
    char* result = calloc(BUFFER_SIZE, sizeof(char));
    char *ptr;
    double process_res;
    FILE* reading_file;
    for (int i=1; i<=n; i++){
        snprintf(file_name, BUFFER_SIZE, "w%d.txt", i);
        reading_file = fopen(file_name, "r");
        fread(result, sizeof(char), BUFFER_SIZE, reading_file);
        process_res = strtod(result, &ptr);
        fclose(reading_file);

        if (process_res == 0){
            printf("ERROR: Wrong number, pointer value: `%s`\n", ptr);
            return 1;
        }
        
        sum += process_res;
    }

    free(file_name);
    free(result);
    return sum;
}

double f(double x){
    return 4/(x*x + 1);
}

double center(double x1, double x2){
    return (x1+x2) / 2;
}

int calculate_and_save(double x1, double x2, int processNumber){
    char* buffer = calloc(BUFFER_SIZE, sizeof(char));
    double result = f(center(x1, x2)) * (x2 - x1);
    int snprintf_result = snprintf(buffer, BUFFER_SIZE, "echo %f > w%d.txt", result, processNumber);
    if (snprintf_result >= BUFFER_SIZE){
        printf("ERROR: Command is too long (max length is %d)\n", BUFFER_SIZE);
        free(buffer);
        return -1;
    }
    else if(snprintf_result < 0){
        printf("ERROR: Snprintf error\n");
        free(buffer);
        return -1;
    }
    else {
        system(buffer);
    }
    free(buffer);
    return 0;
}

char* get_time_result(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms){
    char* buffer = calloc(BUFFER_SIZE, sizeof(char));
    double real = time_in_s(clock_start, clock_end);
    double user = time_in_s(start_tms.tms_cutime, end_tms.tms_cutime);
    double sys = time_in_s(start_tms.tms_cstime, end_tms.tms_cstime);
    snprintf(buffer, BUFFER_SIZE, "real: %f, user: %f, sys: %f", real, user, sys);
    return buffer;
}


int main(int arg_len, char **args){
    struct tms start_main, end_main;
    clock_t c_start_main, c_end_main;
    c_start_main = times(&start_main);
    // liczba procesów to ilość prostokątów 
    if (arg_len == 1){
        printf("ERROR: you have to give number of processes\n");
        return 1;
    }

    char *ptr;
    int n = strtol(args[1], &ptr, 10);

    if (n == 0){
        printf("ERROR: Wrong number, pointer value: `%s`\n", ptr);
        return 1;
    }

    double* checkpoints = calloc(n+1, sizeof(double));
    checkpoints[0]=0;
    checkpoints[n]=1;
    double delta = (double) 1 / (double) n;
    for (int i=1; i<n; i++)
        checkpoints[i] = checkpoints[i-1] + delta;

    pid_t curr_pid;
    for (int i = 0; i<n; i++){
        curr_pid = fork();
        if (curr_pid == 0){
            calculate_and_save(checkpoints[i], checkpoints[i+1], i+1);
            return 0;
        }
    }

    free(checkpoints);

    for (int i=0; i<n; i++) 
        wait(NULL);

    double area = sum_results(n);
    c_end_main = times(&end_main);
    char* time_result = get_time_result(c_start_main, c_end_main, start_main, end_main);
    printf("PROCESSESS: %d\n", n);
    printf("AREA: %f\n", area);
    printf("TIME: %s\n", time_result);
    free(time_result);
    return 0;
}