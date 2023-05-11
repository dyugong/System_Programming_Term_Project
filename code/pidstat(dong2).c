#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {

    int status;
    char stat_path[32];
    char stat_str[1024];

    pid_t pid = fork();

    if(pid < 0){
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if(pid == 0){
        execvp(argv[1], argv);
        perror("execvp failed.");
        exit(2);
    }
    else{
        pid = waitpid(pid, &status, 0);
        sprintf(stat_path, "/proc/%d/stat", pid);
        FILE *stat_file = fopen(stat_path, "r");
        if (stat_file == NULL) {
            perror("fopen");
            exit(1);
        }
        fgets(stat_str, 1024, stat_file);
        fclose(stat_file);
        unsigned long long utime_ch, stime_ch;
        sscanf(stat_str, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %llu %llu", &utime_ch, &stime_ch);
        double utime = (utime_ch) / (double)sysconf(_SC_CLK_TCK);
        double stime = (stime_ch) / (double)sysconf(_SC_CLK_TCK);
        double total_time = utime + stime;
        printf("CPU time used by ls command: %.6f seconds\n", total_time);
        return 0;
    }

}