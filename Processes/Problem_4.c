#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int count = 0;
    pid_t PID = fork();
    if(PID < 0){
        printf("Child's creation failed!\n");
        exit(EXIT_FAILURE);
    }
    else if(PID == 0){
        while(1){
            printf("PID -> %d\n", getpid());
            sleep(1);
        }
    }
    else{
        sleep(3);
        kill(PID, SIGTERM);
        printf("SUCCESS!\n");
    }
}
