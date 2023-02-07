#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// static char HEX_CHARACTERS[] = "0123456789ABCDEF";
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

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(const char *string, int width, int pad) {
  int pc = 0, padchar = ' ';
  if (width > 0) {
    int len = 0;
    const char *ptr;
    for (ptr = string; *ptr; ++ptr) ++len;
    if (len >= width) width = 0;
    else width -= len;
    if (pad & PAD_ZERO) padchar = '0';    // 如果是0填充
  }
  if (!(pad & PAD_RIGHT)) {               // 如果不是右对齐
    for ( ; width > 0; --width) {
      putch(padchar);
      ++pc;
    }
  }
  for ( ; *string ; ++string) {
    putch(*string);
    ++pc;
  }
  for ( ; width > 0; --width) {
    putch(padchar);
    ++pc;
  }
  return pc;
}


#define PRINT_BUF_LEN 12

static int printi(int i, int b, int sg, int width, int pad, int letbase) {  
  char print_buf[PRINT_BUF_LEN];    
  char *s;                          
  int t, neg = 0, pc = 0;           
  unsigned int u = i;   
  if (i == 0) {
    print_buf[0] = '0';
    print_buf[1] = '\0';
    return prints(print_buf, width, pad);
  }
  if (sg && b == 10 && i < 0) {
    neg = 1;
    u = -i;
  }
  s = print_buf + PRINT_BUF_LEN-1;
  *s = '\0';
  while (u) {
    t = u % b;
    if( t >= 10 )
      t += letbase - '0' - 10;
    *--s = t + '0';
    u /= b;
  }
  if (neg) {
    if( width && (pad & PAD_ZERO) ) {
      putch('-');
      ++pc;
      --width;
    }
    else {
      *--s = '-';
    }
  }
  return pc + prints(s, width, pad);
}



int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char *txt, cha; //txt是字符串，cha是字符
  int num; 
  unsigned int unum;  
  uint32_t pointer;

  int width, pad;     // width是宽度，pad是填充
  int pc = 0;         // pc是打印的字符数 
  char scr[2];        // 用于存放一个字符

  for (; *fmt != '\0'; ++fmt) {
    if (*fmt == '%') {
      ++fmt;
      width = pad = 0;
      if (*fmt == '\0') break;
      if (*fmt == '%') goto out;
      if (*fmt == '-') {
        ++fmt;
        pad = PAD_RIGHT;
      }
      while (*fmt == '0') {
        ++fmt;
        pad |= PAD_ZERO;
      }
      for ( ; *fmt >= '0' && *fmt <= '9'; ++fmt) {
        width *= 10;
        width += *fmt - '0';
      }
      if( *fmt == 's' ) {
        txt = va_arg(ap, char*);
        pc += prints(txt ? txt : "(null)", width, pad);
        continue;
      }
      if( *fmt == 'd' ) {
        num = va_arg(ap, int);
        pc += printi(num, 10, 1, width, pad, 'a');
        continue;
      }
      if( *fmt == 'x' ) {
        unum = va_arg(ap, unsigned int);
        pc += printi(unum, 16, 0, width, pad, 'a');
        continue;
      }
      if( *fmt == 'X' ) {
        unum = va_arg(ap, unsigned int);
        pc += printi(unum, 16, 0, width, pad, 'A');
        continue;
      }
      if( *fmt == 'u' ) {
        unum = va_arg(ap, unsigned int);
        pc += printi(unum, 10, 0, width, pad, 'a');
        continue;
      }
      if( *fmt == 'c' ) {
        cha = (char)va_arg(ap, int);
        scr[0] = cha;
        scr[1] = '\0';
        pc += prints(scr, width, pad);
        continue;
      }
      if( *fmt == 'p' ) {
        pointer = va_arg(ap, uint32_t);
        pc += prints("0x", 0, pad);
        pc += printi(pointer, 16, 0, width, pad, 'a');
        continue;
      }
    }
    else {
    out:
      putch(*fmt);
      ++pc;
    }
  }
  if (out) *out = '\0';
  va_end(ap);
  return pc;
  // int state = 0, i, j;//模仿一个状态机  0是初始状态  1是%状态
  // for (i = 0, j = 0; fmt[i] != '\0'; ++i){
  //   switch (state)
  //   {
  //   case 0:
  //     if (fmt[i] != '%'){ 
  //       append(fmt[i]);
  //     } else  
  //       state = 1;
  //     break;
    
  //   case 1:
  //     switch (fmt[i])
  //     {
  //     case 's':
  //       txt = va_arg(ap, char*);  // va_arg(ap, type) 从ap中取出一个type类型的参数
  //       for (int k = 0; txt[k] !='\0'; ++k)
  //         append(txt[k]);
  //       break;
  //     case 'd':
  //       num = va_arg(ap, int);
  //       if(num == 0){
  //         append('0');
  //         break;
  //       }
  //       if (num < 0){
  //         append('-');
  //         num = 0 - num;
  //       }
  //       for (len = 0; num ; num /= 10, ++len)
  //         //buffer[len] = num % 10 + '0';//逆序的
  //         buffer[len] = HEX_CHARACTERS[num % 10];//逆序的
  //       for (int k = len - 1; k >= 0; --k)
  //         append(buffer[k]);
  //       break;
      
  //     case 'c':
  //       cha = (char)va_arg(ap, int);
  //       append(cha);
  //       break;

  //     case 'p':
  //       pointer = va_arg(ap, uint32_t);
  //       for (len = 0; pointer ; pointer /= 16, ++len)
  //         buffer[len] = HEX_CHARACTERS[pointer % 16];//逆序的
  //       for (int k = 0; k < BIT_WIDE_HEX - len; ++k)
  //         append('0');

  //       for (int k = len - 1; k >= 0; --k)
  //         append(buffer[k]);
  //       break;

  //     case 'x':
  //       unum = va_arg(ap, unsigned int);
  //       if(unum == 0){
  //         append('0');
  //         break;
  //       }
  //       for (len = 0; unum ; unum >>= 4, ++len)
  //         buffer[len] = HEX_CHARACTERS[unum & 0xF];//逆序的

  //       for (int k = len - 1; k >= 0; --k)
  //         append(buffer[k]);
  //       break;
        
  //     default:
  //       assert(0);
  //     }
  //     state = 0;
  //     break;
      
  //   }
  // }

  // out[j] = '\0';
  // return j;
}

#endif
