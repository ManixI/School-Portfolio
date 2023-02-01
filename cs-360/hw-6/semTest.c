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

void do_thing(int *pnum, int num_children, int child_num);

typedef struct {
    int value;
    struct process *L;
} semaphore;

int shmid, *shm_ptr;
int *rp;

int main(int argc, char *argv[]) {
    int num_children = 4;
    pid_t child[num_children];

    // shared memory
    shmid = shmget(IPC_PRIVATE, num_children*sizeof(int),(IPC_CREAT | 0666));
    if((shm_ptr = (int*)shmat(shmid,(char*)0, 0))<0) {
        perror("shmat failed");
    }

    

    // creat n children
    for (int i = 0; i < num_children; i++) {
        if ((child[i] = fork()) == 0) {
            #if DEBUG
                printf("child %i\n", i);
            #endif
            do_thing(shm_ptr, num_children, i);
            wait(&shm_ptr[(i+1)%num_children]);
            //printf("child %i printing next num %i\n",i,shm_ptr[(i+1)%num_children]);
            exit(0);
        }
    }


    printf("waiting for children to finish\n");
    // wait for all n children
    for (int i = 0; i < num_children; i++) {
        wait(NULL);
    }
    printf("children done:\n");
    for (int i = 0; i < num_children; i++) {
        //printf("a\n");
        printf("%i\n", shm_ptr[i]);
    }

    shmctl(shmid, IPC_RMID, NULL);



    /*

    key_t key = 1;
    int semid;
    struct sembuf sb = {0,-1,0};

    semid = semget(key, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    if (semclt(semid, 0, SETVAL, 1) == -1) {
        perror("semctl");
        exit(1);
    }

    int arr[3] = {1,1,1};

    if (fork()) {

    } else {

    }

    printf("%i %i %i\n", arr[0],arr[1],arr[2]);
    


    semctl(semid, 0, IPC_RMID);
    return 0;
    */

    /*
    semaphore sem;
    int arr[3] = {1,1,1};

    if (fork()) {
        sleep(1);
        printf("parent waiting\n");
        wait(&sem.value);
        printf("parent going\n");
        sleep(1);
        arr[1] = arr[0]+arr[1];
        signal(sem.value);

    } else {
        printf("sem: %i\n",sem.value);
        wait(&sem.value);
        sem.value = 0;
        printf("child\n");
        arr[0] = 3;
        signal(sem.value);
        wait(&sem.value);
        arr[2] = arr[1]+arr[2];
        exit(0);

    }
    printf("%i %i %i\n", arr[0],arr[1],arr[2]);
    return 0;
    */
}

void do_thing(int pnum[], int num_children, int child_num) {
    if (child_num == 0) {
        pnum[child_num] = 1;
        #if DEBUG
            printf("pnum[i] %i\n", pnum[child_num]);
        #endif
    } else {
        #if DEBUG
            printf("child num %i\n", child_num);
        #endif
        while(pnum[child_num-1] == 0) {
            sleep(0.01);
        }
        pnum[child_num] = pnum[child_num-1] * 2;
        #if DEBUG
            printf("pnum[i] %i\n", pnum[child_num]);
        #endif
    }
}