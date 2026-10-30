#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define IP "127.0.0.1"
#define PORT 9999
#define SIZE 512

int main(){
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock == -1){
        perror("Socket creation failed!\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &client_addr.sin_addr);

    int c = connect(client_sock, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if(c == -1){
        perror("Connection failed!\n");
        exit(EXIT_FAILURE);
    }

    char choice_question[SIZE] = {0};
    ssize_t r1 = recv(client_sock, choice_question, SIZE, 0);
    if(r1 < 0){
        perror("Recv failed!\n");
        close(client_sock);
        exit(EXIT_FAILURE);
    }
    else if(r1 == 0){
        printf("Nothing to read!\n");
        printf("Server Disconnected!\n");
        close(client_sock);
        exit(EXIT_FAILURE);
    }
    choice_question[strcspn(choice_question, "\n")] = '\0';
    printf("%s\n", choice_question);
    while(1){
        char client_choice[SIZE] = {0};
        if(fgets(client_choice, SIZE - 1, stdin) == NULL){
            perror("Nothing to send\nTry again:\n");
        }
        client_choice[strcspn(client_choice, "\n")] = '\0';
        int only_one = 0;
        if(strcmp(client_choice, "1") == 0 || strcmp(client_choice, "2") == 0 || strcmp(client_choice, "3") == 0){
            ssize_t s1 = send(client_sock, client_choice, strlen(client_choice), 0);
            if(s1 == -1){
                perror("Sendiong failed!\n");
                close(client_sock);
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(client_choice, "RESULT") == 0){
            ssize_t s1 = send(client_sock, client_choice, strlen(client_choice), 0);
            if(s1 == -1){
                perror("Send failed or Server Disconnected\n");
                close(client_sock);
                exit(EXIT_FAILURE);
            }
        }
        else{
            ssize_t s1 = send(client_sock, client_choice, strlen(client_choice), 0);
            if(s1 < 0){
                perror("Send failed! OR Server Disconnected\n");
                close(client_sock);
                exit(EXIT_FAILURE);
            }
        }
        char result_from_server[SIZE] = {0};
        ssize_t r2 = recv(client_sock, result_from_server, SIZE, 0);
        if(r2 <= 0){
            perror("Recv failed OR Server Disconnected!\n");
            close(client_sock);
            exit(EXIT_FAILURE);
        }
        else{
            printf("%s\n", result_from_server);
        }
    }
}
