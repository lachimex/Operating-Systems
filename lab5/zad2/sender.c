//program to send signal to another program based on its pid, wait for a confirmation from the second one

#define _XOPEN_SOURCE 700

#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void SIGUSR1_handler(int signo) {
    printf("Confirmation received\n");
}

int main(int argc, char** argv) {
    if(argc < 3) {
        printf("Usage: %s <catcher_pid> <signal_argument>\n", argv[0]);
        return -1;
    }
    printf("Sender PID: %d\n", getpid());
    signal(SIGUSR1, SIGUSR1_handler);

    long signal_pid = atoi(argv[1]);
    long signal_argument = atoi(argv[2]);

    union sigval value = {signal_argument};

    sigqueue(signal_pid, SIGUSR1, value);
    printf("Signal sent with argument: %ld\n", signal_argument);

    pause();
    return 0;
}