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
        perror("Child's failed!\n");
        exit(EXIT_FAILURE);
    }
    else if(PID == 0){
        system("mpg123 /home/valer/Downloads/'OST Game of Thrones - Main Theme.mp3'");
        alarm(10);
    }
    else{
        sleep(11);
        exit(0);
    }
}
