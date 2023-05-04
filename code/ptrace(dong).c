#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>

int main(int argc, char* argv[]) {
    pid_t pid;
    struct user_regs_struct regs;

    if (argc != 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        exit(1);
    }

    pid = atoi(argv[1]); //pid 숫자로 변환
    ptrace(PTRACE_ATTACH, pid, NULL, NULL); //

    wait(NULL);

    while (1) {
        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);

        wait(NULL);

        ptrace(PTRACE_GETREGS, pid, NULL, &regs);

        if (regs.orig_rax == SYS_write) {
            printf("Write system call detected: %lld %lld %lld\n", regs.rdi, regs.rsi, regs.rdx);
        }

        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);

        wait(NULL);
    }

    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    return 0;
}
