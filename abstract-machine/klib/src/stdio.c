#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define MAX_BUF 2048
#define TMP_INT_LEN 32
#define is_digit(ch) ((ch) >= '0' && (ch) <= '9')
char tmp_int[TMP_INT_LEN];
// buf char len
int buf_len = 0;
// 1 for ok 0 for out
int check_buflen(int len)
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
  int all_count = 1;
  int int_count = 1;
  // int 153->"153" tmp_int
  int div = 10;
  while (num / div != 0)
  {
    all_count++;
    div *= 10;
  }
  for (int index = 0; index < all_count; index++)
  {
    tmp_int[all_count - int_count] = '0' + num % 10;
    int_count++;
    num = num / 10;
  }
  return 1;
}
int vsprintf(char *out, const char *fmt, va_list ap)
{
  char *buf_ptr = out;

  const char *fmt_ptr = fmt;
  int print_count = 0;
  // putstr("into vsprintf\n")
  while ((*fmt_ptr) != '\0' && check_buflen(buf_len)) // check if buffer out of range
  {
    if (*fmt_ptr == '%' && *(fmt_ptr + 1) != '\0') // is var
    {
      ++fmt_ptr;
      switch (*fmt_ptr)
      {
      case 'd':
      {
        int i = va_arg(ap, int);
        // fommat int->s then put in
        if (!i2s(i))
          return -1;
        // get value
        for (int index = 0; tmp_int[index] != '\0'; index++)
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
          *(char *)buf_ptr = t_str[start];
          buf_ptr += sizeof(char);
        }
        buf_len += start;
        print_count += start;
        break;
      }
      default:
        break;
      }
    }
    else // normal
    {
      *(char *)buf_ptr = *fmt_ptr;
      buf_ptr += sizeof(char);
      buf_len++;
      print_count++;
    }
    fmt_ptr++;
  }
  *(buf_ptr) = '\0';
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
