#include <memory.h>

static void *pf = NULL;
#define PGSIZE 4096
void *new_page(size_t nr_page)
{
  void *p = pf;
  memset(pf, 0, nr_page * PGSIZE);
  pf = (void *)((uintptr_t)pf + nr_page * PGSIZE);
  return p;
}

static inline void *pg_alloc(int n)
{
  int pages = n % PGSIZE == 0 ? n / PGSIZE : n / PGSIZE + 1;
  return new_page(pages);
}

void free_page(void *p)
{
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk)
{
  return 0;
}

void init_mm()
{
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %x", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
