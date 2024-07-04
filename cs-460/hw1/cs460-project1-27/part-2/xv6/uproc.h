#ifndef UPROC_H
#define UPROC_H

struct uproc{
    int pid;
    int ppid;
    int state;
    uint sz;
    char name[16];
};

#endif