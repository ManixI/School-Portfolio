#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "uproc.h"
#include "spinlock.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int toggletrace = 1;

int
sys_toggletrace(void)
{
  if (toggletrace == 0) {
    toggletrace = 1;
  } else {
    toggletrace = 0;
  }
  return 0;
}

#define DEBUG_GETPROCS 0

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable2; // this is very dumb

int
sys_getprocs() {
  int max;
  struct uproc *table = 0;
  if (argint(0, &max) < 0)
    return -1;
  if (argptr(1, (char**)&table, sizeof(struct uproc *)) < 0)
    return -1;
  if (max <= 0)
    return -1;
  if (max > NPROC)
    max = NPROC;

  struct proc *p;

  acquire(&ptable2.lock);
  int j=0;

  #if DEBUG_GETPROCS
  int k=0;
  #endif

  for (p = ptable2.proc; p < &ptable2.proc[NPROC]; p++) {
    #if DEBUG_GETPROCS
    cprintf("%d %s %d\n",k++, p->name, p->sz);
    #endif

    if (j + 1 > max)
        break;
    if (p->state != UNUSED) {
      table[j].pid = p->pid;
      table[j].ppid = p->parent->pid;
      table[j].sz = p->sz;
      table[j].state = (int) p->state;
      strncpy(table[j].name, p->name, strlen(p->name));
      j++;
    }
  }
  release(&ptable2.lock);
  return j;
}