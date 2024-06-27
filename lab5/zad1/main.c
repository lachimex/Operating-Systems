//different methods of signal handling

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#define __USE_POSIX


void handler(int);

int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "usage: .\\main {none, ignore, handler, mask}\n");
        return EXIT_FAILURE;
    }
    sigset_t newmask, oldmask, set;
    if (strcmp(argv[1], "none") == 0){
    } else if (strcmp(argv[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);    
    } else if (strcmp(argv[1], "handler") == 0){
        signal(SIGUSR1, handler);
    } else if (strcmp(argv[1], "mask") == 0){
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    } else{
        fprintf(stderr, "usage: .\\main {none, ignore, handler, mask}\n");
        return -1;
    }
    raise(SIGUSR1);
    sigpending(&set);
    if (sigismember(&set, SIGUSR1) == 1)
        printf("SIGUSR1 oczekuje na odblokowanie\n");
    return 0;
}

void handler(int signum){
    printf("otrzymano sygnal SIGUSR1\n");
}