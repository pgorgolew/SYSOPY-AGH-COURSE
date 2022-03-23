#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int MAX_PATH_LEN = 256;

char* get_date(long int time_in_sec){
    char* date = calloc(20, sizeof(char));
    strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&time_in_sec));
    return date;
}

int listFiles(const char* dirname) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        printf("ERROR: cannot open dir: %s\n", dirname);
        return 1;
    }

    struct dirent* entity;
    entity = readdir(dir);
    struct stat entity_stat;
    //char* entity_path = calloc(MAX_PATH_LEN, sizeof(char));
    while (entity != NULL) {
        if (strcmp(entity->d_name, ".") == 0 || strcmp(entity->d_name, "..") == 0){
            entity = readdir(dir);
            continue;
        }

        //TODO DELETE './' from d_name
        char entity_path[MAX_PATH_LEN];
        int snprintf_result = snprintf(entity_path, MAX_PATH_LEN, "%s/%s", dirname, entity->d_name);
        
        //getting stats:
        lstat(entity_path, &entity_stat);
        
        if (snprintf_result < 0 || snprintf_result > MAX_PATH_LEN){
            printf("ERROR: snprintf error, return %d", snprintf_result);
            return 1;
        }

        char* type = "file";
        printf("%lu\t%s\t%ld\t%s\t%s\t%s/%s\n", entity_stat.st_nlink, type, entity_stat.st_size, get_date(entity_stat.st_atime), get_date(entity_stat.st_mtime), dirname, entity->d_name);
        if (entity->d_type == DT_DIR) {
            char path[MAX_PATH_LEN];
            int snprintf_result = snprintf(path, MAX_PATH_LEN, "%s/%s", dirname, entity->d_name);
            
            if (snprintf_result < 0 || snprintf_result > MAX_PATH_LEN){
                printf("ERROR: snprintf error, return %d", snprintf_result);
                return 1;
            }

            listFiles(path);
        }
        entity = readdir(dir);
    }

    closedir(dir);
    return 0;
}

int main(int argc, char* argv[]) {
    //zal ze arv[1] to sciezka
    char total_path[MAX_PATH_LEN];
    int snprintf_result;
    if (argv[1][0] == '/')    
        snprintf_result = snprintf(total_path, MAX_PATH_LEN, "%s", argv[1]);
    else{
        char cwd[MAX_PATH_LEN];
        if (getcwd(cwd, sizeof(cwd)) == NULL){
            printf("ERROR: getcwd() error");
            return 1;
        }
        snprintf_result = snprintf(total_path, MAX_PATH_LEN, "%s/%s", cwd, argv[1]);
    }
        
    if (snprintf_result < 0 || snprintf_result > MAX_PATH_LEN){
    printf("ERROR: snprintf error, return %d", snprintf_result);
    return 1;
    }

    listFiles(total_path);
    return 0;
}