#define _XOPEN_SOURCE 700
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <time.h>

typedef struct {
    int id;
    char buffer[11];
    int status; //1 printing 0 waiting;
} printer;

typedef struct
{
    printer printers[256];
    int number_of_printers;
} sh_memory;


#define ROZM_BLOKU 1024


void generate_random_string(char* buffer, int length) {
    srand(time(NULL));
    for(int i = 0; i < length; i++) {
        buffer[i] = 'a' + rand() % 26;
    }
    buffer[length] = '\0';
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: %s <number_of_users>\n", argv[0]);
        return -1;
    }

    long number_of_users = strtol(argv[1], NULL, 10);

    int memory_fd = shm_open("shared", O_CREAT | O_RDWR, 0644);
    if(memory_fd < 0)
        perror("shm_open");

    sh_memory* memory = mmap(NULL, sizeof(sh_memory), PROT_READ | PROT_WRITE, MAP_SHARED, memory_fd, 0);
    if (memory == MAP_FAILED)
        perror("mmap");

    char user_buffer[ROZM_BLOKU];

    key_t klucz2 = ftok("./semafor", 'p');
    int id = semget(klucz2, memory->number_of_printers, O_RDONLY);
    if (id == -1) printf("blad1\n");

    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;


    for (int i = 0; i < number_of_users; i++){
        pid_t user_pid = fork(); 
        if (user_pid < 0) {
            perror("fork");
            return -1;
        }
        else if(user_pid == 0) {
            sleep(i);
            while(1) {
                generate_random_string(user_buffer, 10);

                int printer_index = -1;
                for (int j = 0; j < memory->number_of_printers; j++) {
                    int val;
                    val = semctl(id, j, GETVAL, 1);
                    if(val > 0) {
                        printer_index = j;
                        break;
                    }
                }

                if(printer_index == -1)
                    printer_index = rand() % memory->number_of_printers;

                sb.sem_num = printer_index;
                if (semop(id, &sb, 1) == -1) {
                    perror("semop");
                    exit(EXIT_FAILURE);
                }

                strcpy(memory->printers[printer_index].buffer, user_buffer);

                memory->printers[printer_index].status = 1;

                printf("User %d is printing on printer %d, message->%s\n", i, printer_index, user_buffer);
                fflush(stdout);

                int time = rand() % 5 + 1;
                printf("user %d is going to sleep for %d seconds\n", i, time);
                sleep(time);
            }
            return 0;         
        }
    }

    while(wait(NULL) > 0);

    munmap(memory, sizeof(sh_memory));
}