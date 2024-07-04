#include <semaphore.h>
#include <malloc.h>

#include "queue.h"
#include "pcb.h"

#define INIT_QUE_SIZE 255

sem_t queue_mutex;

QUEUE *g_start = NULL; 
QUEUE *g_end = NULL;

#define DEBUG 0

void queue_init() {
    sem_init(&queue_mutex, 0, 1);
}

// add PCB to queue
void push_queue(struct pcb *elem) {
    #if DEBUG
        printf("pushing elem %d\n", elem->proc_id);
    #endif
    QUEUE *new = (QUEUE *) malloc(sizeof(QUEUE));
    new->left = NULL;
    new->right = NULL;
    new->element = elem;
    sem_wait(&queue_mutex);
    if (g_end != NULL) {
        new->left = g_end;
        new->left->right = new;
    }
    if (g_start == NULL) {
        g_start = new;
    }
    g_end = new;
    sem_post(&queue_mutex);
}

// get pcb from queue
struct pcb* pop_queue() {
    sem_wait(&queue_mutex);
    if (g_start == NULL) {
        #if DEBUG
            printf("queue empty NULL\n");
        #endif
        sem_post(&queue_mutex);
        return NULL;
    }
    PCB *out = g_start->element;
    QUEUE *cursor = g_start;
    g_start = cursor->right;
    if (cursor->right != NULL) {
        #if DEBUG
            printf("out: %d\n", cursor->element->proc_id);
            printf("out: %d\n", cursor->right->element->proc_id);
        #endif
        cursor->right->left = NULL; // causing segfault
    } 
    else 
        g_end = NULL;
    sem_post(&queue_mutex);
    free(cursor);
    return out;
}

// get pcb with highest prioity from queue
// priority ranked largest to smallest
// will return first if multiple with same priority are in queue
struct pcb* pop_queue_priority() {
    sem_wait(&queue_mutex);
    if (g_start == NULL) {
        sem_post(&queue_mutex);
        return NULL;
    }
    QUEUE *out = g_start;
    QUEUE *cursor = g_start;

    // find highest priority in queue;
    while (cursor != NULL) {
        if (cursor->element->priority > out->element->priority)
            out = cursor;
        cursor = cursor->right;
    }

    // sort out nebhors of returned pcb
    if (out->left && out->right) {
        out->left->right = out->right;
        out->right->left = out->left;
    } else if (out->left && !out->right) {
        out->left->right = NULL;
        g_end = out->left;
    } else if (!out->left && out->right) {
        out->right = NULL;
        g_start = out->right;
    } else if (!out->right && !out->left) {
        g_end = NULL;
        g_start = NULL;
    }
    sem_post(&queue_mutex);
    PCB *o = out->element;
    free(out);
    return o;
}


// get element with shortest combine CPU and IO burst time from queue
// if tie in queue, will return first one encountered of the tie
struct pcb* pop_queue_shortest() {
    sem_wait(&queue_mutex);
    if (g_start == NULL) {
        sem_post(&queue_mutex);
        return NULL;
    }
    QUEUE *out = g_start;
    QUEUE *cursor = g_start;

    // find highest priority in queue;
    while (cursor != NULL) {
        if (cursor->element->duration < out->element->duration)
            out = cursor;
        cursor = cursor->right;
    }

    // sort out nebhors of returned pcb
    if (out->left && out->right) {
        out->left->right = out->right;
        out->right->left = out->left;
    } else if (out->left && !out->right) {
        out->left->right = NULL;
        g_end = out->left;
    } else if (!out->left && out->right) {
        out->right = NULL;
        g_start = out->right;
    } else if (!out->right && !out->left) {
        g_end = NULL;
        g_start = NULL;
    }
    sem_post(&queue_mutex);
    PCB *o = out->element;
    free(out);
    return o;
}

// for debug purposes
void print_queue() {
    QUEUE *cursor = g_start;
    while (cursor != NULL) {
        printf("id: %d, priority: %d\n",cursor->element->proc_id, cursor->element->priority);
        cursor = cursor->right;
    }
}