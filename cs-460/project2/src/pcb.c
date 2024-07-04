#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include "pcb.h"
#include "queue.h"
#include "global.h"

#define DEBUG 0

#define OUTFILE "data/out.txt"

int file_read_done = 0;

// given a file simulate procs added to PCB as described in file
void *gen_pcb_from_file(void* filename) {
    FILE *fp = fopen((char*)filename, "r");
    #if DEBUG
        printf("pcb revieved filename: %s\n", (char*)filename);
        perror("file error check: ");
    #endif

    char cmd[6] = "";
    int val = 0;

    int id = 0;
    int priority = 0;

    while(strcmp(cmd, "stop")) {
        fscanf(fp, "%s ", cmd);
        #if DEBUG
            printf("cmd: %s\n", cmd);
        #endif
        if (!strcmp(cmd, "sleep")) {
            // sleep command given
            #if DEBUG
                printf("Sleeping %dms\n", val);
            #endif
            fscanf(fp, "%d\n", &val);
            sleep_ms(val);
        } else if (!strcmp(cmd, "proc")) {
            // add new proc
            PCB *tmp = (PCB *) malloc(sizeof(PCB));
            tmp->start_bursts = NULL;
            tmp->last_burst = NULL;
            tmp->proc_id = id;
            tmp->duration = 0;

            fscanf(fp, "%d", &priority);
            tmp->priority = priority;

            // create list of CPU and IO burst times
            fscanf(fp, "%d", &val);
            #if DEBUG
                printf("priority: %d\n", priority);
                printf("num bursts: %d\n", val);
            #endif

            int upper_bound = val;
            // add bursts
            for (int i=0; i<upper_bound; i++) {
                fscanf(fp, "%d", &val);
                BURST *b = (BURST *) malloc(sizeof(BURST));
                b->left = tmp->last_burst;
                if (b->left)
                    b->left->right = b;
                b->right = NULL;
                tmp->last_burst = b;
                if (tmp->start_bursts == NULL)
                    tmp->start_bursts = b;
                if (i%2 == 0) {
                    b->type = CPU;
                } else {
                    b->type = IO;
                }
                b->time = val;
                tmp->duration += val;
                #if DEBUG
                    if (b->type == IO) 
                        printf("IO");
                    else
                        printf("CPU");
                    printf(" length: %d\n", b->time);
                #endif
            }
            #if DEBUG
                printf("pushing queue\n");
            #endif
            // get time added to queue
            clock_gettime(CLOCK_MONOTONIC, &tmp->ts_begin);
            push_queue(tmp);
            #if DEBUG
                struct pcb *test = pop_queue();
                printf("test push queue, popped elem %d\n", test->proc_id);
                push_queue(test);
            #endif
            id++;
        }
        #if DEBUG
            printf("end loop, id: %d\n", id);
        #endif
    }
    // flag tracking progress of this thread
    file_read_done = 1;
    #if DEBUG
        printf("file read done\n");
        printf("flags: file %d, io: %d, cpu: %d\n", file_read_done, io_queue_done, cpu_queue_done);
    #endif
    int *out = malloc(sizeof(int));
    *out = id+1;
    // return number of procs ecountered
    return (void*) out;
}

// get next burst in list
BURST *pop_burst(PCB *entry) {
    #if DEBUG
        printf("popping burst\n");
    #endif
    if (entry->start_bursts == NULL)
        return NULL;
    BURST *out = entry->start_bursts;
    if (entry->start_bursts->right == NULL) {
        entry->start_bursts = NULL;
        entry->last_burst = NULL;
    } else if (entry->start_bursts->right->right == NULL) {
        entry->last_burst = entry->start_bursts;
        entry->start_bursts = entry->start_bursts->right;
    } else {
        entry->start_bursts = entry->start_bursts->right;
    }
    return out;
}

// push burst to start of queue, only used for RR
void push_burst(PCB *ent, BURST *b) {
    b->right = ent->start_bursts;
    b->left = NULL;
    if (ent->start_bursts != NULL) {
        ent->start_bursts->left = b;
    } else {
        ent->last_burst = b;
    }
    ent->start_bursts = b;
}

// everything below this should be in a lib.c file

// helper function for sleeping x ms
void sleep_ms(int ms) {
    struct timespec remaining, request = {0, ms * 1000000};
    nanosleep(&request, &remaining);
}

// write output to file
void print_out(char *alg, char *infile, int num_procs, double total_turnaround, double total_wait) {
    // this happens for every alg, so easier to do it here then elsewhere
    total_turnaround *= 1000;
    total_wait *= 1000;

    FILE *fp = fopen(OUTFILE, "w");
    fprintf(fp, "Input File Name: %s\n", infile);
    fprintf(fp, "CPU Scheduling Alg: %s\n", alg);
    fprintf(fp, "Throughput: %.3f procs / ms\n", num_procs / total_turnaround);
    fprintf(fp, "Avg. Turnaround Time: %.3fms\n", total_turnaround / num_procs);
    fprintf(fp, "Avg. Waiting time in R que: %.3fms\n", total_wait / num_procs);
}