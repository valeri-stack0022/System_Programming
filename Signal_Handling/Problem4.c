#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

char symbol;
void SIGINT_Handler(int signum)
{
    printf("Do tou really want to exit? (y/n) ");
    scanf(" %c", &symbol);
    if(symbol == 'y'){
        printf("Cleaning up resources...\n");
        exit(0);
    }
    else if(symbol == 'n'){
        return;
    }
}

int main()
{
    signal(SIGINT, SIGINT_Handler);
    while(1){
        sleep(1);
        printf("Working...\n");
    }

}
