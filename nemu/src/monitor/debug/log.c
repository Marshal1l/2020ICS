#include <common.h>
#include <stdarg.h>
#define RING_SIZE 10
#define TMP_LEN 256
FILE *log_fp = NULL;
FILE *log_cr = NULL;
typedef struct Log_ring_node Log_ring_node;
typedef struct Log_ring_node
{
  char log_asm[80];
  Log_ring_node *next;
  vaddr_t addr;
} Log_ring_node;
static const char *Logfile_cr = "../build/call_ret.txt";
char log_bytebuf[80] = {};
char log_asmbuf[80] = {};
static char crbuf[TMP_LEN] = {};
static char tempbuf[TMP_LEN] = {};
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
void init_logcall_ret()
{
  log_cr = fopen(Logfile_cr, "w");
}
void add_call_ret(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(crbuf, sizeof(crbuf), fmt, ap);
  fwrite(crbuf, sizeof(crbuf), 1, log_cr);
  va_end(ap);
  strcat(crbuf, tempbuf);
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
    log_ring[i].next = &log_ring[i + 1];
  }
  log_ring[RING_SIZE - 1].next = &log_ring[0];
  head_node = &log_ring[0];
}
void print_ring()
{
  printf("Ring buffer list:\n");
  Log_ring_node *tmp = head_node;
  while (!if_emptynode(tmp))
  {
    if (tmp->next == head_node)
    {
      printf("0x%08x\t%s\n", tmp->addr, tmp->log_asm);
      break;
    }
    printf("0x%08x\t%s\n", tmp->addr, tmp->log_asm);
    tmp = tmp->next;
  }
}
void add2ring(vaddr_t tmp_pc, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(tempbuf, sizeof(tempbuf), fmt, ap);
  va_end(ap);
  if (!if_emptynode(free_node))
  {
    memset(head_node->log_asm, '\0', sizeof(head_node->log_asm));
    head_node = head_node->next;
  }
  // var
  strcat(free_node->log_asm, tempbuf);
  free_node->addr = tmp_pc;
  free_node = free_node->next;
}
void strcatf(char *buf, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  memset(tempbuf, '0', TMP_LEN);
  vsnprintf(tempbuf, TMP_LEN, fmt, ap);
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
