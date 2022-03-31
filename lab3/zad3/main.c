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
    char* buffor = calloc(MAX_PATH_LEN, sizeof(char));

    while(fgets(buffor, MAX_PATH_LEN*sizeof(char), file)){
        if (strstr(buffor,pattern)){
            printf("PATH: %s\tPID:%d\n", path, getpid());
            break;    
        }
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

        char* extension = strrchr(entity->d_name,'.');
        if (entity->d_type == DT_DIR && depth_left > 0) {
            processes++;
            if (fork() == 0){
                listFiles(entity_path, depth_left-1);
                return 0;
            }
                
        }

        else if (S_ISREG(entity_stat.st_mode) && extension != NULL && strcmp(extension, ".txt") == 0)
            is_pattern(entity_path);
        
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