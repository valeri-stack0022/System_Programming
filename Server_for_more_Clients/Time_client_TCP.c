#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9292
#define SIZE 512

int main()
{
    int client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock_fd == -1){
        perror("Socket creation failed!\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);

    int c = connect(client_sock_fd, (struct sockaddr*)&addr, sizeof(addr));
    if(c == -1){
        perror("Connect is failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("You can find out the current times for the following cities:\n");
    printf("Yerevan | London | New York | Tokyo | UTC\n");
    printf("Or write /exit/ for Quit\n");
    while(1){
        printf("Input your city -> ");
        char city_name[SIZE] = {0};
        if(fgets(city_name, SIZE - 1, stdin) == NULL){
            perror("You write nothing!\n");
            exit(EXIT_FAILURE);
        }
        city_name[strcspn(city_name, "\n")] = '\0';
        if(strcmp(city_name, "exit") == 0){
            printf("Exited.\n");
            exit(EXIT_SUCCESS);
        }
        ssize_t s1 = send(client_sock_fd, city_name, strlen(city_name), 0);
        if(s1 == -1){
            perror("Send id failed!\n");
            exit(EXIT_FAILURE);
        }

        char city_time[SIZE] = {0};
        ssize_t r1 = recv(client_sock_fd, city_time, SIZE, 0);
        if(r1 > 0){
            city_time[r1] = '\0';
            printf("%s\n", city_time);
        }
        else if(r1 == 0){
            perror("Nothing to read!\n");
            exit(EXIT_FAILURE);
        }
        else{
            perror("Recving failed!\n");
            exit(EXIT_FAILURE);
        }
    }
}
