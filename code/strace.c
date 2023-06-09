#include "strace.h"

#define oops(msg) { perror(msg); exit(1); }

double totaltime = 0;     // 총 실행 시간
int totalerror = 0;       // 총 에러 횟수
long totalcall = 0;       // 총 시스템 콜 횟수
long* call;
int* errnor;
double* ptime;

pid_t pid;
char buffer[256];

void strace(int argc, char** argv)
{
    if (argc == 1)    // 인자의 개수가 부족할 때 에러 및 원인 출력
    {
        printf("Usage:\n");
        printf("\t%s EXECUTABLE [arg1] ... [argn]\n", argv[0]);
    }
    else
    {
        call = (long*)malloc(sizeof(long) * 400);     //  호출된 시스템 콜 번호를 담기 위한 배열
        errnor = (int*)malloc(sizeof(int) * 400);     //  에러 횟수를 담기 위한 배열
        ptime = (double*)malloc(sizeof(double) * 400);     //  실행 시간을 담기 위한 변수
        for (int i = 0; i < 400; i++)        // 초기화
        {
            errnor[i] = 0;
            call[i] = 0;
            ptime[i] = 0;
        }
        if ((pid = fork()) < 0)    // fork()호출 
        {
            oops("fork");
        }
        else if (pid == 0)    // 자식 프로세스일 때
        {
            child(argc, argv + 1);
        }
        else   // 부모 프로세스일 때
            parent(pid);
    }
}

void child(int argc, char** argv)
{
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0)      // 자신(자식 프로세스) 추적 시작
        oops("ptrace()");

    if (execvp(argv[0], argv) < 0)   // 커맨드 실행
        oops("execvp()");
}

void parent(pid_t pid)
{
    FILE* fp;
    fp = fopen("strace.txt", "w");        // 결과를 담기 위한 텍스트 파일 생성(쓰기 모드)
    if (fp == NULL) {
        perror("파일 열기 실패");
        exit(EXIT_FAILURE);
    }
    int status;
    struct user_regs_struct regs;      // 레지스터 값을 담는 구조체
    struct timeval start, end;      // 시간을 측정하기 위한 구조체
    double elapsed_us;

    while (1)
    {
        if (waitpid(pid, &status, 0) == -1)       // 자식 프로세스의 종료(시그널)를 기다림
            oops("waitpid()");

        // 시작 시간 기록
        gettimeofday(&start, NULL);


        if (WIFEXITED(status))    // 자식프로세스가 정상적으로 종료 되었을 때 실행
        {
            printf("%%%5s %11s %11s %9s %9s %s\n", "time", "seconds", "usecs/call", "calls", "errors", "syscall");
            printf("------ ----------- ----------- --------- --------- ----------------\n");
            fprintf(fp, "%%%5s %11s %11s %9s %9s %s\n", "time", "seconds", "usecs/call", "calls", "errors", "syscall");
            fprintf(fp, "------ ----------- ----------- --------- --------- ----------------\n");
            for (int i = 0; i < 400; i++)
            {
                if (call[i] != 0)
                {
                    if (errnor[i])
                    {
                        printf(" % 6.2f % 11f % 11d % 9ld % 9d %s\n", ptime[i] / totaltime * 100, ptime[i], (int)(ptime[i] * 1000000) / (int)call[i], call[i], errnor[i], syscalltostring(i));
                        fprintf(fp, " % 6.2f % 11f % 11d % 9ld % 9d %s\n", ptime[i] / totaltime * 100, ptime[i], (int)(ptime[i] * 1000000) / (int)call[i], call[i], errnor[i], syscalltostring(i));
                    }
                    else
                    {
                        printf(" % 6.2f % 11f % 11d % 9ld %9s %s\n", ptime[i] / totaltime * 100, ptime[i], (int)(ptime[i] * 1000000) / (int)call[i], call[i], " ", syscalltostring(i));
                        fprintf(fp, " % 6.2f % 11f % 11d % 9ld %9s %s\n", ptime[i] / totaltime * 100, ptime[i], (int)(ptime[i] * 1000000) / (int)call[i], call[i], " ", syscalltostring(i));
                    }
                }
            }
            printf("------ ----------- ----------- --------- --------- ----------------\n");
            printf("%6.2f %11f %11s %9ld %9d %s\n", totaltime / totaltime * 100, totaltime, " ", totalcall, totalerror, "total");
            fprintf(fp, "------ ----------- ----------- --------- --------- ----------------\n");
            fprintf(fp, "%6.2f %11f %11s %9ld %9d %s\n", totaltime / totaltime * 100, totaltime, " ", totalcall, totalerror, "total");
            fclose(fp);
            break;
        }

        if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1)            // 레지스터 값을 regs에 받아옴
            oops("PTRACE_GETREGS");

        // 시스템 콜 탐색 성공 시 rax의 초기값 -38
        if ((long)regs.rax == -38)
        {
            call[(int)regs.orig_rax] += 1;
            totalcall += 1;
        }
        else
        {
            long code = (long)regs.rax;
            if (code < 0)    // regs구조체의 rax(결과값)레지스터 값이 음수일 경우 에러
            {
                int id = (int)regs.orig_rax;   // regs.orig_rax(시스템 콜 번호)
                errnor[id] += 1;
                totalerror += 1;
            }

        }
        // 종료 시간 기록
        gettimeofday(&end, NULL);
        // 실행 시간 계산 (마이크로초 단위)
        elapsed_us = (end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000;
        ptime[(int)regs.orig_rax] += elapsed_us;
        totaltime += elapsed_us;
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1)            // 다음 시스템콜 추적
            oops("PTRACE_SYSCALL");
    }
}

