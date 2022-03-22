#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

int MAX_LINE_LEN = 256;

int write_without_black(char* to_copy_file, char* result_file){
    char* buffor = calloc(MAX_LINE_LEN, sizeof(char));
    int out;

    #ifdef LIB
        FILE* reading_file = fopen(to_copy_file, "r");
        FILE* writing_file = fopen(result_file, "w+");
        out=fread(buffor, sizeof(char), MAX_LINE_LEN, reading_file);
    #else
        int reading_file = open(to_copy_file, O_RDONLY);
        int writing_file = open(result_file, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
        out=read(reading_file, buffor, sizeof(buffor));
    #endif


    //do reading and writing
    int is_new_line = 0;
    while(out > 0){
        char* line_to_write = calloc(MAX_LINE_LEN+2, sizeof(char));
        int new_line_index = 0;
        for (int i=0; i<out; i++){
            if (i-new_line_index == 0 && buffor[i] == '\n'){
                new_line_index = i+1;
            }
            else if (is_new_line == 1 && buffor[i] == '\n'){
                line_to_write[i-new_line_index+1] = '\0';

                #ifdef LIB
                    fwrite(line_to_write, sizeof(char), i-new_line_index+1, writing_file);
                #else
                    write(writing_file, line_to_write, i-new_line_index+1);
                #endif
                
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
        printf("%s", line_to_write);
        line_to_write[(out-1)-new_line_index+1] = '\n';

        #ifdef LIB
            fwrite(line_to_write, sizeof(char), out-new_line_index, writing_file);
            out=fread(buffor, sizeof(char), MAX_LINE_LEN, reading_file);
        #else
            write(writing_file, line_to_write, out-new_line_index);
            out=read(reading_file, buffor, sizeof(buffor));
        #endif

        free(line_to_write);
    }
    free(buffor);

    #ifdef LIB
        fclose(reading_file);
        fclose(writing_file);
    #else
        close(reading_file);
        close(writing_file);
    #endif

    return 0;
}

int main(int arg_len, char **args){
    //first arg is './main'
    if (arg_len > 3){
        printf("ERROR: Too many arguments given. Only need file_to_copy and result_file! ");
        return -1;
    }
    
    char* to_copy_file;
    char* result_file;

    if (arg_len < 2) {
        to_copy_file = calloc(MAX_LINE_LEN, sizeof(char));
        printf("Please enter the to_copy_file name:\n");
        scanf("%s", to_copy_file);
    }
    else
        to_copy_file = args[1];

    if (arg_len < 3) {
        result_file = calloc(MAX_LINE_LEN, sizeof(char));
        printf("Please enter the result_file name:\n");
        scanf("%s", result_file);
    }
    else 
        result_file = args[2];

    printf("Loaded files are: to_copy_file -> %s and result_file -> %s\n", to_copy_file, result_file);
    write_without_black(to_copy_file, result_file);

    return 0;
}