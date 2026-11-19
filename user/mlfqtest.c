// MLFQ mixed-workload test without ps/getprocs dependencies.
#include "kernel/types.h"
#include "user/user.h"

static void
work(int duration, int do_sleep, int burst, int confidence)
{
  int pid = getpid();
  if (mlfqset(burst, confidence) < 0) {
    printf("mlfqtest: mlfqset failed pid=%d (requires MLFQ build)\n", pid);
    exit(1);
  }
  uint begin = uptime();
  volatile uint value = (uint)pid;
  while ((uint)(uptime() - begin) < (uint)duration) {
    for (int i = 0; i < 16000; i++)
      value = value * 1664525U + 1013904223U;
    if (do_sleep)
      pause(1); // Voluntary CPU release
  }
  printf("mlfqtest: done pid=%d burst=%d conf=%d elapsed=%d checksum=%d\n",
         pid, burst, confidence, uptime() - begin, (int)value);
  exit(0);
}

int
main(int argc, char **argv)
{
  int duration = 25; // About 2.5 seconds of global ticks
  if (argc >= 2)
    duration = atoi(argv[1]);
  if (duration < 1 || duration > 1000) {
    printf("usage: mlfqtest [ticks=25, 1..1000]\n");
    exit(1);
  }
  int bursts[4] = {2, 8, 20, 40};
  int confidence[4] = {100, 80, 60, 0};
  printf("mlfqtest: start tick=%d duration=%d; Ctrl-P for queues\n",
         uptime(), duration);
  for (int i = 0; i < 4; i++) {
    int pid = fork();
    if (pid < 0) {
      printf("mlfqtest: fork failed\n");
      exit(1);
    }
    if (pid == 0)
      work(duration, i == 3, bursts[i], confidence[i]);
    printf("mlfqtest: child pid=%d burst=%d conf=%d\n",
           pid, bursts[i], confidence[i]);
  }
  for (int i = 0; i < 4; i++) {
    int status;
    if (wait(&status) < 0 || status != 0) {
      printf("mlfqtest: child failed\n");
      exit(1);
    }
  }
  printf("mlfqtest: PASS children finished; tick=%d\n", uptime());
  exit(0);
}
