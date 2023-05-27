#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFERSIZE 4096

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
    int status;
    int in_fd, n_chars;
    char buf[BUFFERSIZE];
    char statFilePath[50];

    sprintf(statFilePath, "/proc/%d/stat", pid);

    // open /proc/[pid]/stat
    if((in_fd = open(statFilePath, O_RDONLY)) == -1){
        perror("open");
        exit(1);
    }
    // read /proc/[pid]/stat -> 읽어들인 데이터 buf에 저장
    if((n_chars = read(in_fd, buf, BUFFERSIZE)) == -1){
        perror("read");
        exit(1);
    }

    // stat 파일 열기
    /*FILE* statFile = fopen(statFilePath, "r");
    if (statFile == NULL) {
        perror("Failed to open stat file");
        return;
    }*/

    // stat 파일에서 CPU 사용량 추출
    unsigned long utime, stime;
    sscanf(buf, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %lu %lu", &utime, &stime);

    // CPU 사용량 출력
    printf("CPU Usage (PID: %d): %lu\n", pid, utime + stime);

    // stat 파일 닫기
    close(in_fd);
}

ssize_t readline(int fd, void *buffer, size_t n) {
    ssize_t total_bytes = 0, bytes_read;
    char *buf = buffer;
    char ch;
    
    while (total_bytes < n - 1) {
        bytes_read = read(fd, &ch, 1);

        if (bytes_read <= 0) {
            // No more input or an error occurred
            break;
        }

        *buf++ = ch;
        total_bytes++;

        if (ch == '\n') {
            // End of the line
            break;
        }
    }

    *buf = '\0';
    return (bytes_read <= 0 && total_bytes == 0) ? -1 : total_bytes;
}

// PID를 입력받아 메모리 사용량을 출력하는 함수
void printMemoryUsage(int pid) {
    char statusFilePath[50];
    sprintf(statusFilePath, "/proc/%d/status", pid);

    int in_fd, n_read;
    char buf[BUFFERSIZE];
    char line[100];
    
    if((in_fd = open(statusFilePath, O_RDONLY)) == -1){
        perror("open");
        exit(1);
    }
    while((n_read = readline(in_fd, line, sizeof(line))) > 0){
        if(strncmp(line, "VmRSS:", 6) == 0){
            unsigned long memUsage;
            sscanf(line, "%*s %lu", &memUsage);
            printf("Memory Usage (PID: %d): %lu kB\n", pid, memUsage);
            break;
        }
    }
    if(n_read == -1){
        perror("read");
        exit(1);
    }
    close(in_fd);

    // status 파일 열기
    /*FILE* statusFile = fopen(statusFilePath, "r");
    if (statusFile == NULL) {
        perror("Failed to open status file");
        return;
    }

    // status 파일에서 메모리 사용량 추출
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
    fclose(statusFile);*/
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
        printCpuUsage(pid);
        printMemoryUsage(pid);
        wait(NULL);
    }


    return 0;
}
