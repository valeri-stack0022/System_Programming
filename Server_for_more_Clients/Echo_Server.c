#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>

#define IP "127.0.0.1"
#define PORT 8888
#define MAX_CLIENTS 4 //Listen socket with client's sockets
#define SIZE 512

int main()
{
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1){
        perror("Socket creation is failed!\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &addr.sin_addr);

    int opt = 1;
    int res_opt = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(res_opt == -1){
        perror("Error Option\n");
        exit(EXIT_FAILURE);
    }

    int b = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    if(b == -1){
        perror("Bind is failed!\n");
        exit(EXIT_FAILURE);
    }

    int l = listen(server_sock, SOMAXCONN);
    if(l == -1){
        perror("Listen is failed!\n");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d\n", PORT);

    //Struct creation for File Descriptors...
    struct pollfd fds[MAX_CLIENTS + 1];
    memset(&fds, 0, sizeof(fds));
    fds[0].fd = server_sock;
    fds[0].events = POLLIN;

    //Մնացածը նախօրոք լցնում ենք ֊1֊երով
    for(int i = 1; i <= MAX_CLIENTS + 1; ++i){
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }

    while(1){
        //Կանչում ենք poll infinite timeout-ով
        int active_socket_count = poll(fds, MAX_CLIENTS + 1, -1);
        if(active_socket_count == -1){
            perror("Polling is failed!\n");
            exit(EXIT_FAILURE);
            break;
        }

        // Ստուգում ենք արդյոք կա միացում լսող(Գլխավոր) սոկետին?
        if(fds[0].revents & POLLIN){
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            socklen_t length = sizeof(client_addr);
            int new_sock_fd = accept(server_sock, (struct sockaddr*)&client_addr, &length);
            if(new_sock_fd == -1){
                perror("Accepting is failed!\n");
            }
            else{
                int added = 0;
                for(int i = 1; i <= MAX_CLIENTS; ++i){
                    if(fds[i].fd == -1){
                        fds[i].fd = new_sock_fd;
                        added = 1;
                        printf("New client Connected (fd = %d)!\n", new_sock_fd);
                        char sending_string[SIZE] = {0};
                        sprintf(sending_string, "Client %d | Send your string -> ", fds[i].fd);
                        ssize_t s1 = send(fds[i].fd, sending_string, strlen(sending_string), 0);
                        if(s1 == -1){
                            perror("Sending is failed!\n");
                            close(fds[i].fd);
                        }
                        break;
                    }
                }
                if(!added){
                    printf("Max clients reached. Rejecting connection!\n");
                    close(new_sock_fd);
                }
            }
        }
        //Ստուգում ենք արդեն միացված client-ներին
        for(int i = 1; i <= MAX_CLIENTS; ++i){
            if(fds[i].fd == -1){
                continue;
            }
            if(fds[i].revents & POLLIN){
                char recving_string[SIZE] = {0};
                ssize_t r1 = recv(fds[i].fd, recving_string, SIZE, 0);
                if(r1 <= 0){
                    perror("Recv is failed!\n");
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
                else{
                    recving_string[strcspn(recving_string, "\n")] = '\0';
                    ssize_t s2 = send(fds[i].fd, recving_string, strlen(recving_string), 0);
                    if(s2 == -1){
                        perror("Sending is failed!\n");
                        close(fds[i].fd);
                    }
                }
            }
        }
    }

}