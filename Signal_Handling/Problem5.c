#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

unsigned int seconds;

void SIGALRM_Handler(int signum)
{
    static int seconds1 = 0;
    seconds1 = seconds1 + seconds;
    if(seconds1 <= 10){
        printf("Elapsed time: %d seconds\n", seconds1);
    }
    if(seconds1 > 10){
        printf("Time is up!\n");
        exit(EXIT_SUCCESS);
    }
    alarm(seconds);
}

int main()
{
    printf("Input seconds: ");
    scanf("%d", &seconds);
    signal(SIGALRM, SIGALRM_Handler);
    alarm(seconds);
    while(1)
        pause();
}
