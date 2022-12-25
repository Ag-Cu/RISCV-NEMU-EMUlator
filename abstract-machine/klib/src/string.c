#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (*s != '\0') {
    len++;
    s++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  // copy src to a temporary buffer first, then copy the buffer to dst
  // this is to avoid the case that src and dst overlap
  size_t src_len;
  for (src_len = 0; src[src_len] != '\0'; src_len++) {
  }
  char buf[src_len + 1];
  for (size_t i = 0; i < src_len; i++) {
    buf[i] = src[i];
  }
  buf[src_len] = '\0';
  for (size_t i = 0; i < src_len + 1; i++) {
    dst[i] = buf[i];
  }
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  for (; i < n; i++) {
    dst[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t i, j;
  for (i = 0; dst[i] != '\0'; i++) {
  }
  for (j = 0; src[j] != '\0'; j++) {
    dst[i + j] = src[j];
  }
  dst[i + j] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  size_t i;
  for (i = 0; s1[i] != '\0' && s2[i] != '\0'; i++) {
    if (s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
  }
  return s1[i] - s2[i];
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i;
  for (i = 0; i < n && s1[i] != '\0' && s2[i] != '\0'; i++) {
    if (s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
  }
  return s1[i] - s2[i];  
}

void *memset(void *s, int c, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    ((uint8_t *)s)[i] = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  size_t i;
  if (dst < src) {
    for (i = 0; i < n; i++) {
      ((uint8_t *)dst)[i] = ((uint8_t *)src)[i];
    }
  } else {
    for (i = n; i > 0; i--) {
      ((uint8_t *)dst)[i - 1] = ((uint8_t *)src)[i - 1];
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    ((uint8_t *)out)[i] = ((uint8_t *)in)[i];
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    if (((uint8_t *)s1)[i] != ((uint8_t *)s2)[i]) {
      return ((uint8_t *)s1)[i] - ((uint8_t *)s2)[i];
    }
  }
  return 0;
}

#endif
