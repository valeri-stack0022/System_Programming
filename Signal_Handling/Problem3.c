#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void handler(int signal){
    if(signal == SIGUSR1){
        printf("Child: Ping!\n");
    }
    else if(signal == SIGUSR2){
        printf("Parent: Pong!\n");
    }
}

int main()
{
    int status;
    pid_t PID = fork();
    if(PID < 0){
        perror("Child's failed!\n");
        exit(-1);
    }
    else if(PID == 0){
        signal(SIGUSR1, handler);
        while(1){
            pause();
            sleep(1);
            kill(getppid(), SIGUSR2);
        }
    }
    else{
        signal(SIGUSR2, handler);
        for(int i = 0; i < 5; ++i){
            sleep(1);
            kill(PID, SIGUSR1);
            pause();
        }
        kill(PID, SIGTERM);
    }
}
/*Ichpes anel vor erku arandzin process irar sms uxarken voch te parenty childin
*/
