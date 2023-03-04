#include <stdio.h>
#include <sys/time.h>

int main() {
  // 通过gettimeofday()获取当前时间, 并每过0.5秒输出一句话.
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    int old_sec = tv.tv_sec;
    int old_usec = tv.tv_usec;
    while (1) {
        gettimeofday(&tv, &tz);
        if (tv.tv_sec != old_sec || tv.tv_usec - old_usec >= 500000) {
            printf("tv_sec = %ld, tv_usec = %ld, tz_minuteswest = %d, tz_dsttime = %d\n", tv.tv_sec, tv.tv_usec, tz.tz_minuteswest, tz.tz_dsttime);
            old_sec = tv.tv_sec;
            old_usec = tv.tv_usec;
        }
    }
  return 0;
}