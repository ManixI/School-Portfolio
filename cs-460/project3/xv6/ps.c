#include "types.h"
#include "user.h"
#include "uproc.h"
#include "param.h"

int main(void) 
{
    struct uproc table[NPROC];
    int num_procs = getprocs(NPROC, table);
    if (num_procs == -1) {
        printf(1, "Error in getprocs\n");
        exit();
    }

    printf(1, "process id, parrent id, state, size, name\n");
    for (int i=0; i<num_procs; i++) {
        printf(1, "%d  %d  %s  %d  %s\n",
            table[i].pid, table[i].ppid, table[i].state, table[i].sz, table[i].name);
    }

    exit();
}