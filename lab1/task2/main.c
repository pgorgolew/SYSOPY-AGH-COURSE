#include "library.h"
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

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
    int to_i = i+1;
    while(to_i < max_i){
        if (isInArr(args[to_i], funcs, 3))
            break;
        to_i++;
    }

    return to_i-(i+1);
}

char** create_arr_for_wc(int i, char** args, int files_count){
    char** file_names = calloc(files_count, sizeof(char*));

    for (int x=0; i<files_count; x++)
        file_names[x] = args[i+x];
    
    return file_names;
}

int main(int arg_len, char **args){
    int i = 1; //first arg is ./main

    if (arg_len < 1){
        printf("ERROR: No arguments given! ");
        return -1;
    }

    if (strcmp(args[i], "create_table") == 0){
        printf("ERROR: You have to create_table first! ");
        return -1;
    }

    char* funcs[3] = {"create_table", "wc_files", "remove_block"};
    while(i<arg_len){
        char* curr_func = args[i];
        
        if (i+1 == arg_len)
            break;

        char* curr_arg;
        i++;
        while(isInArr(curr_arg, funcs, 3) == 0){
            int add_to_i = 1;
            curr_arg  = args[i];
            if (strcmp(curr_func, "create_table") == 0)
                create_table(parse_str_to_uint(curr_arg));

            else if (strcmp(curr_func, "wc_files") == 0){
                int files_count = count_files_for_wc(funcs, i, arg_len, args);
                char** files_for_wc = create_arr_for_wc(i, args, files_count);
                add_to_i = files_count;
                wc_files(files_count, files_for_wc);
            }

            else if (strcmp(curr_func, "remove_block"))
                remove_block(parse_str_to_uint(curr_arg));
            
            i += add_to_i;
        }
    }

    return 1;
}

