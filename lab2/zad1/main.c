#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

int MAX_LINE_LEN = 256;

int write_with_sys(char* to_copy_file, char* result_file){
    char* buffor = calloc(MAX_LINE_LEN, sizeof(char));
    int o, reading_file, writing_file;

    reading_file = open(to_copy_file, O_RDONLY);
    writing_file = open(result_file, O_WRONLY|O_CREAT);

    //do reading and writing


    // close here
    return 0;
}


int main(int arg_len, char **args){
    int i = 1; //first arg is './main'
    if (arg_len > 3){
        printf("ERROR: Too many arguments given. Only need file_to_copy and result_file! ");
        return -1;
    }
    
    char* to_copy_file = calloc(MAX_LINE_LEN, sizeof(char));
    char* result_file = calloc(MAX_LINE_LEN, sizeof(char));

    if (arg_len == 1) {
        printf("Please enter the to_copy_file name:\n");
        scanf("%s", to_copy_file);
    }
    else {
        to_copy_file = args[2];
    }

    if (arg_len < 3) {
        printf("Please enter the result_file name:\n");
        scanf("%s", result_file);
    }
    else {
        result_file = args[3];
    }

    printf("Loaded files are: to_copy_file -> %s and result_file -> %s\n", to_copy_file, result_file);

    //copy gowno by sys
    
    #ifdef LIB
        //copy gowno by lib
    #endif
    #ifndef LIB
        //copy gowno by sys
    #endif

}
