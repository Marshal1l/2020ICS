#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define MAX_BUF 2048

#define is_digit(ch) ((ch) >= '0' && (ch) <= '9')

int vsprintf(char *out, const char *fmt, va_list ap)
{
  char *buf_ptr = out;
  const char *fmt_ptr = fmt;
  putstr("into vsprintf\n");
  while (*fmt_ptr != '\0')
  {
    if (*fmt_ptr == '%' && *(fmt_ptr + 1) != '\0')
    {
      fmt_ptr++;
      switch (*fmt_ptr)
      {
      case 'd':
      {
        int i = va_arg(ap, int);
        *(int *)buf_ptr = i;
        buf_ptr += sizeof(int);
        break;
      }
      case 's':
      {
        char *t_str = va_arg(ap, char *);
        for (int start = 0; t_str[start] != '\0'; t_str++)
        {
          *(char *)buf_ptr = t_str[start];
          buf_ptr += sizeof(char);
        }
        break;
      }
      default:
        break;
      }
    }
  }
  return 0;
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vsprintf(out, fmt, args);
  va_end(args);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  return 0;
}

#endif
