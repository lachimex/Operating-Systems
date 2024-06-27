//child process execute ls on directory provided from a user

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int global = 0;

int main(int argc, char* argv[]) {
    int local = 0;
    if (argc != 2){
        fprintf(stderr, "usage: ./name {path to directory}\n");
        return -1;
    } else{
        int size = sizeof(argv[0]) - 2;
        char* name = malloc(size);
        for (int i = 2; i < size; i++){
            name[i-2] = argv[0][i];
        }
        printf("program name: %s\n", name);
        free(name);
    }
    
    pid_t child_pid = fork();
    if (child_pid == -1){
        fprintf(stderr, "error creating child process\n");
        return -1;
    }
    if (child_pid == 0){
        printf("child process\n");
        global++;
        local++;
        printf("child pid = %d, parent pid = %d\n", child_pid, (int)getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        printf("child process local adress: %p\n", &local);
        int status = execl("/bin/ls", argv[1], NULL);
        if (status == -1){
            fprintf(stderr, "error executing function");
            return -1;
        }
        return 0;
    } else{
        printf("parent process\n");
        printf("parent pid = %d, child pid = %d\n", (int)getppid(), child_pid);
        printf("Child exit code: %d\n", child_pid);
        printf("parent's local = %d, parent's global = %d\n", local, global);
        printf("parent process local adress: %p\n", &local);
    }
    while(wait(0) > 0);
    return 0;
}