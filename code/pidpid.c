#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/resource.h>

int main(int argc, char* argv[])
{
    struct rusage usage;
    int status;
    pid_t pid = fork();

    if (pid < 0) 
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        // 자식 프로세스에서 "ls" 실행
        execlp(argv[1], argv[1], NULL);
        perror("execlp");
        exit(1);
    }

    waitpid(pid, &status, 0);
    getrusage(RUSAGE_CHILDREN, &usage);

    // 출력
    printf("UID: %d\n", getuid());
    printf("PID: %d\n", pid);
    printf("USR: %ld.%06ld s\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
    printf("System:  %ld.%06ld s\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
    printf("CPU: %ld.%06ld s\n", (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec), (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec));

    return 0;
}
