//program to catch the signal sent by sender program and send back the information

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

int status = 0;
int status_changes = 0;

void SIGUSR1_action(int signo, siginfo_t *info, void* more){
    printf("Received status: %d from pid: %d\n", info->si_int, info->si_pid);
    status_changes++;
    status = info->si_int;
    kill(info->si_pid, SIGUSR1);
}

int main() {
    printf("Catcher PID: %d\n", getpid());
    struct sigaction action;
    action.sa_sigaction = SIGUSR1_action;
    action.sa_flags = SA_SIGINFO;  
    sigemptyset(&action.sa_mask);        

    sigaction(SIGUSR1, &action, NULL);
    while(1) {
        pause();
        switch(status){
            case 1:
                for(int i = 1; i <= 100; i++){
                    printf("%d ", i);
                }
                printf("\n");
                status = 0;
                break;
            case 2:
                printf("So far status has changed %d times\n", status_changes);
                status = 0;
                break;
            case 3:
                printf("Exiting...\n");
                exit(0);
                break;
            default:
                printf("No argument defined\n");
                break;
        }
    } 

    return 0;
}