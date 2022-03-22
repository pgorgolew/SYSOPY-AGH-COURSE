#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int write_without_black(char* to_copy_file, char* result_file){
    char* buffor = calloc(1, sizeof(char));
    int out;

    #ifdef LIB
        FILE* reading_file = fopen(to_copy_file, "r");
        FILE* writing_file = fopen(result_file, "w+");
        out=fread(buffor, sizeof(char), 1, reading_file);
    #else
        int reading_file = open(to_copy_file, O_RDONLY);
        int writing_file = open(result_file, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
        out=read(reading_file, buffor, 1);
    #endif

    long int start_line;
    int spaces_count, char_count;
    while(out != 0){
        #ifdef LIB
            start_line = ftell(reading_file);
        #else
            start_line = lseek(reading_file, 0, SEEK_CUR);
        #endif

        spaces_count = char_count = 0;
        while(*buffor != '\n' && out == 1){
            if (isspace(*buffor))
                spaces_count++;
            else
                char_count++;

            #ifdef LIB
                out = fread(buffor, sizeof(char), 1, reading_file);
            #else
                out = read(reading_file, buffor, 1);
            #endif
        }

        if (char_count != 0){
            #ifdef LIB
                fseek(reading_file, start_line-1, 0);
            #else
                lseek(reading_file, start_line-1, SEEK_SET);
            #endif

            for (int z=0; z<spaces_count+char_count; z++){
                #ifdef LIB
                    fread(buffor, sizeof(char), 1, reading_file);
                    fwrite(buffor, 1, 1, writing_file);
                #else
                    read(reading_file, buffor, 1);
                    write(writing_file, buffor, 1);
                #endif
            }

            #ifdef LIB
                fwrite("\n", 1, 1, writing_file);
                fread(buffor, sizeof(char), 1, reading_file); //just to move after \n
                out=fread(buffor, sizeof(char), 1, reading_file);
            #else
                write(writing_file, "\n", 1);
                read(reading_file, buffor, 1); //just to move after \n
                out=read(reading_file, buffor, 1);
            #endif
        }
        else{
            #ifdef LIB
                fseek(reading_file, start_line+spaces_count, 0);
                out=fread(buffor, sizeof(char), 1, reading_file);
            #else
                lseek(reading_file, start_line+spaces_count, SEEK_SET);
                out=read(reading_file, buffor, 1);
            #endif
        }
    }

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
        to_copy_file = calloc(256, sizeof(char));
        printf("Please enter the to_copy_file name:\n");
        scanf("%s", to_copy_file);
    }
    else
        to_copy_file = args[1];

    if (arg_len < 3) {
        result_file = calloc(256, sizeof(char));
        printf("Please enter the result_file name:\n");
        scanf("%s", result_file);
    }
    else 
        result_file = args[2];

    printf("Loaded files are: to_copy_file -> %s and result_file -> %s\n", to_copy_file, result_file);
    write_without_black(to_copy_file, result_file);

    return 0;
}