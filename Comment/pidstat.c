#include "pidstat.h"

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
    struct rusage usage; // Child 프로세스의 resource사용정보를 담을 구조체 usage 선언한다.
    int status = 0; // 프로세스의 종료상태
    int pid = fork();
    int org_pid;
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    if (pid > 0) //Parent
    {
        org_pid = pid;
    }
    else if (pid == 0) //Child
    {
        execvp(argv[1], argv + 1); // 주어진 파라미터로 실행한다.
        perror("execvp");
        exit(1);
    }

    long long ttmp = printMemoryUsage(pid); //메모리 사용량을 담는 ttmp
    waitpid(pid, &status, 0); // Child 프로세스가 종료될 때까지 wait한다.

    getrusage(RUSAGE_CHILDREN, &usage); 
    // RUSAGE_CHILDREN: 호출한 프로세스의 모든 자식 프로세스의 리소스 사용량을 가져온다.
    // 현재 프로세스의 모든 자식 프로세스의 리소스 사용량 정보를 usage 구조체에 저장한다.

    if (status)
    {
        exit(1);
    }
    else //정상적으로 종료되었다면
    {
        printf("Current Child PID: %d\n", pid);
        printf("UID: %d\n", getuid());
        printf("PID: %d\n", pid);
        printf("USR: %ld.%06ld s\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        printf("System:  %ld.%06ld s\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
        printf("CPU: %ld.%06ld s\n", (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec), (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec));
        printf("Memory Usage (PID: %d): %llu kB\n", org_pid, ttmp);

        //pidstat.txt 파일에 정보를 기록한다.
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