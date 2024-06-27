#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#define ROZM_BLOKU 1024

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

int memory_fd;
sh_memory* memory;
int id;

void cleanup(int signal, int num_of_printers) {
    munmap(memory, sizeof(sh_memory));
    close(memory_fd);
    shm_unlink("shared");
    for (int i = 0; i < num_of_printers; i++){
        semctl(id, 0, IPC_RMID);
    }
}


int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: %s <number_of_printers>\n", argv[0]);
        return -1;
    } 
    long number_of_printers = strtol(argv[1], NULL, 10);

    memory_fd = shm_open("shared", O_CREAT | O_RDWR, 0644);
    if(memory_fd < 0)
        perror("shm_open");

    if(ftruncate(memory_fd, sizeof(sh_memory)) < 0)
        perror("ftruncate");

    memory = mmap(NULL, sizeof(sh_memory), PROT_READ | PROT_WRITE, MAP_SHARED, memory_fd, 0);
    if (memory == MAP_FAILED)
        perror("mmap");
    
    memset(memory, 0, sizeof(sh_memory));
    memory->number_of_printers = number_of_printers;


    key_t klucz2 = ftok("./semafor", 'p');
    id = semget(klucz2, number_of_printers, IPC_CREAT | 0600);
    if (id == -1) printf("blad1\n");
    if (semctl(id, 0, SETVAL, 1) == -1) printf("blad2\n");
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;


    for (int i = 0; i < number_of_printers; i++){
        if (semctl(id, i, SETVAL, 1) == -1) printf("blad2\n");

        pid_t printer_pid = fork();

        if(printer_pid < 0) {
            perror("fork");
            return -1;
        }
        else if(printer_pid == 0) {
            while(1) {
                if (memory->printers[i].status == 1) {

                    for (int j = 0; j < 10; j++) {
                        printf("printer %d -> %c\n", i, memory->printers[i].buffer[j]);
                        sleep(1);
                    }
                    
                    memory->printers[i].status = 0;
                    sb.sem_num = i;
                    if (semop(id, &sb, 1) == -1) {
                        perror("semop");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            return 0;
        }
    }

    while(wait(NULL) > 0);

    cleanup(0, number_of_printers);
    return 0;
}
