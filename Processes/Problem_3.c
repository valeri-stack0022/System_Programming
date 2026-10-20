#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    pid_t PID = fork();
    if(PID < 0){
        printf("Child's creation failed!\n");
        exit(EXIT_FAILURE);
    }
    else if(PID == 0){
        sleep(5);
    }
    else{
        wait(NULL);
        printf("Parent -> Child's finished!\n");
    }

}
