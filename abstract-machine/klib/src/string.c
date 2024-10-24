#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
//
size_t strlen(const char *s)
{
  const char *tmp = s;
  while (*tmp != '\0')
  {
    tmp++;
  }
  return tmp - s;
}
//
char *strcpy(char *dst, const char *src)
{
  return memcpy((void *)dst, (void *)src, strlen(src) + 1);
}

char *strncpy(char *dst, const char *src, size_t n)
{
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for (; i < n; i++)
    dst[i] = '\0';
  return dst;
}

char *strcat(char *dst, const char *src)
{
  return strcpy(dst + strlen(dst), src);
}
// strcmp
int strcmp(const char *s1, const char *s2)
{
  const char *tmp1 = s1;
  const char *tmp2 = s2;
  char a, b;
  do
  {
    a = *tmp1++;
    b = *tmp2++;
    if (a == '\0' || b == '\0')
      return a - b;
  } while (a == b);
  return a - b;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  const char *tmp1 = s1;
  const char *tmp2 = s2;
  char a, b;
  do
  {
    a = *tmp1++;
    b = *tmp2++;
    n--;
    if (a == '\0' || b == '\0' || n == 0)
      return a - b;
  } while (a == b);
  return a - b;
}

void *memset(void *v, int c, size_t n)
{
  uint8_t *dst = (uint8_t *)v;
  while (n > 0)
  {
    *dst++ = c;
    n--;
  }
  return v;
}
// 防止内存重叠
void *memmove(void *dst, const void *src, size_t n)
{
  uint8_t *tmpsrc = (uint8_t *)src;
  uint8_t *tmpdst = (uint8_t *)dst;
  if (dst < src)
  {
    for (size_t i = 0; i < n; ++i)
    {
      tmpdst[i] = tmpsrc[i];
    }
  }
  else
  {
    for (size_t i = n - 1; i > -1; --i)
    {
      tmpdst[i] = tmpdst[i];
    }
  }
  return dst;
}
//
void *memcpy(void *out, const void *in, size_t n)
{
  uint8_t *dest = (uint8_t *)out;
  uint8_t *src = (uint8_t *)in;
  size_t i;
  for (i = 0; i < n; ++i, ++dest, ++src)
  {
    *dest = *src;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
  uint8_t *tmp1 = (uint8_t *)s1;
  uint8_t *tmp2 = (uint8_t *)s2;
  uint8_t a, b;
  do
  {
    a = *tmp1++;
    b = *tmp2++;
    n--;
    if (a == '\0' || n == 0)
      return a - b;
  } while (a == b);
  return a - b;
}

#endif
