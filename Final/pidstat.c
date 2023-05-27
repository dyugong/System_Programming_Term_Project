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

#define BUFFERSIZE 4096

int flag;

void sighandler(int signum)
{
    flag = 1;
    printf("Do you want to stop?\n");
}

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
    char buf[BUFFERSIZE];
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

int main(int argc, char** argv) {

    if (argc >= 3 && strcmp(argv[1], "-r") == 0)
    {
        struct sigaction newhandler;
        sigset_t blocked;
        void sighandler();

        newhandler.sa_handler = sighandler;
        newhandler.sa_flags = SA_RESTART;

        sigemptyset(&blocked);
        sigaddset(&blocked, SIGQUIT);

        newhandler.sa_mask = blocked;

        if (sigaction(SIGINT, &newhandler, NULL) == -1)
        {
            perror("sigaction\n");
        }
        flag = 1;
    }
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
        if (argc >= 3 && flag == 1)
        {
            execvp(argv[2], argv + 2);
        }
        else
        {
            execvp(argv[1], argv + 1);
        }
        perror("execvp");
        exit(1);
    }
    long long ttmp = printMemoryUsage(pid);
    waitpid(pid, &status, 0);

   

    if (argc >= 3 && strcmp(argv[1], "-r") == 0)
    {
        while (1)
        {
            char ch;
            ch = getc(stdin);
            if (ch == 'q')
                break;
            while (ch != 'N' && ch != 'Y')
            {
                printf("Invalid argument\n");
                getchar();
                ch = getc(stdin);
            }
            getchar();
            if (flag == 0)
            {
                break;
            }
            if (ch == 'N')
            {
                int pid;
                if ((pid = fork()) == -1)
                {
                    fprintf(stderr, "fork error");
                    exit(2);
                }
                if (pid == 0)
                {
                    execvp(argv[2], argv + 2);
                    perror("execvp");
                    exit(1);
                }
                else
                {
                    ttmp += printMemoryUsage(pid);
                    waitpid(pid, &status, 0);
                }
            }
            else if (ch == 'Y')
            {
                flag = 0;
                break;
            }
        }
    }


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
    }

    return 0;

   /*
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
   */
}