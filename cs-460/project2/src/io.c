#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <semaphore.h>

#include "io.h"
#include "global.h"
#include "queue.h"
#include "pcb.h"

sem_t io_queue_mutex;

#define DEBUG 0

// start and end of IO queue (linked list)
IO_BURST *start, *end = NULL;

// flag to track progress of this thread
int io_queue_done = 0;

void io_queue_init() {
    sem_init(&io_queue_mutex, 0, 1);
}

// IO thread function
void *io_thread(void *unused) {
    int next = pop_io_queue();
    #if DEBUG
        printf("entering IO loop\n");
    #endif
    while(!io_queue_done) {
        if (next != 0) {
            #if DEBUG
                printf("IO burst for %d seconds\n", next);
            #endif
            sleep_ms(next);
        }
        next = pop_io_queue();
        #if DEBUG
            printf("io loop\n");
        #endif
    }
    return NULL;
}

// get first element of io queue
int pop_io_queue() {
    if (file_read_done && cpu_queue_done && start == NULL) {
        io_queue_done = 1;
        return 0;
    }
    sem_wait(&io_queue_mutex);
    int out;
    if (start == NULL) {
        sem_post(&io_queue_mutex);
        return 0;
    } else
        out = start->time;
    if (start == end)
        end = NULL;
    start = start->right;
    if (start != NULL)
        start->left = NULL;
    sem_post(&io_queue_mutex);
    #if DEBUG
        printf("popped element with time %d from IO queue\n", out);
    #endif
    //free(cursor); // double free, fix later
    return out;
}

// for debug purposes
void print_io_queue() {
    IO_BURST *cursor = start;
    while (cursor != NULL) {
        printf("io burst: %d\n", cursor->time);
        cursor = cursor->right;
    }
}

// add element to io queue
void push_io_queue(BURST *in) {
    #if DEBUG
        printf("pushing io queue\n");
    #endif
    IO_BURST *new = (IO_BURST *) malloc(sizeof(IO_BURST));
    new->time = in->time;
    new->left = NULL;
    new->right = NULL;
    sem_wait(&io_queue_mutex);
    if (start == NULL) {
        start = new;
        end = new;
        sem_post(&io_queue_mutex);
        return;
    }
    end->right = new;
    new->left = end;
    end = new;
    sem_post(&io_queue_mutex);
    free(in);
}