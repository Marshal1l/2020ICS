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
int sys_gettimeofday(struct timeval *tv, struct timezone *tz)
{
  AM_TIMER_UPTIME_T uptime;
  uptime = io_read(AM_TIMER_UPTIME);
  int sec = tv->tv_sec = uptime.us / 1000000;
  tv->tv_usec = uptime.us - sec * 100000;
  tz = NULL;
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
