#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>


void got_sig(int sig) {
    printf("b\n");
    fflush(stdout);
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, got_sig);
    while(1) {
        sleep(1);
        printf("a\n");
        fflush(stdout);
    } 
    return 0;   
}