#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void *(*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void *) = NULL;
static int vme_enable = 0;

static Area segments[] = { // Kernel memory mappings
    NEMU_PADDR_SPACE};

#define USER_SPACE RANGE(0x40000000, 0xc0000000)

bool vme_init(void *(*pgalloc_f)(int), void (*pgfree_f)(void *))
{
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);
  printf("kernel alloc page finish\n");
  int i;
  for (i = 0; i < LENGTH(segments); i++)
  {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE)
    {
      map(&kas, va, va, 0);
    }
  }
  printf("kernel map finish\n");
  set_cr3(kas.ptr);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;
  printf("kernel start end\n");
  return true;
}

void protect(AddrSpace *as)
{
  PTE *updir = (PTE *)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as)
{
}

void __am_get_cur_as(Context *c)
{
  c->cr3 = (vme_enable ? (void *)get_cr3() : NULL);
}

void __am_switch(Context *c)
{
  if (vme_enable && c->cr3 != NULL)
  {
    set_cr3(c->cr3);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot)
{
  assert(NULL != as->ptr);
  uint32_t off_dir = (uint32_t)va >> 22;
  uint32_t off_pte = ((uint32_t)va & 0x003ff000) >> 12;
  uint32_t *dir = (uint32_t *)as->ptr;
  uint32_t *pte = &dir[off_dir];
  if ((*pte & 1) == 0)
  {
    uint32_t *tem = (uint32_t *)pgalloc_usr(1024 * 4);
    *pte = ((uint32_t)tem & 0xfffff000) | 1;
  }
  *(uint32_t *)((int)(*pte & 0xfffff000) + off_pte * 4) = ((uint32_t)pa & 0xfffff000) | 1;
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry)
{
  Context *new_p = (Context *)(kstack.end - 16 - sizeof(Context));
  new_p->eip = (uintptr_t)entry;
  new_p->cs = 8;
  new_p->cr3 = NULL;
  new_p->eflags = 0x00000200;
  return new_p;
}
