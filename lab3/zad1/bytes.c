//reverse file reading bits in portions
#include <stdio.h>
#include <string.h>
#include <linux/time.h>

int handle_error1(FILE *file);
int handle_error2(FILE *file1, FILE *file2);

int main(int argc, char* argv[]){
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    FILE *file1, *file2;
    if (argc > 1){
        file1 = fopen(argv[1], "r");
        if (argc == 3){
            file2 = fopen(argv[2], "w");
        }
        else{
            file2 = fopen("copied2.txt", "w");
        }
    }
    else{
        file1 = fopen("to_copy.txt", "r");
        file2 = fopen("copied2.txt", "w");
    }
    if (handle_error2(file1, file2) == -1){
        return -1;
    }
    char buffer[10];
    size_t sign_read;
    fseek(file1, 0, SEEK_END);
    long int size = ftell(file1);
    // fseek(file1, 0, SEEK_SET); //?
    do{
        if (size >= 10){
            fseek(file1, -10, SEEK_CUR);
            sign_read = fread(buffer, 1, 10, file1);
        }
        else{
            fseek(file1, -size, SEEK_CUR);
            sign_read = fread(buffer, 1, size, file1);
        }
        for (int i = sign_read - 1; i >= 0; i--){
            fwrite(buffer + i, 1, 1, file2);
        }
        size -= 10;
        fseek(file1, -10, SEEK_CUR);
    } while (size > 0);
    
    fclose(file1);
    fclose(file2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    int time_usage = (end.tv_nsec - start.tv_nsec);
    FILE *file = fopen("pomiar_zad2.txt", "a+");
    if (handle_error1(file) == -1){
        return -1;
    }
    char output[100]; 
    sprintf(output, "Czas dla czytania paczkami 1024 bitowy: %dns\n", time_usage);
    fwrite(output, 1, strlen(output), file);
    return 0;
}

int handle_error2(FILE *file1, FILE *file2){
    if (!file1){
        fprintf(stderr, "error during opening a file1\n");
        return -1;
    }
    if (!file2){
        fprintf(stderr, "error during opening a file2\n");
        return -1;
    }
    return 0;
}

int handle_error1(FILE *file){
    if (!file){
        fprintf(stderr, "error during opening a file1\n");
        return -1;
    }
    return 0;
}