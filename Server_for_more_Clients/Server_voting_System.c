#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <sys/select.h>

#define IP "127.0.0.1"
#define PORT 9999
#define MAX_CLIENTS 5
#define SIZE 512

static int count_C = 0;
static int count_CPP = 0;
static int count_Python = 0;

char *choice_func(int number){
    char *choice = NULL;
    switch(number){
        case 1:{
            choice = "C";
            ++count_C;
            break;
        }
        case 2:{
            choice = "C++";
            ++count_CPP;
            break;
        }
        case 3:{
            choice = "Python";
            ++count_Python;
            break;
        }
        default:{
            choice = "Not found!";
            break;
        }
    }
    return choice;
}

int main()
{
    int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock_fd == -1){
        perror("Socket creation failed!\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &addr.sin_addr);

    int opt = 1;
    if(setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
        perror("Option for server socket failed!\n");
        exit(EXIT_FAILURE);
    }

    int b = bind(server_sock_fd, (struct sockaddr*)&addr, sizeof(addr));
    if(b == -1){
        perror("Bind failed!\n");
        exit(EXIT_FAILURE);
    }

    int l = listen(server_sock_fd, SOMAXCONN);
    if(l == -1){
        perror("Listen is failed!\n");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on Port -> %d\n", PORT);

    int clients_sockets[MAX_CLIENTS];
    for(int i = 0; i < MAX_CLIENTS; ++i){
        clients_sockets[i] = -1;
    }

    while(1){
        fd_set readfds;
        FD_ZERO(&readfds);
        //Ավելացնում ենք գլխավոր սոկետը բազմության մեջ
        FD_SET(server_sock_fd, &readfds);
        int max_fd = server_sock_fd;

        //Ավելացնում ենք բոլոր ակտիվ հաճախորդների սոկետները
        for(int i = 0; i < MAX_CLIENTS; ++i){
            if(clients_sockets[i] != -1){
                FD_SET(clients_sockets[i], &readfds);
                if(clients_sockets[i] > max_fd){
                    max_fd = clients_sockets[i];
                }
            }
        }
        //Call Select for sleep
        int active_clients = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if(active_clients < 0){
            perror("Select failed!\n");
            close(server_sock_fd);
            exit(EXIT_FAILURE);
        }

        //Ստուգում ենք արդյոք նոր միացման հայտ կա Լսող սոկետին
        if(FD_ISSET(server_sock_fd, &readfds)){
            struct sockaddr_in clients_addr;
            memset(&clients_addr, 0, sizeof(clients_addr));
            socklen_t length = sizeof(clients_addr);
            int new_sock_fd = accept(server_sock_fd, (struct sockaddr*)&clients_addr, &length);
            if(new_sock_fd == -1){
                perror("Accept is failed!\n");
            }
            else{
                int added = 0;
                for(int i = 0; i < MAX_CLIENTS; ++i){
                    if(clients_sockets[i] == -1){
                        clients_sockets[i] = new_sock_fd;
                        added = 1;
                        printf("New Client Connected: Client %d\n", new_sock_fd);

                        char choice_question[SIZE] = "Please enter your choice: 1) C | 2) C++ | 3) Python | Or print RESULT for result choices: ";
                        ssize_t s1 = send(clients_sockets[i], choice_question, strlen(choice_question), 0);
                        if(s1 == -1){
                            perror("Sending failed!\n");
                            close(clients_sockets[i]);
                        }
                        break;
                    }
                }
                if(!added){
                    printf("Max clients reached! Rejecting Connection\n");
                    close(new_sock_fd);
                }
            }
        }
        //Ստուգում ենք Client֊ներին արդյոք իրենցից տվյալ եկել է՞
        for(int i = 0; i < MAX_CLIENTS; ++i){
            int cfd = clients_sockets[i];
            if(cfd != -1 && FD_ISSET(cfd, &readfds)){
                char recving_choice[SIZE] = {0};
                ssize_t r1 = recv(cfd, recving_choice, SIZE, 0);
                if(r1 < 0){
                    perror("Recv failed!\n");
                    close(cfd);
                }
                else if(r1 == 0){
                    printf("Client %d disconnected!\n", clients_sockets[i]);
                    close(cfd);
                    clients_sockets[i] = -1;
                }
                else{
                    recving_choice[strcspn(recving_choice, "\n")] = '\0';
                    if(strcmp(recving_choice, "1") == 0 || strcmp(recving_choice, "2") == 0 || strcmp(recving_choice, "3") == 0){
                        int choice = 0;
                        sscanf(recving_choice, "%d", &choice);
                        choice_func(choice);
                        char accepted[SIZE] = "Voice Accepted!";
                        ssize_t s2 = send(cfd, accepted, strlen(accepted), 0);
                        if(s2 == -1){
                            perror("Send failed!\n");
                            clients_sockets[i] = -1;
                        }
                    }
                    else if(strcmp(recving_choice, "RESULT") == 0){
                        char RESULT_for_client[SIZE] = {0};
                        sprintf(RESULT_for_client, "\nServer:\nC: %d\nC++: %d\nPython: %d", count_C, count_CPP, count_Python);
                        ssize_t s2 = send(clients_sockets[i], RESULT_for_client, strlen(RESULT_for_client), 0);
                        if(s2 == -1){
                            perror("Send failed OR Client Disconnected\n");
                            close(clients_sockets[i]);
                            clients_sockets[i] = -1;
                        }
                    }
                    else{
                        char error_messege_for_client[SIZE] = "Not found!";
                        ssize_t s2 = send(clients_sockets[i], error_messege_for_client, strlen(error_messege_for_client), 0);
                        if(s2 == -1){
                            perror("Send failed OR Client Disconnected!\n");
                            close(clients_sockets[i]);
                            close(clients_sockets[i]);
                            clients_sockets[i] = -1;
                        }
                    }
                }
            }
        }
    }


}