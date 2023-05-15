#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
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
        unsigned long utime, stime, cutime, cstime;
        fscanf(stat_file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %lu %lu %lu %lu", &utime, &stime, &cutime, &cstime);
        fclose(stat_file);

        double usr_time = utime / (double)sysconf(_SC_CLK_TCK);
        double sys_time = stime / (double)sysconf(_SC_CLK_TCK);
        double guest_time = (cutime + cstime) / (double)sysconf(_SC_CLK_TCK);

        // 출력
        printf("Execution Time: %.6f seconds\n", usr_time + sys_time);
        printf("UID: %d\n", getuid());
        printf("PID: %d\n", pid);
        printf("USR: %.6f\n", usr_time);
        printf("System: %.6f\n", sys_time);
        printf("Guest: %.6f\n", guest_time);
        printf("Wait: 0.000000\n");
        printf("CPU: %.6f\n", usr_time + sys_time + guest_time);

        exit(0);
    }

    return 0;
}
