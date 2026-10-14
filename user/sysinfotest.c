#include "kernel/types.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

int
main(void)
{
  struct sysinfo info;

  if (sysinfo(&info) < 0) {
    printf("sysinfo failed\n");
    exit(1);
  }

  printf("Free memory: %d bytes\n", (int)info.freemem);
  printf("Process count: %d\n", (int)info.nproc);
  printf("Open file count: %d\n", (int)info.nopenfile);

  exit(0);
}