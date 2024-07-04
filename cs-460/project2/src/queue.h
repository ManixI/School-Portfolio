#ifndef __QUEUE_H_
#define __QUEUE_H_

struct pcb;

// linked list of PCB elements
typedef struct queue {
    struct pcb *element;
    struct queue *left;
    struct queue *right;
} QUEUE;


void print_queue();

void push_queue(struct pcb *elem);

void queue_init();

struct pcb* pop_queue();

struct pcb* pop_queue_priority();

struct pcb* pop_queue_shortest();


#endif