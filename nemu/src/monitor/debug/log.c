#include <common.h>
#include <stdarg.h>
#define RING_SIZE 20
FILE *log_fp = NULL;
typedef struct Log_ring_node Log_ring_node;
typedef struct Log_ring_node
{
  char log_asm[80];
  Log_ring_node *next;
} Log_ring_node;

char log_bytebuf[80] = {};
char log_asmbuf[80] = {};
static char tempbuf[256] = {};
static Log_ring_node log_ring[RING_SIZE];
static Log_ring_node *head_node = &log_ring[0];
static Log_ring_node *free_node = &log_ring[0];
int if_emptynode(Log_ring_node *node)
{
  if (node->log_asm[0] == '\0')
  {
    return 1;
  }
  return 0;
}
void init_log(const char *log_file)
{
  if (log_file == NULL)
    return;
  log_fp = fopen(log_file, "w");
  Assert(log_fp, "Can not open '%s'", log_file);
  free_node = log_ring;
  // 连接环形链表
  for (int i = 0; i < RING_SIZE - 1; i++)
  {
    log_ring[i].next = log_ring[i + 1].next;
  }
  log_ring[RING_SIZE - 1].next = &log_ring[0];
  head_node = &log_ring[0];
}

void add2ring(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(tempbuf, sizeof(tempbuf), fmt, ap);
  va_end(ap);
  if (!if_emptynode(free_node))
  {
    head_node = head_node->next;
  }
  strcat(free_node->log_asm, tempbuf);
  free_node = free_node->next;
  // printf("Ring buffer list:\n");
  // for (Log_ring_node *tmp = head_node; (!if_emptynode(tmp)) && tmp->next != head_node; tmp = tmp->next)
  // {
  //   printf("---%s\n", tmp->log_asm);
  // }
}
void strcatf(char *buf, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(tempbuf, sizeof(tempbuf), fmt, ap);
  va_end(ap);
  strcat(buf, tempbuf);
}

void asm_print(vaddr_t this_pc, int instr_len, bool print_flag)
{
  snprintf(tempbuf, sizeof(tempbuf), FMT_WORD ":   %s%*.s%s", this_pc, log_bytebuf,
           50 - (12 + 3 * instr_len), "", log_asmbuf);
  log_write("%s\n", tempbuf);
  if (print_flag)
  {
    puts(tempbuf);
  }

  log_bytebuf[0] = '\0';
  log_asmbuf[0] = '\0';
}
