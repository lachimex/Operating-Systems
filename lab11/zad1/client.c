#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int sockfd;
char id[32];

void handle_signal(int signal);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <id> <server_ip> <server_port>\n", argv[0]);
        return -1;
    }

    strncpy(id, argv[1], sizeof(id) - 1);
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);

    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket connection failed");
        close(sockfd);
        return -1;
    }

    if (send(sockfd, id, strlen(id), 0) == -1) {
        perror("ID send failed");
        close(sockfd);
        return -1;
    }

    signal(SIGINT, handle_signal);

    pid_t pid = fork();

    if (pid == 0) {
        char child_buffer[BUFFER_SIZE];
        int nbytes;

        while ((nbytes = recv(sockfd, child_buffer, sizeof(child_buffer), 0)) > 0) {
            child_buffer[nbytes] = '\0';
            if (strcmp(child_buffer, "PING") != 0) printf("%s\n", child_buffer);
        }
    } else {
        char buffer[BUFFER_SIZE];
        while (1) {
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strlen(buffer)] = '\0';

            if (strcmp(buffer, "STOP\n") == 0) {
                send(sockfd, buffer, strlen(buffer), 0);
                break;
            }

            send(sockfd, buffer, strlen(buffer), 0);
        }

        close(sockfd);
        return 0;
    }
}

void handle_signal(int signal) {
    if (signal == SIGINT) {
        send(sockfd, "STOP", 4, 0);
        close(sockfd);
        exit(0);
    }
}
