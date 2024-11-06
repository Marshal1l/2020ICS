#include <fs.h>

size_t events_read(void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
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
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write, 0},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write, 0},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write, 0},
    {"/dev/events", 0, 0, events_read, invalid_write, 0},
    {"/dev/fbctl", 0, 0, invalid_read, invalid_write},
    {"/dev/fb", 128, 0, invalid_read, fb_write, 0},
    {"/proc/dispinfo", 256, 0, dispinfo_read, invalid_write, 0},

#include "files.h"
};

#define FILENUM sizeof(file_table) / sizeof(file_table[0])

#define VGACTL_ADDR 0xa1000100

void init_fs()
{
  // TODO: initialize the size of /dev/fb
  int h = io_read(AM_GPU_CONFIG).height;
  int w = io_read(AM_GPU_CONFIG).width;
  file_table[5].size = (h * w) * sizeof(int); //  /dev/fb
}

size_t ramdisk_read(void *buf, size_t offset, size_t len);

size_t fs_open(const char *pathname, int flags, int mode)
{
  for (int i = 0; i < FILENUM; i++)
  {
    // printf("path:=%s,this:=%s\n", pathname, file_table[i].name);
    if (strcmp(pathname, file_table[i].name) == 0)
    {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  printf("cannot find the file%s\n", pathname);
  return -1;
}
size_t fs_read(int fd, void *buf, size_t len)
{
  // Finfo file = file_table[fd];
  // printf("read file %d offset is %d\n",fd,file_table[fd].open_offset);
  size_t readlen = -1;
  if (len + file_table[fd].open_offset > file_table[fd].size)
  {
    readlen = file_table[fd].size - file_table[fd].open_offset;
  }
  else
  {
    readlen = len;
  }
  if (file_table[fd].read)
  {
    int a = file_table[fd].read(buf, 0, len);
    // printf("@@@   %d    @@@\n",a);
    return a;
  }

  if (readlen == -1)
    return readlen;
  else
  {
    ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, readlen);
    // printf("buf is  %p\n",buf);
    file_table[fd].open_offset += readlen;
    // printf("read len is %d\n",readlen);
    return readlen;
  }
}

size_t fs_close(int fd)
{
  // printf("close %d\n",fd);
  file_table[fd].open_offset = 0;
  return 0;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{

  // Finfo file = file_table[fd];
  // printf("seek file %d\n",fd);
  // printf("first offset is %d\n",file_table[fd].open_offset);
  if (fd <= 2)
    return 0;
  switch (whence)
  {
  case SEEK_SET:
    // printf("off is %d\n file %d size is %d\n",offset,fd,file_table[fd].size);
    // printf("file[%d] size is %d\n",fd,file_table[fd].size);
    assert((long long int)(offset) >= 0 && offset <= file_table[fd].size);
    file_table[fd].open_offset = offset;
    // printf("fd is %d\n",fd);
    // printf("last offset is %d\n",file_table[22].open_offset);
    return file_table[fd].open_offset;
    break;
  case SEEK_CUR:
    assert((long long int)(offset + file_table[fd].open_offset) >= 0 && offset + file_table[fd].open_offset <= file_table[fd].size);
    file_table[fd].open_offset = file_table[fd].open_offset + offset;
    return file_table[fd].open_offset;
    break;
  case SEEK_END:
    assert((long long int)(offset + file_table[fd].size) >= 0 && offset + file_table[fd].size <= file_table[fd].size);
    file_table[fd].open_offset = file_table[fd].size + offset;
    return file_table[fd].open_offset;
    break;

  default:
    panic("lseek error mode\n");
    break;
  }
}

size_t fs_write(int fd, const void *buf, size_t count)
{
  // printf("len is %d\n",len);
  // printf("come in fs_write fd is %d\n",fd);
  assert(fd >= 0 && fd <= FILENUM);
  // Finfo file;
  // file = file_table[fd];
  // printf("fd is %d\n",fd);
  if (file_table[fd].write != NULL)
  {
    // printf("special write fd %d\n",fd);
    return file_table[fd].write(buf, file_table[fd].open_offset, count);
  }
  else
  {
    assert(file_table[fd].open_offset + count <= file_table[fd].size);
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
    file_table[fd].open_offset += count;
  }
  // printf("come out fs_write fd is %d\n",fd);

  return count;
}

// #include <fs.h>
// #include <unistd.h>

// typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
// size_t ramdisk_read(void *buf, size_t offset, size_t len);
// size_t events_read(void *buf, size_t offset, size_t len);
// size_t dispinfo_read(void *buf, size_t offset, size_t len);

// typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);
// size_t serial_write(const void *buf, size_t offset, size_t len);
// size_t ramdisk_write(const void *buf, size_t offset, size_t len);

// typedef struct
// {
//   char *name;
//   size_t size;
//   size_t disk_offset;
//   ReadFn read;
//   WriteFn write;
//   size_t open_offset;
// } Finfo;

// enum
// {
//   FD_STDIN,
//   FD_STDOUT,
//   FD_STDERR,
//   FD_FB
// };

// size_t invalid_read(void *buf, size_t offset, size_t len)
// {
//   panic("should not reach here");
//   return 0;
// }

// size_t invalid_write(const void *buf, size_t offset, size_t len)
// {
//   panic("should not reach here");
//   return 0;
// }

// /* This is the information about all files in disk. */
// static Finfo file_table[] __attribute__((used)) = {

//     [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
//     [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
//     [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
//     {"/dev/events", 0, 0, events_read, invalid_write},
//     {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
// #include "files.h"

// };
// int fs_open(const char *pathname, int flags, int mode)
// {
//   for (int i = 0; i < sizeof(file_table) / sizeof(Finfo); i++)
//   {
//     if (strcmp(pathname, file_table[i].name) == 0)
//     {
//       file_table[i].open_offset = 0;
//       return i;
//     }
//   }
//   panic("open file error! filename=:%s\n", pathname);
//   return -1;
// }
// int fs_close(int fd)
// {
//   return 0;
// }
// void init_fs()
// {
//   // TODO: initialize the size of /dev/fb
// }
// size_t fs_write(int fd, const void *buf, size_t len)
// {
//   if (len == 0 || fd == 0)
//     return 0;
//   if (fd == 1 || fd == 2)
//   {
//     return file_table[fd].write(buf, 0, len);
//   }
//   size_t writelen = 0;
//   if (file_table[fd].open_offset > file_table[fd].size)
//   {
//     panic("write out of the file!\n");
//     return -1;
//   }
//   if (len + file_table[fd].open_offset > file_table[fd].size)
//   {
//     writelen = file_table[fd].size - file_table[fd].open_offset;
//   }
//   else
//   {
//     writelen = len;
//   }
//   size_t write_pos = file_table[fd].disk_offset + file_table[fd].open_offset;
//   if (ramdisk_write(buf, write_pos, len) < 0)
//   {
//     panic("write file error!\n");
//     return -1;
//   }
//   file_table[fd].open_offset += writelen;
//   return writelen;
// }
// size_t fs_read(int fd, void *buf, size_t len)
// {
//   if (len == 0 || fd == 0)
//     return 0;
//   if (fd == 1 || fd == 2)
//   {
//     size_t readlen = 0;
//     if (file_table[fd].open_offset > file_table[fd].size)
//     {
//       panic("read out of the file!\n");
//       return -1;
//     }
//     if (len + file_table[fd].open_offset > file_table[fd].size)
//     {
//       readlen = file_table[fd].size - file_table[fd].open_offset;
//     }
//     else
//     {
//       readlen = len;
//     }
//     size_t read_pos = file_table[fd].disk_offset + file_table[fd].open_offset;
//     if (ramdisk_read(buf, read_pos, len) < 0)
//     {
//       panic("read file error!\n");
//       return -1;
//     }
//     file_table[fd].open_offset += readlen;
//     return readlen;
//   }
//   else
//   {
//     printf("read from %s\n", file_table[fd].name);
//     int ret = file_table[fd].read(buf, 0, len);
//     return ret;
//   }
// }
// size_t fs_lseek(int fd, size_t offset, int whence)
// {
//   Finfo *des_file = &file_table[fd];
//   if (whence == SEEK_SET)
//   {
//     des_file->open_offset = offset;
//     if (des_file->open_offset > des_file->size || des_file->open_offset < 0)
//     {
//       panic("offset out of file error! offset=:%d\n", des_file->open_offset);
//       return -1;
//     }
//     return des_file->open_offset;
//   }
//   if (whence == SEEK_CUR)
//   {
//     des_file->open_offset = des_file->open_offset + offset;
//     if (des_file->open_offset > des_file->size || des_file->open_offset < 0)
//     {
//       panic("offset out of file error! offset=:%d\n", des_file->open_offset);
//       return -1;
//     }
//     return des_file->open_offset;
//   }
//   if (whence == SEEK_END)
//   {
//     des_file->open_offset = des_file->size + offset;
//     if (des_file->open_offset > des_file->size || des_file->open_offset < 0)
//     {
//       panic("offset out of file error! offset=:%d\n", des_file->open_offset);
//       return -1;
//     }
//     return (des_file->open_offset);
//   }
//   return -1;
// }