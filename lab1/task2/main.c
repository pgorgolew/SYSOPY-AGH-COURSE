#include "library.h"
#include <sys/times.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
int isInArr(char* value, char** arr, int arr_len){
    for (int i=0; i<arr_len; i++){
        if (strcmp(value, arr[i]) == 0)
            return 1;
    }

    return 0;
}

uint32_t parse_str_to_uint(char* value){
    int arr_len_int = atoi(value);
    return (uint32_t) arr_len_int;
}

int count_files_for_wc(char** funcs, int i, int max_i, char** args){
    int to_i = i;
    while(to_i < max_i){
        if (isInArr(args[to_i], funcs, 3))
            break;
        to_i++;
    }

    return to_i - i;
}

char** create_arr_for_wc(int i, char** args, int files_count){
    char** file_names = calloc(files_count, sizeof(char*));

    for (int z=0; z<files_count; z++)
        file_names[z] = args[i+z];

    
    return file_names;
}


double time_in_s(clock_t clock_start, clock_t clock_end){
    return (double)(clock_end - clock_start) / sysconf(_SC_CLK_TCK);
}

void print_times(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms){
    printf(
        "real: %f, user: %f, sys: %f\n", time_in_s(clock_start, clock_end), 
        time_in_s(start_tms.tms_utime, end_tms.tms_utime), time_in_s(start_tms.tms_stime, end_tms.tms_stime)
    );
}

int main(int arg_len, char **args){
    struct tms start_tms;
    struct tms end_tms;
    clock_t clock_start;
    clock_t clock_end;

    int i = 1; //first arg is ./main

    if (arg_len < 1){
        printf("ERROR: No arguments given! ");
        return -1;
    }

    if (strcmp(args[i], "create_table") != 0){
        printf("ERROR: You have to create_table first! ");
        return -1;
    }

    char** funcs = calloc(3, sizeof(char*));
    funcs[0] = "create_table";
    funcs[1] = "wc_files";
    funcs[2] = "remove_block";

    char* curr_func;
    char* curr_arg;
    while(i<arg_len){
        clock_start = times(&start_tms);
        curr_func = args[i];

        i++;
        if (i == arg_len)
            break;
        
        curr_arg = args[i];
        while(isInArr(curr_arg, funcs, 3) == 0){
            int add_to_i = 1;
            if (strcmp(curr_func, "create_table") == 0)
                create_table(parse_str_to_uint(curr_arg));

            else if (strcmp(curr_func, "wc_files") == 0){
                int files_count = count_files_for_wc(funcs, i, arg_len, args);
                char** files_for_wc = create_arr_for_wc(i, args, files_count);
                add_to_i = files_count;
                printf("%d %s\n", files_count, files_for_wc[0] );
                wc_files(files_count, files_for_wc);
            }

            else if (strcmp(curr_func, "remove_block"))
                remove_block(parse_str_to_uint(curr_arg));
            
            i += add_to_i;
            if (i>=arg_len)
                break;

            curr_arg  = args[i];
        }
        clock_end = times(&end_tms);
        print_times(clock_start, clock_end, start_tms, end_tms);
    }
    clock_end = times(&end_tms);
    print_times(clock_start, clock_end, start_tms, end_tms);
    
    return 1;
}

