#include <stdio.h>
#include <sys/time.h>

typedef unsigned int uint32_t;

uint32_t NDL_GetTicks() {
  // 以毫秒为单位返回系统时间
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


int main() {
    uint32_t old_msec = NDL_GetTicks();
    int i = 1;
    while (1) {
        uint32_t now_msec = NDL_GetTicks();
        if (now_msec != old_msec && now_msec - old_msec >= 500) {
            printf("Hello World from Navy-apps for the %dth time!\n", i++);
            old_msec = now_msec;
        }
    }
  return 0;
}