char* syscalltostring(long id)         // 시스템 콜 번호 -> 시스템콜(문자열)
{
    switch (id)
    {
    case 0:
        return "read";
    case 1:
        return "write";
    case 2:
        return "open";
    case 3:
        return "close";
    case 4:
        return "stat";
    case 5:
        return "fstat";
    case 6:
        return "lstat";
    case 7:
        return "poll";
    case 8:
        return "lseek";
    case 9:
        return "mmap";
    case 10:
        return "mprotect";
    case 11:
        return "munmap";
    case 12:
        return "brk";
    case 13:
        return "rt_sigaction";
    case 14:
        return "rt_sigprocmask";
    case 15:
        return "rt_sigreturn";
    case 16:
        return "ioctl";
    case 17:
        return "pread64";
    case 18:
        return "pwrite64";
    case 19:
        return "readv";
    case 20:
        return "writev";
    case 21:
        return "access";
    case 22:
        return "pipe";
    case 23:
        return "select";
    case 24:
        return "sched_yield";
    case 25:
        return "mremap";
    case 26:
        return "msync";
    case 27:
        return "mincore";
    case 28:
        return "madvise";
    case 29:
        return "shmget";
    case 30:
        return "shmat";
    case 31:
        return "shmctl";
    case 32:
        return "dup";
    case 33:
        return "dup2";
    case 34:
        return "pause";
    case 35:
        return "nanosleep";
    case 36:
        return "getitimer";
    case 37:
        return "alarm";
    case 38:
        return "setitimer";
    case 39:
        return "getpid";
    case 40:
        return "sendfile";
    case 41:
        return "socket";
    case 42:
        return "connect";
    case 43:
        return "accept";
    case 44:
        return "sendto";
    case 45:
        return "recvfrom";
    case 46:
        return "sendmsg";
    case 47:
        return "recvmsg";
    case 48:
        return "shutdown";
    case 49:
        return "bind";
    case 50:
        return "listen";
    case 51:
        return "getsockname";
    case 52:
        return "getpeername";
    case 53:
        return "socketpair";
    case 54:
        return "setsockopt";
    case 55:
        return "getsockopt";
    case 56:
        return "clone";
    case 57:
        return "fork";
    case 58:
        return "vfork";
    case 59:
        return "execve";
    case 60:
        return "exit";
    case 61:
        return "wait4";
    case 62:
        return "kill";
    case 63:
        return "uname";
    case 64:
        return "semget";
    case 65:
        return "semop";
    case 66:
        return "semctl";
    case 67:
        return "shmdt";
    case 68:
        return "msgget";
    case 69:
        return "msgsnd";
    case 70:
        return "msgrcv";
    case 71:
        return "msgctl";
    case 72:
        return "fcntl";
    case 73:
        return "flock";
    case 74:
        return "fsync";
    case 75:
        return "fdatasync";
    case 76:
        return "truncate";
    case 77:
        return "ftruncate";
    case 78:
        return "getdents";
    case 79:
        return "getcwd";
    case 80:
        return "chdir";
    case 81:
        return "fchdir";
    case 82:
        return "rename";
    case 83:
        return "mkdir";
    case 84:
        return "rmdir";
    case 85:
        return "creat";
    case 86:
        return "link";
    case 87:
        return "unlink";
    case 88:
        return "symlink";
    case 89:
        return "readlink";
    case 90:
        return "chmod";
    case 91:
        return "fchmod";
    case 92:
        return "chown";
    case 93:
        return "fchown";
    case 94:
        return "lchown";
    case 95:
        return "umask";
    case 96:
        return "gettimeofday";
    case 97:
        return "getrlimit";
    case 98:
        return "getrusage";
    case 99:
        return "sysinfo";
    case 100:
        return "times";
    case 101:
        return "ptrace";
    case 102:
        return "getuid";
    case 103:
        return "syslog";
    case 104:
        return "getgid";
    case 105:
        return "setuid";
    case 106:
        return "setgid";
    case 107:
        return "geteuid";
    case 108:
        return "getegid";
    case 109:
        return "setpgid";
    case 110:
        return "getppid";
    case 111:
        return "getpgrp";
    case 112:
        return "setsid";
    case 113:
        return "setreuid";
    case 114:
        return "setregid";
    case 115:
        return "getgroups";
    case 116:
        return "setgroups";
    case 117:
        return "setresuid";
    case 118:
        return "getresuid";
    case 119:
        return "setresgid";
    case 120:
        return "getresgid";
    case 121:
        return "getpgid";
    case 122:
        return "setfsuid";
    case 123:
        return "setfsgid";
    case 124:
        return "getsid";
    case 125:
        return "capget";
    case 126:
        return "capset";
    case 127:
        return "rt_sigpending";
    case 128:
        return "rt_sigtimedwait";
    case 129:
        return "rt_sigqueueinfo";
    case 130:
        return "rt_sigsuspend";
    case 131:
        return "sigaltstack";
    case 132:
        return "utime";
    case 133:
        return "mknod";
    case 134:
        return "uselib";
    case 135:
        return "personality";
    case 136:
        return "ustat";
    case 137:
        return "statfs";
    case 138:
        return "fstatfs";
    case 139:
        return "sysfs";
    case 140:
        return "getpriority";
    case 141:
        return "setpriority";
    case 142:
        return "sched_setparam";
    case 143:
        return "sched_getparam";
    case 144:
        return "sched_setscheduler";
    case 145:
        return "sched_getscheduler";
    case 146:
        return "sched_get_priority_max";
    case 147:
        return "sched_get_priority_min";
    case 148:
        return "sched_rr_get_interval";
    case 149:
        return "mlock";
    case 150:
        return "munlock";
    case 151:
        return "mlockall";
    case 152:
        return "munlockall";
    case 153:
        return "vhangup";
    case 154:
        return "modify_ldt";
    case 155:
        return "pivot_root";
    case 156:
        return "_sysctl";
    case 157:
        return "prctl";
    case 158:
        return "arch_prctl";
    case 159:
        return "adjtimex";
    case 160:
        return "setrlimit";
    case 161:
        return "chroot";
    case 162:
        return "sync";
    case 163:
        return "acct";
    case 164:
        return "settimeofday";
    case 165:
        return "mount";
    case 166:
        return "umount2";
    case 167:
        return "swapon";
    case 168:
        return "swapoff";
    case 169:
        return "reboot";
    case 170:
        return "sethostname";
    case 171:
        return "setdomainname";
    case 172:
        return "iopl";
    case 173:
        return "ioperm";
    case 174:
        return "create_module";
    case 175:
        return "init_module";
    case 176:
        return "delete_module";
    case 177:
        return "get_kernel_syms";
    case 178:
        return "query_module";
    case 179:
        return "quotactl";
    case 180:
        return "nfsservctl";
    case 181:
        return "getpmsg";
    case 182:
        return "putpmsg";
    case 183:
        return "afs_syscall";
    case 184:
        return "tuxcall";
    case 185:
        return "security";
    case 186:
        return "gettid";
    case 187:
        return "readahead";
    case 188:
        return "setxattr";
    case 189:
        return "lsetxattr";
    case 190:
        return "fsetxattr";
    case 191:
        return "getxattr";
    case 192:
        return "lgetxattr";
    case 193:
        return "fgetxattr";
    case 194:
        return "listxattr";
    case 195:
        return "llistxattr";
    case 196:
        return "flistxattr";
    case 197:
        return "removexattr";
    case 198:
        return "lremovexattr";
    case 199:
        return "fremovexattr";
    case 200:
        return "tkill";
    case 201:
        return "time";
    case 202:
        return "futex";
    case 203:
        return "sched_setaffinity";
    case 204:
        return "sched_getaffinity";
    case 205:
        return "set_thread_area";
    case 206:
        return "io_setup";
    case 207:
        return "io_destroy";
    case 208:
        return "io_getevents";
    case 209:
        return "io_submit";
    case 210:
        return "io_cancel";
    case 211:
        return "get_thread_area";
    case 212:
        return "lookup_dcookie";
    case 213:
        return "epoll_create";
    case 214:
        return "epoll_ctl_old";
    case 215:
        return "epoll_wait_old";
    case 216:
        return "remap_file_pages";
    case 217:
        return "getdents64";
    case 218:
        return "set_tid_address";
    case 219:
        return "restart_syscall";
    case 220:
        return "semtimedop";
    case 221:
        return "fadvise64";
    case 222:
        return "timer_create";
    case 223:
        return "timer_settime";
    case 224:
        return "timer_gettime";
    case 225:
        return "timer_getoverrun";
    case 226:
        return "timer_delete";
    case 227:
        return "clock_settime";
    case 228:
        return "clock_gettime";
    case 229:
        return "clock_getres";
    case 230:
        return "clock_nanosleep";
    case 231:
        return "exit_group";
    case 232:
        return "epoll_wait";
    case 233:
        return "epoll_ctl";
    case 234:
        return "tgkill";
    case 235:
        return "utimes";
    case 236:
        return "vserver";
    case 237:
        return "mbind";
    case 238:
        return "set_mempolicy";
    case 239:
        return "get_mempolicy";
    case 240:
        return "mq_open";
    case 241:
        return "mq_unlink";
    case 242:
        return "mq_timedsend";
    case 243:
        return "mq_timedreceive";
    case 244:
        return "mq_notify";
    case 245:
        return "mq_getsetattr";
    case 246:
        return "kexec_load";
    case 247:
        return "waitid";
    case 248:
        return "add_key";
    case 249:
        return "request_key";
    case 250:
        return "keyctl";
    case 251:
        return "ioprio_set";
    case 252:
        return "ioprio_get";
    case 253:
        return "inotify_init";
    case 254:
        return "inotify_add_watch";
    case 255:
        return "inotify_rm_watch";
    case 256:
        return "migrate_pages";
    case 257:
        return "openat";
    case 258:
        return "mkdirat";
    case 259:
        return "mknodat";
    case 260:
        return "fchownat";
    case 261:
        return "futimesat";
    case 262:
        return "newfstatat";
    case 263:
        return "unlinkat";
    case 264:
        return "renameat";
    case 265:
        return "linkat";
    case 266:
        return "symlinkat";
    case 267:
        return "readlinkat";
    case 268:
        return "fchmodat";
    case 269:
        return "faccessat";
    case 270:
        return "pselect6";
    case 271:
        return "ppoll";
    case 272:
        return "unshare";
    case 273:
        return "set_robust_list";
    case 274:
        return "get_robust_list";
    case 275:
        return "splice";
    case 276:
        return "tee";
    case 277:
        return "sync_file_range";
    case 278:
        return "vmsplice";
    case 279:
        return "move_pages";
    case 280:
        return "utimensat";
    case 281:
        return "epoll_pwait";
    case 282:
        return "signalfd";
    case 283:
        return "timerfd_create";
    case 284:
        return "eventfd";
    case 285:
        return "fallocate";
    case 286:
        return "timerfd_settime";
    case 287:
        return "timerfd_gettime";
    case 288:
        return "accept4";
    case 289:
        return "signalfd4";
    case 290:
        return "eventfd2";
    case 291:
        return "epoll_create1";
    case 292:
        return "dup3";
    case 293:
        return "pipe2";
    case 294:
        return "inotify_init1";
    case 295:
        return "preadv";
    case 296:
        return "pwritev";
    case 297:
        return "rt_tgsigqueueinfo";
    case 298:
        return "perf_event_open";
    case 299:
        return "recvmmsg";
    case 300:
        return "fanotify_init";
    case 301:
        return "fanotify_mark";
    case 302:
        return "prlimit64";
    case 303:
        return "name_to_handle_at";
    case 304:
        return "open_by_handle_at";
    case 305:
        return "clock_adjtime";
    case 306:
        return "syncfs";
    case 307:
        return "sendmmsg";
    case 308:
        return "setns";
    case 309:
        return "getcpu";
    case 310:
        return "process_vm_readv";
    case 311:
        return "process_vm_writev";
    case 312:
        return "kcmp";
    case 313:
        return "finit_module";
    default:
        sprintf(buffer, "%ld", id);
        return buffer;
    }
}
