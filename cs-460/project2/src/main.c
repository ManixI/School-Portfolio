#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h> 
#include <time.h> 
#include <sys/times.h> 
#include <unistd.h>

#include "pcb.h"
#include "queue.h"
#include "io.h"
#include "schedulers.h"
#include "global.h"

#define DEBUG 0

#define OUTFILE "data/out.txt"


// TODO: queues must be doubly linked list

int main(int argc, char *argv[]) {
    #if DEBUG
    printf("%d\n", argc);
    for (int i=0; i < argc; i++) {
        printf("%s ",argv[i]);
    }
    printf("\n");
    #endif

    // sanity check number of args provided
    if (argc < 5) {
        printf("Error: incorect number of arguments.\nUsage is prog -alg [FIFO|SJF|PR] -input [file name] -quant [quant size]\n");
        return 1;
    }

    char *alg, *file, *quant = NULL;

    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "-input"))
            file = argv[++i];
        if (!strcmp(argv[i], "-alg"))
            alg = argv[++i];
        if (!strcmp(argv[i], "-quant")) 
            quant = argv[++i];
    }
    #if DEBUG
        printf("file is: %s\n", file);
        printf("alg is :%s\n", alg);
        if (quant!=NULL)
            printf("quant is: %s\n", quant);
    #endif

    // init queue mutexes
    io_queue_init();
    queue_init();

    // make treads
    pthread_t file_id, io_id;
    pthread_create(&io_id, NULL, io_thread, NULL);
    pthread_create(&file_id, NULL, gen_pcb_from_file, (void*)file);

    // default quant size if not provided
    int q = 10;
    if (quant)
        q = atoi(quant);
    struct schedule_out *times = scheduler(alg, q);

    // join threads
    void *proc_count_void;
    pthread_join(file_id, &proc_count_void);
    int proc_count = *(int*)proc_count_void ;
    free(proc_count_void);
    pthread_join(io_id, NULL);

    print_out(alg, file, proc_count, times->total_turnaround, times->total_wait);
    free(times);
}