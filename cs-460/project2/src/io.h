#ifndef __IO_H_
#define __IO_H_

struct burst;

// linked list of io bursts in io queue
typedef struct io_burst {
    int time;
    int burst_proc_id;  // for debug, not actually used
    struct io_burst *left;
    struct io_burst *right;
} IO_BURST;

void io_queue_init();

void *io_thread(void *unused);

int pop_io_queue();

void push_io_queue(struct burst *in);

void print_io_queue();

#endif