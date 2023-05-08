#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    pid_t pid = getpid();  // 현재 프로세스의 PID를 가져옴
    char stat_path[32];
    sprintf(stat_path, "/proc/%d/stat", pid);

    // 명령어 실행 전의 CPU 사용 시간
    FILE *stat_file = fopen(stat_path, "r");
    if (stat_file == NULL) {
        perror("fopen");
        exit(1);
    }
    char stat_str[1024];
    fgets(stat_str, 1024, stat_file);
    fclose(stat_file);

    unsigned long long utime_before, stime_before;
    sscanf(stat_str, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %llu %llu", &utime_before, &stime_before);

    // ls 명령어 실행
    system("ls");

    // 명령어 실행 후의 CPU 사용 시간
    stat_file = fopen(stat_path, "r");
    if (stat_file == NULL) {
        perror("fopen");
        exit(1);
    }
    fgets(stat_str, 1024, stat_file);
    fclose(stat_file);

    unsigned long long utime_after, stime_after;
    sscanf(stat_str, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %llu %llu", &utime_after, &stime_after);

    // CPU 사용 시간 계산
    double utime = (utime_after - utime_before) / (double)sysconf(_SC_CLK_TCK);
    double stime = (stime_after - stime_before) / (double)sysconf(_SC_CLK_TCK);
    double total_time = utime + stime;
    printf("CPU time used by ls command: %.6f seconds\n", total_time);

    return 0;
}
