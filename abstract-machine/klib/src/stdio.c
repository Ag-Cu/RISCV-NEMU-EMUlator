#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  while(*fmt != '\0') {
    if(*fmt == '%') { // 如果是%开头，说明是格式化输出
      fmt++;
      switch(*fmt) {  // 根据后面的字符，输出不同的格式
        case 'd': {
          int num = va_arg(ap, int);  // 从ap中取出一个int类型的参数
          char buf[20]; // 用于存放转换后的字符串
          int i = 0;
          if(num < 0) { // 如果是负数，先输出一个负号
            out[i++] = '-';
            num = -num;
          }
          do {  // 将数字转换成字符串
            buf[i++] = num % 10 + '0';
            num /= 10;
          } while(num != 0);
          while(i > 0) {  
            *out++ = buf[--i];
          }
          break;
        }
        case 's': {
          char *str = va_arg(ap, char *);
          while(*str != '\0') {
            *out++ = *str++;
          }
          break;
        }
        case 'x': {
          int num = va_arg(ap, int);
          char buf[20];
          int i = 0;
          do {
            int tmp = num % 16;
            if(tmp < 10) {
              buf[i++] = tmp + '0';
            } else {
              buf[i++] = tmp - 10 + 'a';
            }
            num /= 16;
          } while(num != 0);
          while(i > 0) {
            *out++ = buf[--i];
          }
          break;
        }
        case 'p': {
          int num = va_arg(ap, int);
          char buf[20];
          int i = 0;
          do {
            int tmp = num % 16;
            if(tmp < 10) {
              buf[i++] = tmp + '0';
            } else {
              buf[i++] = tmp - 10 + 'a';
            }
            num /= 16;
          } while(num != 0);
          while(i > 0) {
            *out++ = buf[--i];
          }
          break;
        }
        case 'c': {
          char c = va_arg(ap, int);
          *out++ = c;
          break;
        }
        default: {
          *out++ = *fmt;
          break;
        }
      }
    } else {
      *out++ = *fmt;
    }
    fmt++;
  }
    
  va_end(ap);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
