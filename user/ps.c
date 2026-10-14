
#include "kernel/types.h"
#include "kernel/uproc.h"
#include "user/user.h"

char *
statename(int state)
{
  switch (state) {
  case 1:
    return "used";
  case 2:
    return "sleeping";
  case 3:
    return "runnable";
  case 4:
    return "running";
  case 5:
    return "zombie";
  default:
    return "unknown";
  }
}

int
main(void)
{
  struct uproc procs[64];
  int n;
  int i;

  n = getprocs(procs, 64);

  if (n < 0) {
    printf("ps: getprocs failed\n");
    exit(1);
  }

  printf("PID\tPPID\tSTATE\tSIZE\tNAME\n");

  for (i = 0; i < n; i++) {
    printf("%d\t%d\t%s\t%lu\t%s\n",
           procs[i].pid,
           procs[i].ppid,
           statename(procs[i].state),
           procs[i].sz,
           procs[i].name);
  }

  exit(0);
}
