#include "watchpoint.h"
#include "expr.h"
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
// 控制台打印w信息
void info_w_display()
{
  if (head == NULL)
  {
    printf("No watchpoint now!\n");
  }
  WP *tmp_wp = head;
  while (tmp_wp != NULL)
  {
    printf("NO:%d\t expr:  %s\n", tmp_wp->NO, tmp_wp->wp_expr);

    tmp_wp = tmp_wp->next;
  }
};
void info_fw_display()
{
  if (free_ == NULL)
  {
    printf("No free watchpoint now!\n");
  }
  WP *tmp_wp = free_;
  int num = 0;
  while (tmp_wp != NULL)
  {
    if (num == 5)
    {
      printf("\n");
      num = 0;
    }
    num++;
    printf("free NO:%d\t", tmp_wp->NO);
    tmp_wp = tmp_wp->next;
  };
  printf("\n");
}
// TODO new wp
WP *new_wp(char *s)
{
  if (free_ == NULL)
  {
    printf("No more free watchpoint in pool!\n");
    assert(0);
    return NULL;
  }
  // 从free上释放节点
  WP *new_wp = free_;
  free_ = free_->next;
  new_wp->next = NULL;
  // 连接新节点到head上
  if (head == NULL)
  {
    head = new_wp;
    head->next = NULL;
  }
  else
  {
    WP *tmp_wp = head;
    while (tmp_wp->next != NULL)
    {
      tmp_wp = tmp_wp->next;
    }
    tmp_wp->next = new_wp;
  }
  bool success = false;
  // 初始化数据
  new_wp->old_val = expr(s, &success);
  for (int i = 0; s[i] != '\0'; i++)
  {
    new_wp->wp_expr[i] = s[i];
  }
  if (success)
  {
    printf("new watchpoint:NO %d\texpr: %s\n", new_wp->NO, new_wp->wp_expr);
    return new_wp;
  }
  else
  {
    printf("expr defult\n");
    assert(0);
    return NULL;
  }
};
// TODO free wp which NO==no_delete
void delete_wp(WP *find, WP *prev_find)
{
  if (find == NULL)
  {
    assert(0);
  }
  // 重连head
  if (prev_find != NULL) // find处于中间
  {
    if (find->next != NULL)
      prev_find->next = find->next;
    else
      prev_find->next = NULL;
  }
  else // 第一个是find
  {
    if (find->next != NULL)
      head = find->next;
    else
      head = NULL;
  }
  printf("del watchpoint NO:%d\texpr:%s\n", find->NO, find->wp_expr);
  // 清空数据
  for (int i = 0; i < WPEXPRLEN; i++)
    find->wp_expr[i] = '\0';
  find->old_val = 0;
  // 连接到free
  find->next = free_;
  free_ = find;
}
void free_wp(int no_delete)
{
  if (head == NULL)
  {
    printf("No watchpoint here \n");
    return;
  }
  WP *find = head;
  WP *prev_find = head;
  if (find->NO == no_delete)
  {
    delete_wp(find, NULL);
  }
  else
  {
    find = find->next;
    while (find != 0)
    {
      if (find->NO == no_delete)
      {
        delete_wp(find, prev_find);
      }
    }
    printf("no such a watchpoint\n");
    return;
  }
};
// void check_wp()
// {
//   WP *tmp_head = head;
//   bool success = true;
//   while (tmp_head != NULL)
//   {
//     uint32_t result = expr(tmp_head->wp_expr, &success);
//     if (!success)
//       assert(0);
//     if (result != tmp_head->old_val)
//     {
//       tmp_head->old_val = result;
//       printf("NO:%d\t expr:  %s\t val: %x\n", tmp_head->NO, tmp_head->wp_expr, tmp_head->old_val);
//       nemu_state.state = NEMU_STOP;
//       break;
//     }
//     tmp_head = tmp_head->next;
//   }
// }
void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
