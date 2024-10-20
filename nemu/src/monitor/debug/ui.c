#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/vaddr.h>
char strpc[5] = "$pc==";
void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args)
{
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args)
{
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_dw(char *args);
static int cmd_b(char *args);
static struct
{
  char *name;
  char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "step any instuctions and stop", cmd_si},
    {"s", "step any instuctions and stop", cmd_si},
    {"info", "print the info of r and w", cmd_info},
    {"i", "print the info of r and w", cmd_info},
    {"x", "print the val of given arg", cmd_x},
    {"p", "run expr", cmd_p},
    {"watch", "add watch point", cmd_w},
    {"w", "add watch point", cmd_w},
    {"delete", "delete watch point", cmd_dw},
    {"d", "delete watch point", cmd_dw},
    {"b", "set the  breakpoint", cmd_b},

    /* TODO: Add more commands */

};
// si
static int cmd_si(char *args)
{
  int n_step = 1;
  char *arg = strtok(NULL, " ");
  if (arg == NULL)
    ;
  else
    sscanf(arg, "%d", &n_step);
  printf("nstep %d \n", n_step);
  cpu_exec(n_step);
  return 0;
}
// info
static int cmd_info(char *args)
{
  char *arg = strtok(NULL, " ");
  if (arg == NULL)
  {
    printf("No args\n");
    return 0;
  }
  char rw;
  sscanf(arg, "%c", &rw);
  if (rw == 'r')
  {
    isa_reg_display();
    return 0;
  }
  else if (rw == 'w')
  {
    // info w
    info_w_display();
    return 0;
  }
  else if (rw == 'f')
  {
    // info w
    info_fw_display();
    return 0;
  }
  printf("invalid input\n");
  return 0;
}
// x
static int cmd_x(char *args)
{
  char *n = strtok(args, " ");
  if (n == NULL)
  {
    printf("No args\n");
    return 0;
  }
  char *baseaddr = strtok(NULL, " ");
  if (baseaddr == NULL)
  {
    printf("No second args\n");
    return 0;
  }
  int len = 0;
  paddr_t addr = 0;
  sscanf(n, "%d", &len);
  sscanf(baseaddr, "%x", &addr);
  int val = 0;
  for (int i = 0; i < len; i++)
  {
    val = vaddr_read(addr, 4);
    printf("0x%x\t%08x\t%20d\n", addr, val, val); // addr len
    addr = addr + 4;
  }
  return 0;
}
// p
static int cmd_p(char *args)
{
  if (args == NULL)
  {
    printf("No args\n");
    return 0;
  }
  bool flag = false;
  expr(args, &flag);
  return 0;
};
static int cmd_w(char *args)
{
  if (args == NULL)
  {
    printf("No args\n");
    return 0;
  }
  new_wp(args);
  return 0;
};
static int cmd_dw(char *args)
{
  if (args == NULL)
  {
    printf("No args\n");
    return 0;
  }
  uint32_t no_delete = char2int(args);
  free_wp(no_delete);
  return 0;
};

static int cmd_b(char *args)
{
  if (args == NULL)
  {
    printf("No args\n");
    return 0;
  }
  // char *tmp = strcat(strpc, args);
  printf("%s\n", strpc);
  // new_wp(tmp);
  return 0;
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop()
{
  if (is_batch_mode())
  {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;)
  {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end)
    {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}
