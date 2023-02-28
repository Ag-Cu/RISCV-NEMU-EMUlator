#include <common.h>
#include "syscall.h"

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

size_t fs_write(int fd, void *buf, size_t len);


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  // strace
  // printf("Syscall %s(%d, %d, %d, %d) at epc = 0x%x\n", sysnames[a[0]], a[0], a[1], a[2], a[3], c->mepc);

  switch (a[0]) {
    case SYS_yield: yield(); c->GPRx = 0; break;
    case SYS_exit: halt(a[1]); break;
    case SYS_write: c->GPRx = fs_write(a[1], (void *)a[2], a[3]); break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}


size_t fs_write(int fd, void *buf, size_t len) {
  assert(fd == 1 || fd == 2);
  for (size_t i = 0; i < len; i ++) {
    putch(((char *)buf)[i]);
  }
  return len;
}


