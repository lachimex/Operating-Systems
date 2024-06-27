#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>


int main(){
    DIR* dir = opendir(".");
    if (!dir){
        fprintf(stderr, "error looking for a folder\n");
    }

    struct dirent *file = readdir(dir);
    struct stat buf;
    long long size = 0;
    while(file){
        if (stat(file->d_name, &buf) == -1){
            fprintf(stderr, "error retrievieng stats from a file or a directory\n");
        }
        if (!S_ISDIR(buf.st_mode)){
            printf("Reading file: %s\n", file->d_name);
            printf("Size of file: %s -> %ld\n", file->d_name, buf.st_size);
            size += buf.st_size;
        }
        file = readdir(dir);
    }
    printf("Size of all files: %lld\n", size);

    int result = closedir(dir);
    if(result == -1){
        fprintf(stderr, "error closing folder\n");
    }
    return 0;
}