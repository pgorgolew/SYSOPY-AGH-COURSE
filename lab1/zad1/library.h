#ifndef LIBRARY_H 
#define LIBRARY_H
#include <stdint.h>
#include <stdio.h>

void create_table(uint32_t array_len);

long get_file_size(FILE* file);

int load_to_memory();

int wc_files(int file_count, char** file_names);

void remove_block(uint32_t index);

#endif