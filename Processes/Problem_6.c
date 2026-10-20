#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid = fork();
    if(pid == -1){
        perror("Child's failed!");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        if(execl("/usr/bin/echo", , NULL) == -1){
            perror("Execl is failed!");
            exit(EXIT_FAILURE);
        }
    }
    else{
        waitpid(pid, NULL, 0);
        exit(EXIT_SUCCESS);
    }

}
