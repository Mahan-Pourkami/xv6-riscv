#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  static char *states[] = {
    [0] "UNUSED",
    [1] "USED",
    [2] "SLEEPING",
    [3] "RUNNABLE",
    [4] "RUNNING",
    [5] "ZOMBIE"
  };

  struct pinfo procs[64];
  int n = getprocs(procs, 64);

  if(n < 0){
    fprintf(2, "ps: getprocs failed\n");
    exit(1);
  }

  printf("PID\tPPID\tSTATE\t\tSIZE\tNAME\n");
  for(int i = 0; i < n; i++){
    char *st = (procs[i].state >= 0 && procs[i].state <= 5) ? states[procs[i].state] : "???";
    printf("%d\t%d\t%s\t%ld\t%s\n",
           procs[i].pid,
           procs[i].ppid,
           st,
           procs[i].sz,
           procs[i].name);
  }

  exit(0);
}