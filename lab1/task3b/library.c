#include "library.h"
#include <stdlib.h>

int BUFFER_SIZE = 512;
char* TMP_FILE = "file.tmp";

uint32_t ARR_LEN;
char** ARR; 

void create_table(uint32_t array_len){
    if (ARR != NULL) {
        printf("Old array lenght %d new array length %d\n", ARR_LEN, array_len);
        free(ARR);
    }
    else {
        printf("Alocating array of length %d\n", array_len);
    }

    ARR_LEN = array_len;
    ARR = calloc(array_len, sizeof(char*));
}

long get_file_size(FILE* file){
    long size;
    fseek(file, 0L, SEEK_END); //goes to the end of file
    size = ftell(file); //return size (cause we are at the end of file)
    rewind(file); //goes back to the begging of file
    return size;
}

int load_to_memory(char* file_name){
    FILE* file_ptr = fopen(file_name, "r");

    if (file_ptr == NULL) {
        printf("ERROR: cannot open tmp file\n");
        return -1;
    }

    long tmp_file_size = get_file_size(file_ptr);
    char* buffer = calloc(tmp_file_size, sizeof(char));
    fread(buffer, sizeof(char), tmp_file_size, file_ptr);
    fclose(file_ptr);

    for (int i=0; i<ARR_LEN; i++){
        if (ARR[i] == NULL){
            ARR[i] = buffer;
            printf("Saving results to index %d\n", i);
            return i;
        }
    }
    
    free(buffer);
    printf("ERROR: No space left in array\n");
    return -1;
}

int wc_files(int file_count, char** file_names){
    uint32_t file_indexes[file_count];

    if (ARR == NULL){
        printf("ERROR: create_table first!\n");
        return -1;
    }
    for (int i=0; i<file_count; i++){
        char* buffer = calloc(BUFFER_SIZE, sizeof(char));
        int snprintf_result = snprintf(buffer, BUFFER_SIZE, "wc -l -w -c %s > %s", file_names[i], TMP_FILE);
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
        int load_status = load_to_memory(TMP_FILE);

        if (load_status < 0)
            return -1;
        
        file_indexes[i] = load_status;
    }

    printf("All commands executed correctly :). Files with indexes: \n");
    for (int i=0; i<file_count; i++){
        printf("FILE NAME: %s -> INDEX: %d\n", file_names[i], file_indexes[i]);
    }
    
    return 1;
}


void remove_block(uint32_t index){
    if (index >= ARR_LEN){
        printf("ERROR: given index is too big. Max index is: %d\n", ARR_LEN-1);
        return;
    }

    free(ARR[index]);
    printf("Deleted block at index: %d\n", index);
}

