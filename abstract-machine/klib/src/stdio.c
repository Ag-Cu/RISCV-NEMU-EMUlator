#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char HEX_CHARACTERS[] = "0123456789ABCDEF";
#define BIT_WIDE_HEX 8

#define append(x) {out[j++]=x; if (j >= n) {break;}}

int printf(const char *fmt, ...) {
  char buffer[2048];
  va_list arg;
  va_start (arg, fmt);
  
  int done = vsprintf(buffer, fmt, arg);

  putstr(buffer);

  va_end(arg);
  return done;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, -1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int ret = vsprintf(out, fmt, ap);

  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int ret = vsnprintf(out, n, fmt, ap);

  va_end(ap);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char buffer[128]; // 128 is enough for a 32-bit integer
  char *txt, cha; //txt是字符串，cha是字符
  int num, len; 
  unsigned int unum;  
  uint32_t pointer; 
  
  
  int state = 0, i, j;//模仿一个状态机  0是初始状态  1是%状态
  for (i = 0, j = 0; fmt[i] != '\0'; ++i){
    switch (state)
    {
    case 0:
      if (fmt[i] != '%'){ 
        append(fmt[i]);
      } else  
        state = 1;
      break;
    
    case 1: 
      switch (fmt[i])
      {
      case 's':
        txt = va_arg(ap, char*);  // va_arg(ap, type) 从ap中取出一个type类型的参数
        for (int k = 0; txt[k] !='\0'; ++k)
          append(txt[k]);
        break;
      
      case 'd':
        num = va_arg(ap, int);
        if(num == 0){
          append('0');
          break;
        }
        if (num < 0){
          append('-');
          num = 0 - num;
        }
        for (len = 0; num ; num /= 10, ++len)
          //buffer[len] = num % 10 + '0';//逆序的
          buffer[len] = HEX_CHARACTERS[num % 10];//逆序的
        for (int k = len - 1; k >= 0; --k)
          append(buffer[k]);
        break;
      
      case 'c':
        cha = (char)va_arg(ap, int);
        append(cha);
        break;

      case 'p':
        pointer = va_arg(ap, uint32_t);
        for (len = 0; pointer ; pointer /= 16, ++len)
          buffer[len] = HEX_CHARACTERS[pointer % 16];//逆序的
        for (int k = 0; k < BIT_WIDE_HEX - len; ++k)
          append('0');

        for (int k = len - 1; k >= 0; --k)
          append(buffer[k]);
        break;

      case 'x':
        unum = va_arg(ap, unsigned int);
        if(unum == 0){
          append('0');
          break;
        }
        for (len = 0; unum ; unum >>= 4, ++len)
          buffer[len] = HEX_CHARACTERS[unum & 0xF];//逆序的

        for (int k = len - 1; k >= 0; --k)
          append(buffer[k]);
        break;
      
      default:
        assert(0);
      }
      state = 0;
      break;
      
    }
  }

  out[j] = '\0';
  return j;
}

#endif
