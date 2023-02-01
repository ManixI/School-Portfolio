#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <wait.h>
#include <unistd.h>

int randomGaussian(int mean, int stddev);
int check_if_free(int *chopstick, int thinker, int num_thinkers);
int take(int *chopstick, int thinker, int num_thinkers);
int setdown(int *chopstick, int thinker, int num_thinkers);


#define EATING 1
#define THINKING 2
#define HUNGRY 0

#define TAKEN 0
#define FREE 1

#define DEBUG 1


/*
typedef struct {
    int value = FREE;
    struct process *L;
} semaphore;
*
/* successive calls to randomGaussian produce integer return values */
/* having a gaussian distribution with the given mean and standard  */
/* deviation.  Return values may be negative.                       */

int randomGaussian(int mean, int stddev) {
    double mu = 0.5 + (double) mean;
    double sigma = fabs((double) stddev);
    double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
    double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
    if (rand() & (1 << 5)) 
        return (int) floor(mu + sigma * cos(f2) * f1);
    else            
        return (int) floor(mu + sigma * sin(f2) * f1);
}

int chopsticks[5] = {1,1,1,1,1};

int main(int argc, char *argv[]) {
    int num_thinkers = 5;
    //semaphore chopstick[num_thinkers];

    // make n forks
    for (int i = 0; i < num_thinkers; i++) {
        if(fork()==0){
            // do fork code here
            #if DEBUG
                printf("seat: %i\n", i);
            #endif
            // loop philosopher until done eating
            int time_eating = 0;
            while (time_eating < 100) {
                printf("Philosopher %i is thinking...\n", i+1);
                int t = randomGaussian(11,7);
                if (t < 0) {t = 0;}
                #if DEBUG
                    printf("thinker %i waiting time %i\n", i+1, t);
                #endif
                sleep(t);
                // when hungry check if chopsticks are free
                printf("Philosopher %i is hungry...\n", i+1);
                t = randomGaussian(9,3);
                if (t < 0) {t = 0;}
                #if DEBUG
                    printf("thinker %i waiting time %i\n", i+1, t);
                #endif
                while (check_if_free(chopsticks, i, num_thinkers)==TAKEN) {
                    sleep(1);
                }

                // pick up chopsticks and eat
                take(chopsticks, i, num_thinkers);
                printf("Philosopher %i is eating...\n", i+1);
                sleep(t);
                time_eating += t;
                // put down chopsticks, then go back to thinking
                setdown(chopsticks, i, num_thinkers);

            }
            //printf("%i\n", seat);
            exit(0);
        }
        
    }
    //semctl(,NULL, IPC_RMID)
    return 0;
}


int check_if_free(int *chopstick, int thinker, int num_thinkers) {
    int left, right;
    if (thinker == 0) { left = chopstick[num_thinkers-1];
    } else { left = chopstick[thinker-1]; }
    if (thinker == num_thinkers-1) { right = chopstick[0];
    } else { right = chopstick[thinker]; }
    #if DEBUG
        printf("thinker: %i, num_thinkers: %i, right: %i, left: %i\n", thinker+1, num_thinkers, right, left);
    #endif

    if (left == FREE && right == FREE) { return FREE; }
    return TAKEN;
}   

int take(int *chopstick, int thinker, int num_thinkers) {
    int left, right;
    if (thinker == 0) { chopstick[num_thinkers-1] = TAKEN;
    } else { chopstick[thinker-1] = TAKEN; }
    if (thinker == num_thinkers-1) { chopstick[0] = TAKEN;
    } else { chopstick[thinker] = TAKEN; }
    #if DEBUG
        printf("thinker: %i, num_thinkers: %i, right: %i, left: %i\n", thinker+1, num_thinkers, right, left);
    #endif

    return 0;
}   

int setdown(int *chopstick, int thinker, int num_thinkers) {
    int left, right;
    if (thinker == 0) { chopstick[num_thinkers-1] = FREE;
    } else { chopstick[thinker-1] = FREE; }
    if (thinker == num_thinkers-1) { chopstick[0] = FREE;
    } else { chopstick[thinker] = FREE; }
    #if DEBUG
        printf("thinker: %i, num_thinkers: %i, right: %i, left: %i\n", thinker+1, num_thinkers, right, left);
    #endif

    return 0;
}   