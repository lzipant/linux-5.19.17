#include <stdio.h>
#include <unistd.h>
#include <string.h>
int main ()
{
  char * buffer = "Welcome to lzip-linux-5.19.17!";
  int ret;
  ret = syscall(480, buffer, strlen(buffer)+1);
  printf("syscall ret: %d\n", ret);
  return 0;
}
