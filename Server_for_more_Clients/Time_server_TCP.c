#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <sys/select.h>

#define IP "127.0.0.1"
#define PORT 9292
#define SIZE 512
#define MAXCLIENT 3

int main()
{
    // Socket creation...
    int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1)
    {
        perror("Socket creation failed!\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &server_addr.sin_addr);

    int opt = 1;
    setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Binding...
    int b = bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (b == -1)
    {
        perror("Bind is failed!\n");
        exit(EXIT_FAILURE);
    }
    // Listening...
    int l = listen(server_sock_fd, SOMAXCONN);
    if (l == -1)
    {
        perror("Listyen failed!\n");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening...\n");

    struct sockaddr_in clients_addr;
    memset(&clients_addr, 0, sizeof(clients_addr));
    socklen_t length = sizeof(clients_addr);

    // UTC creation...
    time_t rawtime;
    time(&rawtime);
    struct tm *utc_time = gmtime(&rawtime);
    mktime(utc_time);

    int clients[MAXCLIENT] = {-1, -1, -1};
    fd_set readfds;
    int max_fd = server_sock_fd;

    while (1)
    {

        FD_ZERO(&readfds);
        FD_SET(server_sock_fd, &readfds);
        max_fd = server_sock_fd;

        for (int i = 0; i < MAXCLIENT; ++i){
            if (clients[i] != -1){
                FD_SET(clients[i], &readfds);
                if (clients[i] > max_fd){
                    max_fd = clients[i];
                }
            }
        }
        select(max_fd + 1, &readfds, NULL, NULL, NULL); // Ինչու max_fd + 1 ?

        if (FD_ISSET(server_sock_fd, &readfds)){
            int new_sock = accept(server_sock_fd, (struct sockaddr *)&server_addr, &length);
            if (new_sock == -1){
                perror("Accepting failed!\n");
                close(server_sock_fd);
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < MAXCLIENT; ++i){
                if (clients[i] == -1){
                    clients[i] = new_sock;
                    break;
                }
            }
        }

        for (int i = 0; i < MAXCLIENT; ++i){
            if (clients[i] == -1){
                continue;
            }
            if (FD_ISSET(clients[i], &readfds)){
                char city_name[SIZE] = {0};
                ssize_t r1 = recv(clients[i], city_name, SIZE, 0);
                if (r1 > 0){
                    city_name[r1] = '\0';
                    if (strcmp(city_name, "Yerevan") != 0 && strcmp(city_name, "New York") != 0 && strcmp(city_name, "Tokyo") != 0 && strcmp(city_name, "London") != 0 && strcmp(city_name, "UTC") != 0){
                        ssize_t sending_invalid_city = send(clients[i], "UNKNOWN CITY", strlen("UNKNOWN CITY"), 0);
                        if (sending_invalid_city == -1){
                            perror("Send is failed!\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if (strcmp(city_name, "UTC") == 0){
                        char utc_time_string[SIZE] = {0};
                        strftime(utc_time_string, sizeof(utc_time_string), "%H:%M:%S", utc_time);
                        ssize_t s1 = send(clients[i], utc_time_string, strlen(utc_time_string), 0);
                        if (s1 == -1){
                            perror("Sending failed!\n");
                            exit(EXIT_FAILURE);
                        }
                        time(&rawtime);
                        utc_time = gmtime(&rawtime);
                    }
                    else if (strcmp(city_name, "Yerevan") == 0){
                        utc_time->tm_hour += 4;
                        mktime(utc_time);
                        char Yerevan_time_string[SIZE] = {0};
                        strftime(Yerevan_time_string, sizeof(Yerevan_time_string), "%H:%M:%S", utc_time);
                        printf("%s\n", Yerevan_time_string);
                        ssize_t s1 = send(clients[i], Yerevan_time_string, SIZE, 0);
                        if (s1 == -1){
                            perror("Sending failed!\n");
                            exit(EXIT_FAILURE);
                        }
                        time(&rawtime);
                        utc_time = gmtime(&rawtime);
                    }
                    else if (strcmp(city_name, "London") == 0){
                        utc_time->tm_hour += 1;
                        mktime(utc_time);
                        char London_time_string[SIZE] = {0};
                        strftime(London_time_string, sizeof(London_time_string), "%H:%M:%S", utc_time);
                        ssize_t s1 = send(clients[i], London_time_string, SIZE, 0);
                        if (s1 == -1){
                            perror("Sending failed!\n");
                            exit(EXIT_FAILURE);
                        }
                        time(&rawtime);
                        utc_time = gmtime(&rawtime);
                    }
                    else if (strcmp(city_name, "New York") == 0){
                        utc_time->tm_hour -= 4;
                        mktime(utc_time);
                        char New_York_time_string[SIZE] = {0};
                        strftime(New_York_time_string, sizeof(New_York_time_string), "%H:%M:%S", utc_time);
                        ssize_t s1 = send(clients[i], New_York_time_string, SIZE, 0);
                        if (s1 == -1)
                        {
                            perror("Sending failed!\n");
                            exit(EXIT_FAILURE);
                        }
                        time(&rawtime);
                        utc_time = gmtime(&rawtime);
                    }
                    else if (strcmp(city_name, "Tokyo") == 0)
                    {
                        utc_time->tm_hour += 9;
                        mktime(utc_time);
                        char Tokyo_time_string[SIZE] = {0};
                        strftime(Tokyo_time_string, sizeof(Tokyo_time_string), "%H:%M:%S", utc_time);
                        ssize_t s1 = send(clients[i], Tokyo_time_string, SIZE, 0);
                        if (s1 == -1)
                        {
                            perror("Sending failed!\n");
                            exit(EXIT_FAILURE);
                        }
                        time(&rawtime);
                        utc_time = gmtime(&rawtime);
                    }
                }
                else if (r1 == 0)
                {
                    printf("Client disconnected.\n");
                    close(clients[i]);
                    clients[i] = -1;
                }
                else
                {
                    perror("Recving failed!\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}
