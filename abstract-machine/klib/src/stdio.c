#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define MAX_BUF 2048
#define TMP_INT_LEN 20
#define is_digit(ch) ((ch) >= '0' && (ch) <= '9')
char tmp_int[TMP_INT_LEN];
// buf char len
int buf_len = 0;
// 1 for ok 0 for out
int check_len(int len)
{
  if (len > MAX_BUF)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
int i2s(int num)
{
  // reset tmp_int
  for (int i = 0; i < TMP_INT_LEN; i++)
  {
    tmp_int[i] = '\0';
  }
  int single = 0;
  int int_count = 0;
  while (num != 0)
  {
    if (int_count >= TMP_INT_LEN)
      return 0;
    single = num % 10;
    tmp_int[int_count++] = '0' + single;
    num = num / 10;
  }
  return 1;
}
int vsprintf(char *out, const char *fmt, va_list ap)
{
  char *buf_ptr = out;
  const char *fmt_ptr = fmt;
  int print_count = 0;
  // putstr("into vsprintf\n");
  while ((*fmt_ptr) != '\0')
  {
    if (*fmt_ptr == '%' && *(fmt_ptr + 1) != '\0')
    {
      fmt_ptr++;
      switch (*fmt_ptr)
      {
      case 'd':
      {
        int i = va_arg(ap, int);
        // check if buffer out of range
        if (!check_len(buf_len))
          return -1;
        // fommat int->s then put in
        if (!i2s(i))
          return -1;
        // var
        int index = 0;
        while (tmp_int[index] != '\0')
        {
          *(char *)buf_ptr = tmp_int[index];
          buf_ptr += sizeof(char);
          buf_len++;
          print_count++;
        }
        break;
      }
      case 's':
      {
        char *t_str = va_arg(ap, char *);
        int start = 0;
        for (; t_str[start] != '\0'; t_str++)
        {
          if (!check_len(buf_len))
            return -1;
          *(char *)buf_ptr = t_str[start];
          buf_ptr += sizeof(char);
        }
        buf_len += sizeof(char) * start;
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
