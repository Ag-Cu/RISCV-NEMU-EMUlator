#include <common.h>
#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (size_t i = 0; i < len; i ++) {
    if (((char *)buf)[i] == 0) { return i; }
    putch(((char *)buf)[i]);
  }
  return len;
}

static AM_INPUT_KEYBRD_T ev;
size_t events_read(void *buf, size_t offset, size_t len) {
  ioe_read(AM_INPUT_KEYBRD, &ev);
  // if (ev.keycode != AM_KEY_NONE) {
  //   Log("keycode = %s, keydown = %d", keyname[ev.keycode], ev.keydown);
  // }
  if (ev.keycode == AM_KEY_NONE) {
    return 0;
  }
  if (ev.keydown) {
    strncat(buf, "kd ", 3);
  } else {
    strncat(buf, "ku ", 3);
  }
  strncat(buf, keyname[ev.keycode], len - 3);
  strncat(buf, "\n", len - 3 - strlen(keyname[ev.keycode]));
  return strlen(buf);
}

static AM_GPU_CONFIG_T gpu_config;
static AM_GPU_FBDRAW_T gpu_fbdraw;

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  ioe_read(AM_GPU_CONFIG,&gpu_config);
  int width = gpu_config.width, height = gpu_config.height;
  
  char num[32];
  strcpy(buf, "WIDTH:");
  strcat(buf, itoa(width, num));
  strcat(buf, "\nHEIGHT:");
  strcat(buf, itoa(height, num));
  strcat(buf, "\n");
  printf("dispinfo_read: %s", buf);
  return strlen((char *)buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  if (len == 0)
  {
    gpu_fbdraw.sync = 1;
    gpu_fbdraw.w = 0;
    gpu_fbdraw.h = 0;
    ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);
    return 0;
  }
  int width = gpu_config.width;
  gpu_fbdraw.pixels = (void *)buf;
  gpu_fbdraw.w = len;
  gpu_fbdraw.h = 1;
  gpu_fbdraw.x = offset % width;
  gpu_fbdraw.y = offset / width;
  gpu_fbdraw.sync = 0;
  ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);

  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
