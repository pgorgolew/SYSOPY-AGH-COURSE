#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

int MAX_LINE_LEN = 256;

int write_with_sys(char* to_copy_file, char* result_file){
    char* buffor = calloc(MAX_LINE_LEN, sizeof(char));
    int out, reading_file, writing_file;

    reading_file = open(to_copy_file, O_RDONLY);
    writing_file = open(result_file, O_WRONLY|O_CREAT);

    //do reading and writing
    char c;
    while(out=read(reading_file, buffor, sizeof(buffor) > 0)){
        char* line_to_write = calloc(MAX_LINE_LEN+2, sizeof(char));
        int is_new_line, new_line_index = 0;
        for (int i=0; i<out; i++){
            if (is_new_line == 1 && buffor[i] == '\n'){
                line_to_write[i-new_line_index] = '\n';
                line_to_write[i-new_line_index+1] = '\0';

                write(writing_file, line_to_write, sizeof(line_to_write));
                new_line_index = i+1;
                is_new_line = 0;
            }
            else if (i-new_line_index == 0 && buffor[i] == '\n'){
                new_line_index = i+1;
            }
            else{
                line_to_write[i-new_line_index] = buffor[i];

                if (buffor[i] == '\n')
                    is_new_line = 1;
                else
                    is_new_line = 0;
            }
        }
    }

    // close here
    close(reading_file);
    close(writing_file);

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

    if (arg_len < 2) {
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
    
    #ifdef LIB
        write_with_sys(to_copy_file, result_file);
    #endif
    #ifndef LIB
        write_with_sys(to_copy_file, result_file);
    #endif

}
