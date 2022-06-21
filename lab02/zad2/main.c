#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int count_chars(char given_char, char* file_name){
    char* buffor = calloc(1, sizeof(char));
    int out;

    #ifdef LIB
        FILE* reading_file = fopen(file_name, "r");
        out=fread(buffor, sizeof(char), 1, reading_file);
    #else
        int reading_file = open(file_name, O_RDONLY);
        out=read(reading_file, buffor, 1);
    #endif

    int rows_count=0, char_count=0, tmp_char_count;
    while(out != 0){
        tmp_char_count=0;
        while(*buffor != '\n' && out == 1){
            if (*buffor == given_char)
                tmp_char_count++;

            #ifdef LIB
                out = fread(buffor, sizeof(char), 1, reading_file);
            #else
                out = read(reading_file, buffor, 1);
            #endif
        }

        if (tmp_char_count > 0){
            char_count+=tmp_char_count;
            rows_count++;
        }

        #ifdef LIB
            out=fread(buffor, sizeof(char), 1, reading_file);
        #else
            out=read(reading_file, buffor, 1);
        #endif
    }

    #ifdef LIB
        fclose(reading_file);
    #else
        close(reading_file);
    #endif

    printf("CHARS: %d\tROWS: %d\n", char_count, rows_count);
    return 0;
}

int main(int arg_len, char **args){
    //first arg is './main'
    if (arg_len != 3){
        printf("ERROR: Must be given 2 arguments: char and file_name! ");
        return -1;
    }
    
    char given_char=args[1][0];
    char* file_name=args[2];

    printf("Loaded: given_char -> %c and file_name -> %s\n", given_char, file_name);
    count_chars(given_char, file_name);

    return 0;
}