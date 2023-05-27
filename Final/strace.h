#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

void stexecute(int argc, char** argv);
// ptrace attach to child
void child(char** argv);
// do like gdb
void parent(pid_t pid);
void detach();
char* syscalltostring(long id);