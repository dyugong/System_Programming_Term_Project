#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

void strace(int, char**);
// ptrace attach to child
void child(int, char**);
// do like gdb
void parent(pid_t);
char* syscalltostring(long);