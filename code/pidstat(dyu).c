#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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


// PID를 입력받아 CPU 사용량을 출력하는 함수
void printCpuUsage(int pid) {
    char statFilePath[50];
    sprintf(statFilePath, "/proc/%d/stat", pid);

    // stat 파일 열기
    FILE* statFile = fopen(statFilePath, "r");
    if (statFile == NULL) {
        perror("Failed to open stat file");
        return;
    }

    // stat 파일에서 CPU 사용량 추출
    unsigned long utime, stime;
    fscanf(statFile, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %lu %lu", &utime, &stime);

    // CPU 사용량 출력
    printf("CPU Usage (PID: %d): %lu\n", pid, utime + stime);

    // stat 파일 닫기
    fclose(statFile);
}

// PID를 입력받아 메모리 사용량을 출력하는 함수
void printMemoryUsage(int pid) {
    char statusFilePath[50];
    sprintf(statusFilePath, "/proc/%d/status", pid);

    // status 파일 열기
    FILE* statusFile = fopen(statusFilePath, "r");
    if (statusFile == NULL) {
        perror("Failed to open status file");
        return;
    }

    // status 파일에서 메모리 사용량 추출
    char line[100];
    while (fgets(line, sizeof(line), statusFile) != NULL) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            unsigned long memUsage;
            sscanf(line, "%*s %lu", &memUsage);

            // 메모리 사용량 출력
            printf("Memory Usage (PID: %d): %lu kB\n", pid, memUsage);
            break;
        }
    }

    // status 파일 닫기
    fclose(statusFile);
}

int main(int argc, char* argv[]) {
    
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
    }
    else {
        wait(NULL);
        printCpuUsage(pid);
        printMemoryUsage(pid);
    }


    return 0;
}

