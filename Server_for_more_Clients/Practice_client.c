#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define PORT 8080
#define IP INADDR_LOOPBACK

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(IP);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    char city[100] = {0};
    char buffer[100] = {0};

    while (1) {
        printf("Enter city name or 'exit' to quit: ");
        fgets(city, sizeof(city), stdin);
        city[strcspn(city, "\n")] = 0;
        if (send(sock, city, strlen(city), 0) < 0) {
            perror("send");
            close(sock);
            exit(EXIT_FAILURE);
        }
        if (strcmp(city, "exit") == 0) {
            break;
        }
        if (recv(sock, buffer, sizeof(buffer) - 1, 0) < 0) {
            perror("recv");
            close(sock);
            exit(EXIT_FAILURE);
        }
        printf("Time: %s\n", buffer);
        memset(buffer, 0, strlen(buffer));
        memset(city, 0, strlen(city));
    }

    close(sock);

}