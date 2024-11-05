
#include <stdio.h>
#include <sys/time.h>
#include <assert.h>
int main()
{

  struct timeval num;
  num.tv_sec = 0;
  gettimeofday(&num, NULL);
  int boot_time = num.tv_sec;
  printf("time-test starts begin = %ld\n", num.tv_sec);

  // int sec = num.tv_sec + 2;
  // while (1)
  // {
  //   while (num.tv_sec <= sec)
  //   {
  //     int t = gettimeofday(&num, NULL);
  //     assert(t == 0);
  //   }
  //   printf("time: seconds = %d   sec=%ld usec%ld\n", sec, num.tv_sec, num.tv_usec);
  //   sec++;
  // }
}
