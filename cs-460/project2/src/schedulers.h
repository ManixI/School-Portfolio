#ifndef __SCHEDULE_H_
#define __SCHEDULE_H_

// so the scheduler can return 2 ints instead of just one
struct schedule_out {
    double total_turnaround;
    double total_wait;
};

struct schedule_out *scheduler(char *alg, int quant);

#endif