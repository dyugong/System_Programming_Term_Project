#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

int main() {
    pid_t pid;
    int status;
    struct rusage r_usage;
    struct timeval start, end;
    double time_taken;

    pid = fork();

    if (pid == 0) {
        // Child process
        execl("/bin/ls", "ls", "-al", NULL);
    }
    else if (pid > 0) {
        // Parent process
        waitpid(pid, &status, 0);
        gettimeofday(&start, NULL);
        //printf("Parent process executing...\n");
        //system("ls -al");
        gettimeofday(&end, NULL);
        time_taken = (end.tv_sec - start.tv_sec) * 1e6;
        time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6;
        getrusage(RUSAGE_CHILDREN, &r_usage);
        printf("CPU time used: %ld.%06ld s\n", r_usage.ru_utime.tv_sec, r_usage.ru_utime.tv_usec);
        printf("Wall-clock time taken: %lf s\n", time_taken);
    }
    else {
        // Fork failed
        printf("Fork failed!\n");
        exit(1);
    }

    return 0;
}