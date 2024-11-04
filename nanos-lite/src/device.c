#include <common.h>
#include <sys/time.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
    [AM_KEY_NONE] = "NONE",
    AM_KEYS(NAME)};
int sys_gettimeofday(struct timeval *tz, struct timezone *tv)
{
  tz->tv_sec = io_read(AM_TIMER_UPTIME).us / 1000000;
  tz->tv_usec = io_read(AM_TIMER_UPTIME).us % 1000000;
  return 0;
}
size_t serial_write(const void *buf, size_t offset, size_t len)
{
  for (int i = 0; i < len; i++)
  {
    putch(*(const uint8_t *)buf);
    buf++;
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len)
{
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len)
{
  return 0;
}

void init_device()
{
  Log("Initializing devices...");
  ioe_init();
}
