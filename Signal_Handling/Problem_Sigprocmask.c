#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main()
{
    int i = 0;
    sigset_t mask, prevmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    sigprocmask(SIG_BLOCK, &mask, &prevmask);
        printf("Hehe, You can't pause me.\n");
        sleep(5);
    sigprocmask(SIG_UNBLOCK, &mask, &prevmask);
    while(1){
        sleep(1);
        printf("Working...\n");
    }
}
