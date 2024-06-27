#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    struct sockaddr_in addr;
    char id[32];
    int active;
} client_t;

typedef struct {
    char id[32];
    char data[BUFFER_SIZE];
} client_data;

client_t clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int sockfd;

void send_message_to_all(char *message, struct sockaddr_in *exclude_addr) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && memcmp(&clients[i].addr, exclude_addr, sizeof(struct sockaddr_in)) != 0) {
            sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_client(char *message, char *client_id) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].id, client_id) == 0) {
            sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr));
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(struct sockaddr_in *client_addr) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (memcmp(&clients[i].addr, client_addr, sizeof(struct sockaddr_in)) == 0) {
            clients[i].active = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* ping_clients(void *arg) {
    while (1) {
        sleep(30);
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active) {
                if (sendto(sockfd, "PING", sizeof("PING"), 0, (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr)) <= 0) {
                    clients[i].active = 0;
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    return NULL;
}

void* handle_client(void *arg) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    client_data cd;

    while (1) {
        int nbytes = recvfrom(sockfd, &cd, sizeof(cd), 0, (struct sockaddr*)&client_addr, &addr_len);
        if (nbytes <= 0) {
            continue;
        }

        cd.data[nbytes - sizeof(cd.id)] = '\0'; // Ensure null-terminated string

        char command[16], message[BUFFER_SIZE], target[32];
        sscanf(cd.data, "%s", command);

        if (strcmp(command, "LIST") == 0) {
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active) {
                    sendto(sockfd, clients[i].id, strlen(clients[i].id), 0, (struct sockaddr*)&client_addr, addr_len);
                }
            }
            sendto(sockfd, "\n", 1, 0, (struct sockaddr*)&client_addr, addr_len);
            pthread_mutex_unlock(&clients_mutex);
        } else if (strcmp(command, "2ALL") == 0) {
            sscanf(cd.data, "%s %[^\n]", command, message);
            char formatted_message[BUFFER_SIZE];
            sprintf(formatted_message, "[%s]: %s", cd.id, message);
            send_message_to_all(formatted_message, &client_addr);
        } else if (strcmp(command, "2ONE") == 0) {
            sscanf(cd.data, "%s %s %[^\n]", command, target, message);
            char formatted_message[BUFFER_SIZE];
            sprintf(formatted_message, "[%s]: %s", cd.id, message);
            send_message_to_client(formatted_message, target);
        } else if (strcmp(command, "STOP") == 0) {
            remove_client(&client_addr);
            break;
        } else if (strcmp(command, "INIT") == 0) {
            pthread_mutex_lock(&clients_mutex);
            int found = 0;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active == 0) {
                    clients[i].addr = client_addr;
                    strcpy(clients[i].id, cd.id);
                    clients[i].active = 1;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                fprintf(stderr, "Max clients limit reached. Cannot add more clients.\n");
            }
            pthread_mutex_unlock(&clients_mutex);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }

    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket bind failed");
        close(sockfd);
        return -1;
    }

    printf("Server listening on port %s\n", argv[1]);

    pthread_t ping_tid;
    pthread_create(&ping_tid, NULL, ping_clients, NULL);

    while (1) {
        handle_client(NULL);
    }

    close(sockfd);
    return 0;
}
