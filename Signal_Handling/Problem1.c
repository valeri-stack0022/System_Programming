#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void handler(int signum){
    printf("Your signal is worked!\n");
    exit(EXIT_SUCCESS);
}

int main()
{
    int status;
    pid_t PID = fork();
    if(PID < 0){
        perror("Child's failed!\n");
        exit(EXIT_FAILURE);
    }
    else if(PID == 0){
        signal(SIGUSR1, handler);
        pause();
    }
    else{
        sleep(1);
        kill(PID, SIGUSR1);
        pid_t child_PID = wait(&status);
    }
}
