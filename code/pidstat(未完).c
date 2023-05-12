#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>

void fatal(char* s1, char* s2, int n)
{
    /*
        purpose: print out error and terminate
    */
    fprintf(stderr, "Error: %s, %s\n", s1, s2);
    exit(n);
}

// extended malloc
void* emalloc(size_t n) {
    void* rv = (void*)malloc(n);
    if (rv == NULL) {
        fatal("out of memory", "", 1);
    }
    return rv;
}

void* erealloc(void* p, size_t n) {
    void* rv = realloc(p, n);
    if (rv == NULL) {
        fatal("realloc() failed", "", 1);
    }
    return rv;
}

void freelist(char** list) {
    char** cp = list;
    while (*cp) {
        free(*cp++);
    }
    free(list);
}

int main(int argc, char *argv[]) {
    int status;
    struct rusage usage;
    
    int pid, argnum;  // 현재 프로세스의 PID를 가져옴
    char** arglist;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s", argv[0]);
        exit(1);
    }
    arglist = emalloc(BUFSIZ * argc);
    for (argnum = 0; argnum < argc; argnum++) {
        arglist[argnum] = argv[argnum + 1];
    }
    arglist[argnum] = NULL;


    if ((pid = fork()) == -1)
        perror("fork");
    else if (pid == 0) {
        execvp(arglist[0], arglist);
        perror("unexecutable command");
        exit(1);
    }
    else {
         // 부모 프로세스
            waitpid(pid, &status, 0);
            if (getrusage(RUSAGE_CHILDREN, &usage) == -1)
            {
                perror("getrusage");
                exit(1);
            }
    }
    printf("maxrss  ixrss  idrss  isrss  inblock  outblock  command\n");
    printf("%ld  %ld  %ld  %ld  %ld  %ld  %s\n", usage.ru_maxrss, usage.ru_ixrss, usage.ru_idrss, usage.ru_isrss, usage.ru_inblock, usage.ru_oublock, argv[1]);

    free(arglist);
    return 0;
}
