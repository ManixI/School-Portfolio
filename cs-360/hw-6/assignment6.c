#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>


// set to 1 for debug output
#define DEBUG 1

#if DEBUG
    #include <time.h>
    #include <signal.h>
#endif

#define FREE 1
#define IN_USE 0

// shared chopstick array, global so all threads can see it;
int *chopsticks;
pthread_mutex_t **mutex;
pthread_mutex_t *mut0, *mut1, *mut2, *mut3, *mut4;
int num_thinkers;
int time_eating;

// for debuging
// print stick states on alarm, then schedule new alarm 5 sec in the future
void sig_handler (int signum) {
    printf("stick states: ");
    for (int i = 0; i < num_thinkers; i++) {
        printf("%i, ",chopsticks[i]);
    }
    printf("\n");
    alarm(5);
}

// RNG funcrion provided in class
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

// philosopher function
// each thread will call this once
// paramaters shared accross all philosophers are set via global variables num_thinkers and time_eating
// takes a void pointer that is cast to an int to get thinker's seat num
int philosopher(void *spot) {
    // set rng paramaters
    int mean_eat = 9;
    int std_eat = 3;
    int mean_think = 11;
    int std_think = 7;

    // get seat num from void pointer
    int seat = *(int *)spot;
    int *ch_sticks = chopsticks;

    // find relevent chopsticks
    int left_pos =  seat;
    int right_pos = ((seat + 1) % num_thinkers);

    #if DEBUG
        printf("thinker %i left: %i, right: %i\n",seat, left_pos, right_pos);
    #endif

    int time = 0;
    int t;
    // loop until thinker has eaten long enough to get up
    while (time < time_eating) {
        t = randomGaussian(mean_think, std_think);
        if (t < 0) { t = 0; }
        printf("Philosopher %i is thinking for %i seconds\n", seat+1, t);
        sleep(t);
        printf("Philosopher %i is hungry\n", seat+1);
        #if DEBUG
            printf("left chopstick %i state: %i, right chopstick %i state: %i\n",
                left_pos, ch_sticks[left_pos], right_pos, ch_sticks[right_pos]);
        #endif
        // if first seat, look left first, otherwise look right first
        if (seat == 0) {
            if ((pthread_mutex_lock(mutex[left_pos]))!=0) {
                perror("lock error: ");
            }
            if ((pthread_mutex_lock(mutex[right_pos]))!=0) {
                perror("lock error: ");
            }
        } else {
            if ((pthread_mutex_lock(mutex[right_pos]))!=0) {
                perror("lock error: ");
            }
            if ((pthread_mutex_lock(mutex[left_pos]))!=0) {
                perror("lock error: ");
            }
        }
        printf("ss\n");
        t = randomGaussian(mean_eat, std_eat);
        if (t < 0) { t = 0; }
        printf("Philosopher %i eating for %i seconds\n", seat, t);
        sleep(t);
        time += t;
        if ((pthread_mutex_unlock(mutex[right_pos]))!=0) {
                perror("lock error: ");
            }
            if ((pthread_mutex_unlock(mutex[left_pos]))!=0) {
                perror("lock error: ");
            }
        #if DEBUG
            printf("thinker %i relinquishing left stick %i and right stick %i\n",seat, left_pos, right_pos);
            printf("\nthinker %i total eat time %i\n\n", seat, time);
        #endif
    }
    printf("Philosopher %i is done\n",seat+1);
    return 0;
}


int main (int argc, char *argv[]) {
    // default values
    num_thinkers = 5;
    time_eating = 5;

    #if DEBUG
        // debut code to print state of each chopstick every 5 seconds
        signal(SIGALRM, sig_handler);
        alarm(5);
    #endif

    // error handling code for taking arguments from cmd line
    /*
    if (argc < 2) {
        print("no philosophers entered, using default value of %i\n", num_thinkers);
    } else {
        num_thinkers = atoi(argv[1]);
        if (num_thinkers < 2) {
            printf("%i philosophers is not enough, using default value of 5\n", num_thinkers);
            num_thinkers = 5;
        }
    }
    if (argc < 3) {
        printf("no eat time provided, using default of %i seconds\n", time_eating)
    } else {
        time_eating = atoi(argv[2]);
        if (time_eating < 1) {
            printf("time eating %i is less then 1, using default value of 100\n", time_eating);
            time_eating = 100;
        }
    }
    */
    // allocate and initallize all chopsticks to free
    mut0 = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if ((pthread_mutex_init(mut0, NULL))!=0) {
            perror("mutex creation: ");
    }
    mut1 = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if ((pthread_mutex_init(mut1, NULL))!=0) {
            perror("mutex creation: ");
    }
    mut2 = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if ((pthread_mutex_init(mut2, NULL))!=0) {
            perror("mutex creation: ");
    }
    mut3 = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if ((pthread_mutex_init(mut3, NULL))!=0) {
            perror("mutex creation: ");
    }
    mut4 = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if ((pthread_mutex_init(mut4, NULL))!=0) {
            perror("mutex creation: ");
    }    
    printf("kk\n");
    mutex = (pthread_mutex_t **) calloc(num_thinkers, sizeof(pthread_mutex_t *));
    mutex[0] = mut0;
    mutex[1] = mut1;
    mutex[2] = mut2;
    mutex[3] = mut3;
    mutex[4] = mut4;


    
    #if DEBUG
        time_t start_time = time(NULL);
    #endif 

    int seating_chart[num_thinkers];
    for (int i = 0; i < num_thinkers; i++) {
        seating_chart[i] = i;
    }

    // array to track thread ids
    pthread_t **threads = (pthread_t **) calloc(num_thinkers, sizeof(pthread_t *));
    for (int i = 0; i < num_thinkers; i++) {
        #if DEBUG
            printf("thinker %i spawned at time %ld\n", i, time(NULL)-start_time);
        #endif
        threads[i] = (pthread_t *) calloc(1, sizeof(pthread_t));
    }

    for (int i = 0; i < num_thinkers; i++) {
        void *seat = (void *) &seating_chart[i];
        if (pthread_create(threads[i], NULL, (void *) philosopher, (void *) seat)!=0) {
            perror("thread creation error: ");
        }
    }

    // wait for all thinkers to finish eating
    for (int i = 0; i < num_thinkers; i++) {
        #if DEBUG
            printf("\nwaiting on thread %i\n\n",i);
        #endif
        if ((pthread_join(*threads[i], NULL))!=0) {
            perror("thread join error: ");
            return 3;
        }
    }

    free(chopsticks);
    for (int i = 0; i < num_thinkers; i++) {
        free(threads[i]);
    }
    for (int i = 0; i < num_thinkers; i++) {
        if ((pthread_mutex_destroy(mutex[i]))!=0) {
            perror("destroy mutex: ");
        }
        //free(mutex[i]);
    }
    free(mutex);
    free(threads);
    #if DEBUG
        printf("runtime: %ld\n",time(NULL)-start_time);
    #endif
    return 0;
}