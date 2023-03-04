#include <common.h>
#include <fs.h>
#include "syscall.h"
#include<stdint.h>
#include<proc.h>

char *sysnames[] = {
  "SYS_exit",
  "SYS_yield",
  "SYS_open",
  "SYS_read",
  "SYS_write",
  "SYS_kill",
  "SYS_getpid",
  "SYS_close",
  "SYS_lseek",
  "SYS_brk",
  "SYS_fstat",
  "SYS_time",
  "SYS_signal",
  "SYS_execve",
  "SYS_fork",
  "SYS_link",
  "SYS_unlink",
  "SYS_wait",
  "SYS_times",
  "SYS_gettimeofday"
};

struct timeval{
  long int tv_sec;
  long int tv_usec;
}tv;

struct timezone{
  int tz_minuteswest;
  int tz_dsttime;
}tz;

int fs_gettimeofday(struct timeval *tv, struct timezone *tz) {
  uint64_t uptime=0;
  ioe_read(AM_TIMER_UPTIME, &uptime);
  tv->tv_usec = (int32_t)uptime;
  tv->tv_sec = (int32_t)uptime / 1000000;
  return 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

    #ifdef CONFIG_STRACE_COND
    printf("Syscall %s(%d, %d, %d, %d) at epc = 0x%x\n", sysnames[a[0]], a[0], a[1], a[2], a[3], c->mepc);
    #endif

  switch (a[0]) {
    case SYS_yield:
        yield();
        c->GPRx = 0; 
        break;
    case SYS_exit: 
        halt(a[1]); 
        break;
    case SYS_write: 
        c->GPRx = fs_write(a[1], (void *)a[2], a[3]); 
        break;
    case SYS_brk: 
        c->GPRx = 0;
        break;
    case SYS_open: 
        yield();
        c->GPRx = fs_open((void *)a[1], a[2], a[3]);
        break;
    case SYS_read:
        c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
        break;
    case SYS_close:
        c->GPRx = fs_close(a[1]);
        break;
    case SYS_lseek:
        c->GPRx = fs_lseek(a[1], a[2], a[3]);
        break;
    case SYS_gettimeofday:
        c->GPRx = fs_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]);
        break;
    default: 
        panic("Unhandled syscall ID = %d", a[0]);
}
}


