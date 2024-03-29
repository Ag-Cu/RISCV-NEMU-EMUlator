#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;
static char *hbrk;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

char * itoa(int value, char * str) {
  char *p = str;
  char *head = str;
  int tmp = 0;
  if (value < 0) {
    *p++ = '-';
    value = -value;
  }
  do {
    tmp = value % 10;
    *p++ = tmp + '0';
    value /= 10;
  } while (value > 0);
  *p = '\0';
  for (--p; head < p; ++head, --p) {
    tmp = *head;
    *head = *p;
    *p = tmp;
  }
  return str;
}

void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
// #if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
//   panic("Not implemented");
// #endif
  size = (size + 7) & ~7;   // 意思是size+7后，取反，然后再取反，这样就是8的倍数了
  if (hbrk == NULL) {
    hbrk = (char *)heap.start;
  }
  char *old = hbrk;
  hbrk += size;
  if (hbrk > (char *)heap.end) {
    return NULL;
  }
  return old;
}

void free(void *ptr) {
}

#endif
