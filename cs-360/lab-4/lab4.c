#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>

char *str;
int i = 0;

void got_sig(int sig) {
    printf("%c\n",str[i]);
    i++;
    fflush(stdout);
    //exit(0);
}



int main(int argc, char *argv[]) {
    str = (char *) calloc(argc, sizeof(char));
    strcpy(str, argv[1]);
    ualarm(999999,999999);
    signal(SIGALRM, got_sig);

    while(1) {
        if (i == argc+1) {
            return 0;
        }
    }
    return 0;   
}