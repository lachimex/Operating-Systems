#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int sockfd;
struct sockaddr_in server_addr;


typedef struct {
    char id[32];
    char data[BUFFER_SIZE];
} client_data;

client_data cd;

void handle_signal(int signal);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <id> <server_ip> <server_port>\n", argv[0]);
        return -1;
    }

    strncpy(cd.id, argv[1], sizeof(cd.id) - 1);
    cd.id[sizeof(cd.id) - 1] = '\0';
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    signal(SIGINT, handle_signal);

    strcpy(cd.data, "INIT");
    sendto(sockfd, &cd, sizeof(cd), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    pid_t pid = fork();

    if (pid == 0) {
        char child_buffer[BUFFER_SIZE];
        int nbytes;
        socklen_t addr_len = sizeof(server_addr);

        while ((nbytes = recvfrom(sockfd, child_buffer, sizeof(child_buffer) - 1, 0, (struct sockaddr*)&server_addr, &addr_len)) > 0) {
            child_buffer[nbytes] = '\0';
            if (strcmp(child_buffer, "PING") != 0) {
                printf("%s\n", child_buffer);
            }
        }
    } else {
        while (1) {
            fgets(cd.data, BUFFER_SIZE, stdin);
            cd.data[strlen(cd.data) - 1] = '\0';

            if (strcmp(cd.data, "STOP") == 0) {
                sendto(sockfd, &cd, sizeof(cd), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
                break;
            }

            sendto(sockfd, &cd, sizeof(cd), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        }

        close(sockfd);
        return 0;
    }
}

void handle_signal(int signal) {
    if (signal == SIGINT) {
        cd.id[sizeof(cd.id) - 1] = '\0';
        strcpy(cd.data, "STOP");
        sendto(sockfd, &cd, sizeof(cd), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        close(sockfd);
        exit(0);
    }
}
