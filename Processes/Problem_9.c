#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
    int status;
    pid_t PID = fork();
    if(PID < 0){
        printf("Child's failed!\n");
        exit(-1);
    }
    else if(PID == 0){
        char *const args[] = {"ls", "-la", NULL};
        if(execv("/bin/ls", args) == -1){
            printf("Exec failed!\n");
            exit(-1);
        }
    }
    else{
        wait(&status);
        printf("End!\n");
    }
}
