#ifndef __MONITOR_LOG_H__
#define __MONITOR_LOG_H__

#include <common.h>

#ifdef DEBUG
extern FILE *log_fp;
extern FILE *log_cr;
extern void add_call_ret(const char *fmt, ...);
#define log_write(...)                  \
  do                                    \
  {                                     \
    extern bool log_enable();           \
    if (log_fp != NULL && log_enable()) \
    {                                   \
      fprintf(log_fp, __VA_ARGS__);     \
      fflush(log_fp);                   \
    }                                   \
  } while (0)
#else
#define log_write(...)
#endif

#define _Log(...)           \
  do                        \
  {                         \
    printf(__VA_ARGS__);    \
    log_write(__VA_ARGS__); \
  } while (0)

void strcatf(char *buf, const char *fmt, ...);

#endif
