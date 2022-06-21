#include <sys/times.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#ifdef DDL
    #include <dlfcn.h>
#endif
#ifndef DDL
    #include "library.h"
#endif

struct func_time{
    char* func;
    char* time;
};


int isInArr(char* value, char** arr, int arr_len){
    for (int i=0; i<arr_len; i++){
        if (strcmp(value, arr[i]) == 0)
            return 1;
    }

    return 0;
}

u_int32_t parse_str_to_uint(char* value){
    return (u_int32_t) atoi(value);
}

int count_files_for_wc(char** funcs, int i, int max_i, char** args){
    int to_i = i;
    while(to_i < max_i){
        if (isInArr(args[to_i], funcs, 4))
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

struct func_time get_time_result(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms, char* func_name){
    char* buffer = calloc(512, sizeof(char));
    double real = time_in_s(clock_start, clock_end);
    double user = time_in_s(start_tms.tms_cutime, end_tms.tms_cutime);
    double sys = time_in_s(start_tms.tms_cstime, end_tms.tms_cstime);
    snprintf(buffer, 512, "real: %f, user: %f, sys: %f", real, user, sys);
    struct func_time curr_time_result = {func_name, buffer};
    return curr_time_result;
}

void print_times(struct func_time* results, int results_len){
    for (int i=0; i<results_len; i++){
        printf("%s %s \n", results[i].func, results[i].time);
    }
}

int main(int arg_len, char **args){
    #ifdef DDL
        void *handle = dlopen("./liblib_wc.so", RTLD_LAZY);
        if(!handle){/*error*/}
        int (*load_to_memory)(char*) = dlsym(handle, "load_to_memory");
        void (*create_table)(u_int32_t) = dlsym(handle, "create_table");
        int (*wc_files)(int, char**) = dlsym(handle, "wc_files");
        void (*remove_block)(u_int32_t) = dlsym(handle, "remove_block"); 
    #endif

    struct tms start_tms, end_tms, start_main, end_main;
    clock_t clock_start, clock_end, c_start_main, c_end_main;
    c_start_main = times(&start_main);
    
    int i = 1; //first arg is './main'
    if (arg_len < 2){
        printf("ERROR: No arguments given! ");
        return -1;
    }

    if (strcmp(args[1], "create_table") != 0){
        printf("ERROR: You have to create_table first! ");
        return -1;
    }

    char** funcs = calloc(4, sizeof(char*));
    funcs[0] = "create_table";
    funcs[1] = "wc_files";
    funcs[2] = "remove_block";
    funcs[3] = "load_to_memory";

    char* curr_func;
    char* curr_arg;

    int funcs_count = 1; //cause one is main func
    for (int j=1; j<arg_len; j++){
        if (isInArr(args[j], funcs, 4) == 1)
            funcs_count++;
    }
    struct func_time* time_results = calloc(funcs_count, sizeof(struct func_time));
    int t = 0;
    while(i<arg_len){
        curr_func = args[i];
        i++;
        if (i == arg_len)
            break;
        
        curr_arg = args[i];
        int add_to_i = 1;

        if (strcmp(curr_func, "create_table") == 0){
            clock_start = times(&start_tms);
            create_table(parse_str_to_uint(curr_arg));
        }

        else if (strcmp(curr_func, "wc_files") == 0){
            int files_count = count_files_for_wc(funcs, i, arg_len, args);
            char** files_for_wc = create_arr_for_wc(i, args, files_count);
            add_to_i = files_count;
            clock_start = times(&start_tms);
            wc_files(files_count, files_for_wc);
        }

        else if (strcmp(curr_func, "load_to_memory") == 0){
            clock_start = times(&start_tms);
            load_to_memory(curr_arg);
        }
        
        else if (strcmp(curr_func, "remove_block") == 0){
            clock_start = times(&start_tms);
            remove_block(parse_str_to_uint(curr_arg));
        }
        else {
            printf("ERROR: not defined function");
            return -1;
        }
        clock_end = times(&end_tms);
        i += add_to_i;
        curr_arg  = args[i];
        
        struct func_time curr_time_result = get_time_result(clock_start, clock_end, start_tms, end_tms, curr_func);
        time_results[t] = curr_time_result;
        t++;
    }
    c_end_main = times(&end_main);
    struct func_time curr_time_result = get_time_result(c_start_main, c_end_main, start_main, end_main, "main");
    time_results[t] = curr_time_result;
    print_times(time_results, funcs_count);

    free(funcs);
    free(time_results);

    #ifdef DDL
        dlclose(handle);
    #endif

    return 0;
}
