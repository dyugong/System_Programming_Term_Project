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
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // 자식 프로세스에서 "ls" 실행
        execlp(argv[1], argv[1], NULL);
        perror("execlp");
        exit(1);
    } else {
        // 부모 프로세스에서 자식 프로세스의 정보 출력
        int status;
        // 자식 프로세스의 정보 가져오기
        char stat_path[32];
        sprintf(stat_path, "/proc/%d/stat", pid);

        FILE *stat_file = fopen(stat_path, "r");
        if (stat_file == NULL) {
            perror("fopen");
            exit(1);
        }
	
	waitpid(pid, &status, 0);
        unsigned long cutime, cstime;
        fscanf(stat_file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*d %*d %lu %lu", &cutime, &cstime);
        fclose(stat_file);
	getrusage(RUSAGE_CHILDREN, &usage);
        double guest_time = (cutime + cstime) / (double)sysconf(_SC_CLK_TCK);
	guest_time = 0;        
	// 출력
        printf("UID: %d\n", getuid());
        printf("PID: %d\n", pid);
        printf("USR: %ld.%06ld s\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        printf("System:  %ld.%06ld s\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
        printf("Guest: %.6f s\n", guest_time);
        printf("CPU: %ld.%06ld s\n", (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec), (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec));

        exit(0);
    }

    return 0;
}
