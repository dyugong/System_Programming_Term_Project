#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    int status;
    int cpu_usage = 0;
    int ch_pid;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }
    else if (pid == 0) {
        // Child process -> argv[1]에 해당하는 명령어 실행
        execvp(argv[1], &argv[1]);
        fprintf(stderr, "Failed to execute command\n");
        //ch_pid = getpid();
        exit(1);
    }
    else {
        // Parent process
        char pid_cmd[1024];
        sprintf(pid_cmd, "pidstat -p %d -u 1", pid);
        //pid_cmd에 실행할 문자열 저장 
        FILE *pipe = popen(pid_cmd, "r");
        //popen()함수를 통해 새로운 프로세스 생성(파이프: 부모 프로세스와 자식 프로세스 사이에 생성)
        //부모 프로세스는 FILE *를 통해 파이프에 접근 -> 파이프에서 pidstat 명령어를 실행하고 그 결과를 부모프로세스에서 출력
        if (!pipe) {
            printf("Failed to execute pidstat command\n");
            exit(1);
        }

        //fgets함수를 통해 pipe로부터 데이터를 읽어옴. -> NULL이 아니라면 계속 실행
        //
        while (1) {
            char buffer[1024];
            if (fgets(buffer, 1024, pipe) != NULL) {
                if (buffer[0] >= '0' && buffer[0] <= '9') {
                    sscanf(buffer, "%*s %*s %*s %d", &cpu_usage);
                    //%d에 해당하는 값만 cpu_usage에 저장.
                    printf("CPU usage: %d%%\n", cpu_usage);
                }
            }
            else {
                // pidstat 명령어 종료됨
                break;
            }
        }

        pclose(pipe);
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Command executed successfully\n");
        }
        else {
            printf("Failed to execute command\n");
            exit(1);
        }
    }

    return 0;
}
