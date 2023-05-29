#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

void sighandler(int);
ssize_t readline(int, void*, size_t);
long long printMemoryUsage(int);
int pidstat(int, char**);