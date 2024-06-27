//basic program to present how creating new processes works

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2){
        fprintf(stderr, "usage: ./name {number of processes}\n");
        return -1;
    }
    int num_of_p = atoi(argv[1]);
    pid_t child_pid;
    for (int i = 0; i < num_of_p; i++){
        child_pid = fork();
        if (child_pid == -1){
            fprintf(stderr, "error creating child process\n");
            return -1;
        }
        if (child_pid == 0){
            printf("pid macierzystego: %d, pid_aktualnego(potomnego): %d\n", (int)getppid(), (int)getpid());
            return 0;
        }
    }
    while (wait(0) > 0);
    printf("proces macierzysty wywołał %d procesow\n", num_of_p);

    return 0;
}