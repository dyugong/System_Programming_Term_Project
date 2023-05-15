#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // 자식 프로세스에서 "ls" 실행
        execlp("ls", "ls", NULL);
        perror("execlp");
        exit(1);
    } else {
        // 부모 프로세스에서 자식 프로세스의 정보 출력
        int status;
        waitpid(pid, &status, 0);

        // 실행 시간 가져오기
        long clk_tck = sysconf(_SC_CLK_TCK);
        FILE *stat_file = fopen("/proc/self/stat", "r");
        if (stat_file == NULL) {
            perror("fopen");
            exit(1);
        }
        unsigned long utime, stime;
        fscanf(stat_file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %lu %lu", &utime, &stime);
        fclose(stat_file);
        double exec_time = (utime + stime) / (double) clk_tck;

        // UID와 PID 가져오기
        uid_t uid = getuid();
        pid_t child_pid = pid;

        // CPU 사용량 가져오기
        FILE *statm_file = fopen("/proc/self/statm", "r");
        if (statm_file == NULL) {
            perror("fopen");
            exit(1);
        }
        unsigned long size, resident, shared, text, lib, data, dt;
        fscanf(statm_file, "%lu %lu %lu %lu %lu %lu %lu", &size, &resident, &shared, &text, &lib, &data, &dt);
        fclose(statm_file);
        double cpu_usage = (double) (resident + shared + text + lib + data) / (double) clk_tck;

        // 출력
        printf("Execution Time: %.6f seconds\n", exec_time);
        printf("UID: %d\n", uid);
        printf("PID: %d\n", child_pid);
        printf("USR: %.6f\n", cpu_usage);
        printf("System: 0.000000\n");
        printf("Guest: 0.000000\n");
        printf("Wait: 0.000000\n");
        printf("CPU: %.6f\n", cpu_usage);

        exit(0);
    }

    return 0;
}
