
struct uproc {
  int pid;
  int ppid;       // Parent's PID, or 0 if there is no parent
  int state;      // Process state from kernel/proc.h
  uint64 sz;      // Size of user memory in bytes
  char name[16];
};
