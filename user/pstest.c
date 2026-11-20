#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  int pid1 = fork();
  if(pid1 == 0){
    pause(50); 
    exit(0);
  }

  int pid2 = fork();
  if(pid2 == 0){
    pause(50);
    exit(0);
  }

  struct pinfo buf[64];

  int n_all = getprocs(buf, 64);
  printf("Test 1 (max=64): found %d procs (parent pid=%d, child1=%d, child2=%d)\n",
         n_all, getpid(), pid1, pid2);


  int n_small = getprocs(buf, 2);
  printf("Test 2 (max=2): returned %d procs\n", n_small);

  int n_inv = getprocs(buf, -5);
  printf("Test 3 (max=-5): returned %d (expected -1)\n", n_inv);

  wait(0);
  wait(0);
  exit(0);
}