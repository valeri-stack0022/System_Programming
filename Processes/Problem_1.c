#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pid = fork();
    int status = -89;
    if(pid > 0){
        printf("hello from Parent process, childs PID -> %d\n", pid);
        wait(&status);
        printf("Status of Child -> %d\n", status);
        printf("\n");
        return 0;
}
    else if(pid < 0){
        printf("Child's failed!\n");
    }
    else{
        printf("hello from Child procces, return value of fork in child process -> %d\n", pid);
        printf("Parent's PID in Child -> %d\n", getppid());
        printf("\n");
        return 0;
   }

}
