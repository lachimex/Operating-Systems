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
    int sockfd;
    char id[32];
    int active;
} client_t;

client_t clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void send_message_to_all(char *message, int exclude_sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].sockfd != exclude_sockfd) {
            send(clients[i].sockfd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_client(char *message, char *client_id) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && strcmp(clients[i].id, client_id) == 0) {
            send(clients[i].sockfd, message, strlen(message), 0);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int client_sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == client_sockfd) {
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
                if (send(clients[i].sockfd, "PING", sizeof("PING"), 0) <= 0) {
                    clients[i].active = 0;
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    return NULL;
}

void* handle_client(void *arg) {
    int client_sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];
    char id[32];
    int nbytes;

    if ((nbytes = recv(client_sockfd, id, sizeof(id), 0)) <= 0) {
        close(client_sockfd);
        return NULL;
    }

    id[nbytes] = '\0';

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active == 0) {
            clients[i].sockfd = client_sockfd;
            strcpy(clients[i].id, id);
            clients[i].active = 1;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    while ((nbytes = recv(client_sockfd, buffer, sizeof(buffer), 0)) > 0) {
        buffer[nbytes] = '\0';
        char command[16], message[BUFFER_SIZE], target[32];

        sscanf(buffer, "%s", command);

        if (strcmp(command, "LIST") == 0) {
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].active) {
                    send(client_sockfd, clients[i].id, strlen(clients[i].id), 0);
                    send(client_sockfd, "\n", strlen("\n"), 0);
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if (strcmp(command, "2ALL") == 0) {
            sscanf(buffer, "%s %[^\n]", command, message);
            char formatted_message[BUFFER_SIZE];
            sprintf(formatted_message, "[%s]: %s", id, message);
            send_message_to_all(formatted_message, client_sockfd);
        } else if (strcmp(command, "2ONE") == 0) {
            sscanf(buffer, "%s %s %[^\n]", command, target, message);
            char formatted_message[BUFFER_SIZE];
            sprintf(formatted_message, "[%s]: %s", id, message);
            send_message_to_client(formatted_message, target);
        } else if (strcmp(command, "STOP") == 0) {
            break;
        }
    }

    remove_client(client_sockfd);
    close(client_sockfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }

    int server_sockfd, client_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket bind failed");
        close(server_sockfd);
        return -1;
    }

    if (listen(server_sockfd, MAX_CLIENTS) == -1) {
        perror("Socket listen failed");
        close(server_sockfd);
        return -1;
    }

    printf("Server listening on port %s\n", argv[1]);

    pthread_t ping_tid;
    pthread_create(&ping_tid, NULL, ping_clients, NULL);

    while (1) {
        client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sockfd == -1) {
            perror("Client accept failed");
            continue;
        }

        pthread_t client_tid;
        pthread_create(&client_tid, NULL, handle_client, &client_sockfd);
        pthread_detach(client_tid);
    }

    close(server_sockfd);
    return 0;
}
