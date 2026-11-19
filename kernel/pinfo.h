#ifndef PINFO_H
#define PINFO_H

#include "types.h"

struct pinfo {
  int pid;
  int ppid;
  int state;
  uint64 sz;
  char name[16];
};

#endif