#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>


struct counter{
    int files;
    int dirs;
    int fifos;
    int sockets;
    int char_devs;
    int block_devs;
    int links;
    int unknows;
};

struct counter c = {0,0,0,0,0,0,0,0};
struct counter* ptr_c = &c;
int MAX_PATH_LEN = 256;

char* path_without_last_dir(char* cwd){
    int last_index = strlen(cwd) - 1;
    while (cwd[last_index] != '/')
        last_index--;
    
    cwd[last_index] = '\0';
    return cwd;
}

char* get_date(long int time_in_sec){
    char* date = calloc(20, sizeof(char));
    strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&time_in_sec));
    return date;
}

char* get_type(mode_t mode){
    if (S_ISDIR(mode)){
        c.dirs++;
        return "Directory";
    }
    else if (S_ISREG(mode)){
        c.files++;
        return "Regular file";    
    }
    else if (S_ISFIFO(mode)){
        c.fifos++;
        return "FIFO\t";
    }
    else if (S_ISSOCK(mode)){
        c.sockets++;
        return "SOCKET\t";
    }
    else if (S_ISCHR(mode)){
        c.char_devs++;
        return "Character device";
    }
    else if (S_ISBLK(mode)){
        c.block_devs++;
        return "Block device";
    }    
    else if (S_ISLNK(mode)){
        c.links++;
        return "LINK\t";
    }
    c.unknows++;
    return "Unknown\t";
}

int parse_nftw_res(const char* filename, const struct stat* statptr, int tflag, struct FTW* ftwbuf){
    char* type = get_type(statptr->st_mode);
    printf("%lu\t%s\t%ld\t%s\t%s\t%s\n", statptr->st_nlink, type, statptr->st_size, get_date(statptr->st_atime), 
            get_date(statptr->st_mtime), filename);
    return 0;
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

        char* type = get_type(entity_stat.st_mode);
        printf("%lu\t%s\t%ld\t%s\t%s\t%s/%s\n", entity_stat.st_nlink, type, entity_stat.st_size, get_date(entity_stat.st_atime), 
                get_date(entity_stat.st_mtime), dirname, entity->d_name);
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
    char total_path[MAX_PATH_LEN];
    realpath(argv[1], total_path);
    printf("NLINKS\tTYPE\t\tSIZE\tLAST_ACCES\t\tLAST_MODIFICATION\tABS_PATH\n");
    #ifdef NFTW
        nftw(total_path, parse_nftw_res, 20, 0);
    #else
        listFiles(total_path);
    #endif

    printf("TOTALS:\n");
    printf("DIR: %d   FILE: %d   FIFO: %d   SOCKET: %d   CHAR_DEV: %d   BLOCK_DEV: %d   LINK: %d   UNKNOW: %d\n", 
           c.dirs, c.files, c.fifos, c.sockets, c.char_devs, c.block_devs, c.links, c.unknows);
    return 0;
}