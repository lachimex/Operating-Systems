#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

int main() {
    int status, gniazdo;
    struct sockaddr_in ser, cli;
    char buf[200];
    gniazdo = socket(AF_INET, SOCK_STREAM, 0);
    if (gniazdo == -1) {
        printf("blad socket\n");
        return 0;
    }

    memset(&ser, 0, sizeof(ser));
    ser.sin_family = AF_INET;
    ser.sin_port = htons(9002);
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");

    status = connect(gniazdo, (struct sockaddr *) &ser, sizeof(ser));
    if (status < 0) {
        printf("blad connect\n");
        return 0;
    }
    while(1) {
        printf("podaj teskt:");
        fgets(buf, sizeof(buf), stdin);
        status = write(gniazdo, buf, strlen(buf));
        status = read(gniazdo, buf, sizeof(buf) - 1);
        printf("Otrzymalem: %s\n", buf);
    }
    close(gniazdo);
    printf("KONIEC DZIALANIA KLIENTA\n");
    return 0;
}