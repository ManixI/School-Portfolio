#ifndef __PCB_H_
#define __PCB_H_

// linked list of PCBs
typedef struct pcb {
    int proc_id;
    int priority;
    int duration;
    struct timespec ts_begin, ts_end;
    struct burst *start_bursts;
    struct burst *last_burst;
} PCB;

enum burstType {
    IO,
    CPU,
};

// linked list of bursts, held by parent PCB
typedef struct burst {
    enum burstType type;
    int time;
    struct burst *left;
    struct burst *right;
} BURST;


void *gen_pcb_from_file(void* filename);

BURST* pop_burst(PCB *entry);

void push_burst(PCB *ent, BURST *b);

void sleep_ms(int ms);

void print_out(char *alg, char *infile, int num_procs, double total_turnaround, double total_wait);

#endif