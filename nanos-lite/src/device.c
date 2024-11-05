#include <common.h>
#include <sys/time.h>

#include <common.h>
#include <fs.h>

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

size_t serial_write(const void *buf, size_t offset, size_t len)
{
#ifndef HAS_TIMER_IRQ
  yield();
#endif

  for (int i = 0; i < len; i++)
  {
    putch((((char *)(buf))[i]));
  }
  return len;
}
#define KEYDOWN_MASK 0x8000
#define KBD_ADDR 0xa1000060
static inline uint32_t inl(int port)
{
  uint32_t data;
  asm volatile("inl %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

// void change_fg(int i);

size_t events_read(void *buf, size_t offset, size_t len)
{

#ifndef HAS_TIMER_IRQ
  yield();
#endif

  AM_INPUT_KEYBRD_T kbd;
  int k = inl(KBD_ADDR);
  kbd.keydown = (k & KEYDOWN_MASK ? true : false);
  kbd.keycode = k & ~KEYDOWN_MASK;
  if (kbd.keycode == AM_KEY_NONE)
    return 0;
  else
  {
    char ku[] = "ku";
    char kd[] = "kd";
    char *kstate = (char *)((kbd.keydown) ? &kd : &ku);
    int res = sprintf(buf, "%s %s\n", kstate, keyname[kbd.keycode]);

    // if (kbd.keycode == AM_KEY_F1)
    // {
    //   change_fg(1);
    // }
    // if (kbd.keycode == AM_KEY_F2)
    // {
    //   change_fg(2);
    // }
    // if (kbd.keycode == AM_KEY_F3)
    // {
    //   change_fg(3);
    // }
    printf("event : %s\n", buf);
    return res;
  }
}
int sys_gettimeofday(struct timeval *tv, struct timezone *tz)
{
  AM_TIMER_UPTIME_T uptime;
  uptime = io_read(AM_TIMER_UPTIME);
  int sec = tv->tv_sec = (uint64_t)uptime.us >> 32;
  tv->tv_usec = (uint64_t)uptime.us - ((uint64_t)sec << 32);
  tz = NULL;
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
