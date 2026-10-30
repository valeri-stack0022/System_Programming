#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#define MAXCLIENT 3



#define PORT 8080
#define IP INADDR_ANY

int get_offset(const char* city) {
    if (strcmp(city, "Yerevan") == 0) {
        return 4;
    } else if (strcmp(city, "London") == 0) {
        return 1;
    } else if (strcmp(city, "New York") == 0) {
        return -4;
    } else if (strcmp(city, "Tokyo") == 0) {
        return 9;
    } else {
        return 0;
    }
}

char* get_time(const char* request) {
    int offset = get_offset(request);
    char *response = malloc(20);
    if (offset == 0) {
        strcpy(response, "UNKNOWN CITY");
        return response;
    }

    time_t rawtime;
    time(&rawtime);
    struct tm *utc_time = gmtime(&rawtime);
    utc_time->tm_hour += get_offset(request);
    mktime(utc_time);

    strftime(response, 20, "%H:%M:%S", utc_time);
    return response;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    int sopt = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (sopt < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(IP);

    int b = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (b < 0) {
        perror("bind");
        close(sock);
        exit(EXIT_FAILURE);
    }


    int l = listen(sock, SOMAXCONN);
    if (l < 0) {
        perror("listen");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);
    struct sockaddr_in client_addr = {0};
    socklen_t client_len = sizeof(client_addr);


    int clients[MAXCLIENT] = {-1, -1, -1};
    fd_set readfds;
    int max_fd = sock;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        for (int i = 0; i < MAXCLIENT; ++i) {
            if (clients[i] != -1) {
                FD_SET(clients[i], &readfds);
                if (clients[i] > max_fd) {
                    max_fd = clients[i];
                }
            }
        }
        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(sock, &readfds)) {
            int client_sock = accept(sock, (struct sockaddr*)&client_addr, &client_len);
            if (client_sock < 0) {
                perror("accept");
                close(sock);
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < MAXCLIENT; ++i) {
                if (clients[i] == -1) {
                    clients[i] = client_sock;
                    break;
                }
            }
        }

        for (int i = 0; i < MAXCLIENT; ++i) {
            if (clients[i] == -1) {
                continue;
            }
            if (FD_ISSET(clients[i], &readfds)) {
                char buffer[1024] = {0};
                int bytes_received = recv(clients[i], buffer, sizeof(buffer) - 1, 0);
                if (bytes_received < 0) {
                    perror("recv");
                    close(sock);
                    close(clients[i]);
                    exit(EXIT_FAILURE);
                }
                buffer[bytes_received] = '\0';
                if (strcmp(buffer, "exit") == 0) {
                    close(sock);
                    close(clients[i]);
                    exit(EXIT_SUCCESS);
                }

                char *response = get_time(buffer);
                send(clients[i], response, strlen(response), 0);
                free(response);
            }
        }

    }
}