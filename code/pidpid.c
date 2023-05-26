#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <errno.h>

void sighandler(int signum)
{
    printf("There's no permission\n");
}

int main(int argc, char* argv[])
{
    struct sigaction newhandler;
    sigset_t blocked;
    void sighandler();

    newhandler.sa_handler = sighandler;
    newhandler.sa_flags = SA_RESETHAND | SA_RESTART;

    sigemptyset(&blocked);
    sigaddset(&blocked, SIGQUIT);

    newhandler.sa_mask = blocked;

    if (sigaction(SIGINT, &newhandler, NULL) == -1)
    {
        perror("sigaction\n");
    }

    struct rusage usage;
    int status = 0;
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

    if (status)
    {
        exit(1);
    }
    else
    {
        // 출력
        printf("UID: %d\n", getuid());
        printf("PID: %d\n", pid);
        printf("USR: %ld.%06ld s\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        printf("System:  %ld.%06ld s\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
        printf("CPU: %ld.%06ld s\n", (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec), (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec));
    }

    return 0;
}
