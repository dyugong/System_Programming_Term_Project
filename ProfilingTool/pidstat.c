#include "pidstat.h"

#define BUFFERSIZE 4096

int flag;

ssize_t readline(int fd, void* buffer, size_t n) {
    ssize_t total_bytes = 0, bytes_read;
    char* buf = buffer;
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
long long printMemoryUsage(int pid) {
    char statusFilePath[50];
    sprintf(statusFilePath, "/proc/%d/status", pid);

    int in_fd, n_read;
    char line[100];
    unsigned long memUsage;

    if ((in_fd = open(statusFilePath, O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }
    while ((n_read = readline(in_fd, line, sizeof(line))) > 0) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            sscanf(line, "%*s %lu", &memUsage);
            break;
        }
    }
    if (n_read == -1) {
        perror("read");
        exit(1);
    }
    close(in_fd);
    return memUsage;
}

int pidstat(int argc, char** argv)
{
    struct rusage usage;
    int status = 0;
    int pid = fork();
    int org_pid;
    if (pid > 0)
    {
        org_pid = pid;
    }

    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {

        execvp(argv[1], argv + 1);
        perror("execvp");
        exit(1);
    }

    long long ttmp = printMemoryUsage(pid);
    waitpid(pid, &status, 0);

    getrusage(RUSAGE_CHILDREN, &usage);

    if (status)
    {
        exit(1);
    }
    else
    {
        // 출력
        printf("Current Child PID: %d\n", pid);
        printf("UID: %d\n", getuid());
        printf("PID: %d\n", pid);
        printf("USR: %ld.%06ld s\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        printf("System:  %ld.%06ld s\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
        printf("CPU: %ld.%06ld s\n", (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec), (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec));
        printf("Memory Usage (PID: %d): %llu kB\n", org_pid, ttmp);
        FILE* file;
        file = fopen("pidstat.txt", "w");
        fprintf(file, "Current Child PID: %d\n", pid);
        fprintf(file, "UID: %d\n", getuid());
        fprintf(file, "PID: %d\n", pid);
        fprintf(file, "USR: %ld.%06ld s\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        fprintf(file, "System:  %ld.%06ld s\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
        fprintf(file, "CPU: %ld.%06ld s\n", (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec), (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec));
        fprintf(file, "Memory Usage (PID: %d): %llu kB\n", org_pid, ttmp);
        fclose(file);
    }

    return 0;

}