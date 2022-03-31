#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>

int MAX_PATH_LEN = 256;
int processes = 0;
char* pattern;
int pattern_len;

void is_pattern(char* path){
    FILE* file = fopen(path, "r");
    char* buffor = calloc(pattern_len+1, sizeof(char));
    int out, tmp, curr_char=0;
    char first = pattern[0];
    while((out=fread(buffor, 1, pattern_len+1, file)) >= pattern_len){
        tmp = curr_char;
        if (strcmp(buffor, pattern) == 0){
            printf("PATH: %s\tPID:%d\n", path, getpid());
            break;
        }

        for (int i=1; i<=pattern_len; i++){
            if (buffor[i] == first){
                curr_char += i;
                fseek(file, curr_char, SEEK_SET);
                break;
            }
        }

        if (tmp == curr_char)
            curr_char+=pattern_len+1;
    }

    fclose(file);
    free(buffor);
    return;
}   


int listFiles(const char* dirname, int depth_left) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        printf("ERROR: cannot open dir: %s\n", dirname);
        return 1;
    }

    struct dirent* entity;
    entity = readdir(dir);
    struct stat entity_stat;
    while (entity != NULL) {
        if (strcmp(entity->d_name, ".") == 0 || strcmp(entity->d_name, "..") == 0){
            entity = readdir(dir);
            continue;
        }

        char entity_path[MAX_PATH_LEN];
        int snprintf_result = snprintf(entity_path, MAX_PATH_LEN, "%s/%s", dirname, entity->d_name);
        
        lstat(entity_path, &entity_stat);
        
        if (snprintf_result < 0 || snprintf_result > MAX_PATH_LEN){
            printf("ERROR: snprintf error, return %d", snprintf_result);
            return 1;
        }

        if (entity->d_type == DT_DIR && depth_left > 0) {
            char path[MAX_PATH_LEN];
            int snprintf_result = snprintf(path, MAX_PATH_LEN, "%s/%s", dirname, entity->d_name);
            
            if (snprintf_result < 0 || snprintf_result > MAX_PATH_LEN){
                printf("ERROR: snprintf error, return %d", snprintf_result);
                return 1;
            }
            processes++;
            if (fork() == 0)
                listFiles(path, depth_left-1);
            
        }
        else if (S_ISREG(entity_stat.st_mode)){
            is_pattern(entity_path);
        }
        entity = readdir(dir);
    }

    closedir(dir);
    return 0;
}


int main(int arg_len, char **args){
    if (arg_len < 4){
        printf("ERROR: you have to give: <directory> <pattern> <dir_search_depth>\n");
        return 1;
    }
    int status;
    char* dir = args[1];
    pattern = args[2];
    pattern_len = strlen(pattern);
    int depth = atoi(args[3]);

    listFiles(dir, depth);
    
    while (wait(&status) > 0);
    return 0;
}