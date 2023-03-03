#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t stdin_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t stdout_write(const void *buf, size_t offset, size_t len) {
  for (size_t i = 0; i < len; i ++) {
    if (((char *)buf)[i] == 0) { return i; }
    putch(((char *)buf)[i]);
  }
  return len;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, 0, stdin_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, stdout_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the `read` and `write` field of `file_table`
  for (int i = 3; i < sizeof(file_table) / sizeof(Finfo); ++i) {
    file_table[i].read = ramdisk_read;
    file_table[i].write = ramdisk_write;
  }

  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < sizeof(file_table) / sizeof(Finfo); ++i) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  assert(0);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  assert(fd >= 0 && fd < sizeof(file_table) / sizeof(Finfo));

  Finfo *f = &file_table[fd];

  if (fd == FD_STDIN) {
    return f->read(buf, 0, len);
  }

  if (len == 0) return 0;
  if (f->open_offset >= f->size) return 0;

  if (len + f->open_offset > f->size) len = f->size - f->open_offset;

  size_t offset = f->disk_offset + f->open_offset;
  f->open_offset += len;
  return f->read(buf, offset, len);
}

size_t fs_write(int fd, const void *buf, size_t len) {
    assert(fd >= 0 && fd < sizeof(file_table) / sizeof(Finfo));

    Finfo *f = &file_table[fd];

    if (fd == FD_STDOUT || fd == FD_STDERR) {
        return f->write(buf, 0, len);
    }


    if (len == 0) return 0;
    if (f->open_offset >= f->size) return 0;

    if (len + f->open_offset > f->size) len = f->size - f->open_offset;

    size_t offset = f->disk_offset + f->open_offset;

    size_t ret = f->write(buf, offset, len);
    f->open_offset += len;
    return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  assert(fd >= 0 && fd < sizeof(file_table) / sizeof(Finfo));

  Finfo *f = &file_table[fd];

  switch (whence) {
    case SEEK_SET: f->open_offset = offset; break;
    case SEEK_CUR: f->open_offset += offset; break;
    case SEEK_END: f->open_offset = f->size + offset; break;
    default: assert(0);
  }
  return f->open_offset;
}

int fs_close(int fd) {
  assert(fd >= 0 && fd < sizeof(file_table) / sizeof(Finfo));
  return 0;
}
