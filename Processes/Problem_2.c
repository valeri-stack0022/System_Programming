#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

int main()
{
    unsigned int process_count = 3;
    int status;
    pid_t array_childPID[process_count];
    for(int i = 0; i < process_count; ++i){
        pid_t PID = fork();
        if(PID < 0){
            printf("Child's creation failed!\n");
            exit(EXIT_FAILURE);
        }
        else if(PID == 0){
            printf("Hello from Child process - process PID -> %d\n", getpid());
            sleep(i + 1);
            exit(0);
        }
        else{
            array_childPID[i] = PID;
            printf("Hello from Parent process\n");
            continue;
        }
    }
    pid_t First_ended_childs_PID = wait(&status);
    printf("First ended Child's PID -> %d and Status -> %d\n", First_ended_childs_PID, WEXITSTATUS(status));
    for(int i = 0; i < process_count; ++i){
        if(array_childPID[i] != First_ended_childs_PID){
            int flag = kill(array_childPID[i], SIGKILL);
        }
    }
    for(int i = 0; i < process_count - 1; ++i){
        wait(NULL);
    }
    exit(EXIT_SUCCESS);
}
