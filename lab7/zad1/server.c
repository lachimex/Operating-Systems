#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

struct pakiet {
    long type;
    char buffer[2][1024];
};

struct assigned_number {
    long type;
    int value;
};

int main(){
    int numery_klientow[5], number_of_clients = 0;
    struct pakiet received;
    struct assigned_number as;
    struct pakiet send;
    key_t klucz = ftok("./plik1", 'p');

    if (klucz == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int server = msgget(klucz, IPC_CREAT | 0600);
    if (server == -1) {
        perror("msgget (server)");
        exit(EXIT_FAILURE);
    }

    while(1){
        int info = msgrcv(server, &received, sizeof(struct pakiet), 0, IPC_NOWAIT);
        if (info == -1) {
            if (errno != ENOMSG) {
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
        } else {
            if (strcmp(received.buffer[1], "INIT") == 0){
                if (number_of_clients < 5) {
                    numery_klientow[number_of_clients] = atoi(received.buffer[0]);
                    printf("%d\n", atoi(received.buffer[0]));
                    int client = msgget(numery_klientow[number_of_clients], 0600);
                    if (client == -1) {
                        perror("msgget (client)");
                        exit(EXIT_FAILURE);
                    }
                    as.value = number_of_clients;
                    if (msgsnd(client, &as, sizeof(as), 0) == -1) {
                        perror("msgsnd (INIT)");
                        exit(EXIT_FAILURE);
                    }
                    number_of_clients++;
                    printf("nastepny dostanie: %d\n", number_of_clients);
                    printf("otrzymano wiadomosc INIT\n");
                } else {
                    fprintf(stderr, "Maximum number of clients reached.\n");
                }
            } else {
                strcpy(send.buffer[1], received.buffer[1]);
                printf("wiadomosc od %d\n", atoi(received.buffer[0]));
                for (int i = 0; i < number_of_clients; i++){
                    if (i != atoi(received.buffer[0])){
                        int num = msgget(numery_klientow[i], 0600);
                        if (msgsnd(num, &send, sizeof(send), 0) == -1) {
                            perror("msgsnd");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
        }
        usleep(100000); // sleep for 0.1 seconds to avoid busy waiting
    }

    for (int i = 0; i < number_of_clients; i++){
        msgctl(numery_klientow[i], IPC_RMID, NULL); // <- usuniecie kolejki
    }

    return 0;
}
