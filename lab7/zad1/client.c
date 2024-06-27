#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

struct pakiet {
    long type;
    char buffer[2][1024];
};

struct assigned_number {
    long type;
    int value;
};

int main(){
    struct pakiet o1;
    struct assigned_number as;
    key_t server = ftok("./plik1", 'p');
    srand(time(NULL));
    key_t client = rand();
    if (server == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int server_kol = msgget(server, 0600);
    if (server_kol == -1) {
        perror("msgget (server)");
        exit(EXIT_FAILURE);
    }
    printf("client key: %d\n", client);
    int client_kol = msgget(client, IPC_CREAT | 0600);
    printf("client_kol: %d\n", client_kol);
    if (client_kol == -1) {
        perror("msgget (client)");
        exit(EXIT_FAILURE);
    }
    sprintf(o1.buffer[1], "INIT");
    sprintf(o1.buffer[0], "%d", client);
    printf("wysylam sygnal INIT\n");
    if (msgsnd(server_kol, &o1, sizeof(o1), 0) == -1) {
        perror("msgsnd (INIT)");
        exit(EXIT_FAILURE);
    }
    while(1){
        int info = msgrcv(client_kol, &as, sizeof(as), 0, IPC_NOWAIT);
        if (info == -1) {
            if (errno != ENOMSG) {
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("assigned number: %d\n", as.value);
            break;
        }
    }
    pid_t child_pid;
    struct pakiet received;
    sprintf(o1.buffer[0], "%d", as.value);
    child_pid = fork();
    while(1){
        if (child_pid == 0){
            int info = msgrcv(client_kol, &received, sizeof(received), 0, IPC_NOWAIT);
            usleep(100000); // sleep for 0.1 seconds to avoid busy waiting
            if (info == -1) {
                if (errno != ENOMSG) {
                    perror("msgrcv");
                    exit(EXIT_FAILURE);
                }
            } else {
                printf("%s", received.buffer[1]);
            }
        } else{
            fgets(o1.buffer[1], sizeof(o1.buffer[1]), stdin);
            msgsnd(server_kol, &o1, sizeof(o1), 0);
            usleep(100000);
        }
    }

    msgctl(client_kol, IPC_RMID, NULL); // <- usuniecie kolejki
    return 0;
}
