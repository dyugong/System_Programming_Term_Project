//"strace.h"
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
void child(int, char**);
void parent(pid_t);
char* syscalltostring(long);
