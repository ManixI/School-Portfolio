#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "queue.h"
#include "pcb.h"
#include "io.h"
#include "global.h"
#include "schedulers.h"

#define DEBUG 0

// flag to track progress of this thread
int cpu_queue_done = 0;

typedef struct timespec TIMESPEC;


struct timespec diff_times(struct timespec *a, struct timespec *b);
void add_delta_from_now(double *total, TIMESPEC *start);

struct schedule_out *scheduler(char *alg, int quant) {
    // create thread reading processes from file
    #if DEBUG
        printf("starting fifo\n");
    #endif

    struct schedule_out *out = (struct schedule_out *) malloc(sizeof(struct schedule_out));

    PCB *active;
    BURST *next_b;
    int time_in_quant = 0;
    #if DEBUG
        printf("file flag: %d, cpu flag: %d, IO flag: %d\n", file_read_done, cpu_queue_done, io_queue_done);
    #endif
    // while all 3 threads are active
    while (!file_read_done || !io_queue_done || !cpu_queue_done) {
        // pop PCB
        #if DEBUG
            printf("popping from queue\n");
        #endif
        // pop based on alg
        if (!strcmp(alg, "PR"))
            active = pop_queue_priority();
        else if (!strcmp(alg, "SJF"))
            active = pop_queue_shortest();
        else
            active = pop_queue();
        if (active != NULL) {
            add_delta_from_now(&out->total_wait, &active->ts_begin);
        }
        
        #if DEBUG
            if (active != NULL)
                printf("popped proc %d\n", active->proc_id);
        #endif
        // if queue is emtpy
        if (active == NULL) {
            #if DEBUG
                printf("queue empty\n");
            #endif
            if (file_read_done)
                cpu_queue_done = 1;
            continue;
        }
        // run PCB to compleation
        next_b = pop_burst(active);
        time_in_quant = quant;
        while (next_b != NULL) {
            if (!strcmp(alg, "RR")) {
                if (next_b->type == CPU) {
                    // run burst to compleation and get next burst if time left in quant
                    if (next_b->time < time_in_quant) {
                        time_in_quant -= next_b->time;
                        sleep_ms(next_b->time);
                        free(next_b);
                    } else {
                        // run time remaining in burst and re-add remaining burst to queue
                        sleep_ms(time_in_quant);
                        next_b->time -= time_in_quant;
                        push_burst(active, next_b);
                        push_queue(active);
                        break;
                    }
                } else {
                    // don't count io burst time in quant
                    push_io_queue(next_b);
                }
                next_b = pop_burst(active);
            } else {
                // not RR alg
                if (next_b->type == CPU) {
                    #if DEBUG
                        printf("CPU burst %d ms\n", next_b->time);
                    #endif
                    sleep_ms(next_b->time);
                    free(next_b);
                } else {
                    push_io_queue(next_b);
                }
                next_b = pop_burst(active);
                #if DEBUG
                    printf("burst loot\n");
                #endif
            }
        }
        add_delta_from_now(&out->total_turnaround, &active->ts_begin);
        #if DEBUG
            printf("cpu loop\n");
            printf("file flag: %d, cpu flag: %d, IO flag: %d\n", file_read_done, cpu_queue_done, io_queue_done);
        #endif
    }
    return out;
}

// helper function to increment time delta
void add_delta_from_now(double *total, TIMESPEC *start) {
    TIMESPEC tmp;
    clock_gettime(CLOCK_MONOTONIC, &tmp);
    tmp = diff_times(&tmp, start);
    *total += tmp.tv_sec;
    *total += (tmp.tv_nsec) / 1000000000.0;
}

// diff of times
struct timespec diff_times(struct timespec *a, struct timespec *b) {
    struct timespec out;
    if (a->tv_nsec - b->tv_nsec < 0) {
        out.tv_sec = a->tv_sec - b->tv_sec-1;
        out.tv_nsec = a->tv_nsec - b->tv_nsec + 1000000000;
    } else {
        out.tv_sec = a->tv_sec - b->tv_sec;
        out.tv_nsec = a->tv_nsec - b->tv_nsec;
    }
    return out;
}