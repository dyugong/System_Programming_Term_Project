#include "pidstat.h"
#include "strace.h"

#define PIPE_ENDS 2
#define oops2(m,x) { perror(m); exit(x); }
#define BUFFERSIZE 4096
#define COPYMODE 0644

int main(int argc, char** argv)
{
	signal(SIGINT, SIG_IGN);		// SIGINT 무시
	int thepipe[PIPE_ENDS], newfd, pid, n_chars;
	char buf[BUFFERSIZE];

	if (pipe(thepipe) == -1)		// get pipe
		oops2("Cannot get a pipe", 1);

	if ((pid = fork()) == -1)
		oops2("Cannot fork", 2);
	if (pid == 0) {
		strace(argc, argv);		// strace 함수 실행(출력 결과를 보여줌과 동시에 strace.txt에 저장)
		pidstat(argc, argv);	// pidstat 함수 실행(출력 결과를 보여줌과 동시에 pidstat.txt에 저장)
		// child for writing end by using pipe
		close(thepipe[0]);
		if (dup2(thepipe[1], 1) == -1)
			oops2("couldn't redirect stdout", 4);
		close(thepipe[1]);
		FILE* sf,*pf;

		// strace.txt에 담겨있는 내용 출력
		if ((sf = fopen("strace.txt", "r")) == NULL) {
			perror("no file");
			exit(1);
		}
		char ststr[255];
		while (fgets(ststr, sizeof(ststr), sf) != NULL) {
			printf("%s", ststr);
		}
		printf("\n");
		fclose(sf);
		
		// pidstat.txt에 담겨있는 내용 출력
		if ((pf = fopen("pidstat.txt", "r")) == NULL) {
			perror("no file");
			exit(1);
		}
		char psstr[255];
		while (fgets(psstr, sizeof(psstr), pf) != NULL) {
			printf("%s", psstr);
		}
		fclose(pf);
	}
	else {
		// parent for reading end by using pipe
		close(thepipe[1]);
		if (dup2(thepipe[0], 0) == -1)
			oops2("couldn't redirect stdin", 3);
		close(thepipe[0]);
	
		// result.txt 생성(권한 rw-r--r--)
		if ((newfd = creat("result.txt", COPYMODE)) == -1)	
			oops2("Cannot open ", 1);
		
		// child에서 출력한 결과(strace & pidstat 출력 결과값)를 입력받아 result.txt에 쓰기
		while ((n_chars = read(0, buf, BUFFERSIZE)) > 0)	
			if (write(newfd, buf, n_chars) != n_chars)
				oops2("write error to", 2);
		if (n_chars == -1)
			oops2("read error from", 2);
		if (close(newfd) == -1)
			oops2("error closing file", 1);
	}
	
	return 0;
	
}