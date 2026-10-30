#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define SIZE 512

int main()
{
    int client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock_fd == -1){
        perror("Socket creation is failed!\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &client_addr.sin_addr);

    int c = connect(client_sock_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if(c < 0){
        perror("Connection failed!\n");
        exit(EXIT_FAILURE);
    }

    char recving_string[SIZE] = {0};
    ssize_t r1 = recv(client_sock_fd, recving_string, SIZE, 0);
    if(r1 < 0){
        perror("Recv is failed!\n");
        close(client_sock_fd);
        exit(EXIT_FAILURE);
    }
    else if(r1 == 0){
        printf("Nothing to read!\n");
        printf("Server Disconnected!\n");
        close(client_sock_fd);
        exit(EXIT_FAILURE);
    }
    else{
        recving_string[strcspn(recving_string, "\n")] = '\0';
        printf("%s\n", recving_string);
    }

    while(1){
        char sending_string[SIZE] = {0};
        if(fgets(sending_string, SIZE - 1, stdin) == NULL){
            printf("Nothing to send!\n");
            exit(EXIT_FAILURE);
        }
        else{
            sending_string[strcspn(sending_string, "\n")] = '\0';
            ssize_t s1 = send(client_sock_fd, sending_string, strlen(sending_string), 0);
            if(s1 == -1){
                perror("Sending is failed!\n");
                close(client_sock_fd);
                exit(EXIT_FAILURE);
            }

            char recving_echo[SIZE] = {};
            ssize_t r2 = recv(client_sock_fd, recving_echo, SIZE, 0);
            if(r2 < 0){
                perror("Recv is failed!\n");
                close(client_sock_fd);
                exit(EXIT_FAILURE);
            }
            else if(r2 == 0){
                printf("Nothing to read!\n");
                printf("Server Disconnected!\n");
                close(client_sock_fd);
                exit(EXIT_FAILURE);
            }
            else{
                recving_echo[strcspn(recving_echo, "\n")] = '\0';
                printf("%s\n", recving_echo);
            }
        }
    }
}