#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int status;
    pid_t ChildPID1 = fork();
    if(ChildPID1 < 0){
        printf("Child 1 failed!\n");
        exit(-1);
    }
    else if(ChildPID1 == 0){
        char *const args[] = {"ls", "-la", NULL};
        if(execv("/bin/ls", args) == -1){
            printf("Exec failed!\n");
            exit(-1);
        }
    }
    else{
        wait(&status);
        pid_t ChildPID2 = fork();
        if(ChildPID2 < 0){
            printf("Child 2 failed!\n");
            exit(-1);
        }
        else if(ChildPID2 == 0){
            char *const args[] = {"pwd", NULL};
            if(execv("/usr/bin/pwd", args)){
                printf("Exec failed!\n");
                exit(-1);
            }
        }
        else{
            wait(&status);
            printf("Two Childs are ended:\n");
        }
    }
}

