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

#define DEBUG 1

int shmid_chopsticks, *shm_ptr_chopsticks;

void philosopher (int pnum[], int num_children, int child_num);


int main(int argc, char *argv[]) {

    pid_t child[num_children];

    // error handling
    if (argc < 2) {
        printf("need the number of philosophers\n");
        return 1;
    }
    if (num_thinkers = atoi(argv[1]) < 2) {
        printf("need more philosophers\n");
        return 2;
    }

    // initalize shared memory
    shmid_chopsticks = shmget(IPC_PRIVATE, num_thinkers*sizeof(int),(IPC_CREAT | 0666));
    if((shm_ptr_chopsticks = (int*)shmat(shmid,(char*)0, 0))<0) {
        perror("shmat failed");
    }

    // initalize values for chopstick array
    for (int i = 0; i < num_children; i++) {
        shm_ptr_chopsticks[i] = 1;
    }

    // make n philosophers
    for (int i = 0; i < num_children; i++) {
        if ((child[i] = fork()) == 0) {
            #if DEBUG
                printf("child %i\n", i);
            #endif
            philosopher(shm_ptr_chopsticks, num_thinkers, i);
            wait(&shm_ptr[(i+1)%num_children]);
            //printf("child %i printing next num %i\n",i,shm_ptr[(i+1)%num_children]);
            exit(0);
        }
    }

    // free shared memory
    shmctl(shmid, IPC_RMID, NULL);
}

void philosopher (int pnum[], int num_children, int child_num) {

}