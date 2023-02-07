#include <am.h>
#include <nemu.h>
#include "../../../riscv/riscv.h"

uint64_t boot_time = 0;

void __am_timer_init() {
//   ioe_read(AM_TIMER_UPTIME, &boot_time);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
//   uptime->us = inl(RTC_ADDR) - boot_time;
  uptime->us = 0;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
