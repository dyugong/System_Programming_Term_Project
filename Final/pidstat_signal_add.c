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

int flag;

void sighandler(int signum)
{
    flag = 1;
    printf("Do you want to stop?\n");
}

int main(int argc, char* argv[])
{
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
    }
    struct rusage usage;
    int status = 0;
    int pid = fork();
    if (pid > 0)
    {
        int org_pid = pid;
    }


    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        if (argc >= 3)
        {
            execlp(argv[2], argv[2], NULL);
        }
        else
        {
            execlp(argv[1], argv[1], NULL);
        }
        perror("execlp");
        exit(1);
    }

    waitpid(pid, &status, 0);

    if (argc >= 3 && strcmp(argv[1], "-r") == 0)
    {
        while (1)
        {
	    char ch;
	    ch = getc(stdin);
	    while(ch != 'N' && ch !='Y')
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
                    execlp(argv[2], argv[2], NULL);
                    perror("execlp");
                    exit(1);
                }
                else
                {
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
    }

    return 0;
}
