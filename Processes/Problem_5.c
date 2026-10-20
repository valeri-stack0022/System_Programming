#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pid = fork();
    if(pid == -1){
        perror("Child's failed");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        char *const args[] = {"ps", "-ef", NULL};
        if(execv("/bin/ps", args) == -1){
            perror("Exec failed!");
            exit(EXIT_FAILURE);
        }
    }
    else{
        waitpid(pid, NULL, 0);
        exit(EXIT_SUCCESS);
    }

}
