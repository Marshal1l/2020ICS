#include <fs.h>
#include <unistd.h>
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, invalid_write},
    [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};
int fs_open(const char *pathname, int flags, int mode)
{
  for (int i = 0; i < sizeof(file_table) / sizeof(Finfo); i++)
  {
    // printf("file:%s\n", file_table[i].name);
    if (strcmp(pathname, file_table[i].name) == 0)
    {
      return i;
    }
  }
  panic("open file error! filename=:%s\n", pathname);
  return -1;
}
int fs_close(int fd) { return 0; }
void init_fs()
{
  // TODO: initialize the size of /dev/fb
}
size_t fs_write(int fd, const void *buf, size_t len)
{
  if (len == 0)
    return 0;
  size_t size = 0;
  const void *tmpbuf = buf;
  if (fd == 1 || fd == 2)
  {
    for (int i = 0; i < len && *(const uint8_t *)tmpbuf != '\0'; i++)
    {
      putch(*(const uint8_t *)tmpbuf);
      tmpbuf++;
      size++;
    }
    return size;
  }
  Finfo *des_file = &file_table[fd];
  size_t write_pos = des_file->disk_offset + des_file->open_offset;
  if (write_pos + len > des_file->disk_offset + des_file->size)
  {
    panic("write out of the file!\n");
    return -1;
  }
  if ((size = ramdisk_write(tmpbuf, write_pos, len)) < 0)
  {
    panic("write file error!\n");
    return -1;
  }
  des_file->open_offset += size;
  return size;
}
size_t fs_read(int fd, void *buf, size_t len)
{
  if (len == 0)
    return 0;
  Finfo *des_file = &file_table[fd];
  // printf("offset%d\n", des_file->open_offset);
  size_t size = 0;
  size_t read_pos = des_file->disk_offset + des_file->open_offset;
  if (read_pos + len > des_file->disk_offset + des_file->size)
  {
    panic("read out of the file!\n");
    return -1;
  }
  if ((size = ramdisk_read(buf, read_pos, len)) < 0)
  {
    panic("read file error!\n");
    return -1;
  }
  des_file->open_offset += size;
  return size;
}
size_t fs_lseek(int fd, size_t offset, int whence)
{
  Finfo *des_file = &file_table[fd];
  if (whence == SEEK_SET)
  {
    if ((des_file->open_offset = offset) > des_file->size)
    {
      panic("offset out of file error!\n");
      return -1;
    }
    return des_file->open_offset;
  }
  if (whence == SEEK_CUR)
  {
    if ((des_file->open_offset = des_file->open_offset + offset) > des_file->size)
    {
      panic("offset out of file error!\n");
      return -1;
    }
    return des_file->open_offset;
  }
  if (whence == SEEK_END)
  {
    if ((des_file->open_offset = des_file->size + offset) > des_file->size)
    {
      panic("offset out of file error!\n");
      return -1;
    }
    return des_file->open_offset;
  }
  return -1;
}