#include "pidstat.h"
#include "strace.h"

#define PIPE_ENDS 2
#define oops2(m,x) { perror(m); exit(x); }
#define BUFFERSIZE 4096
#define COPYMODE 0644

int main(int argc, char** argv)
{
	int thepipe[PIPE_ENDS], newfd, pid,n_chars;
	char buf[BUFFERSIZE];

	stexecute(argc, argv);
	
	
	if (pipe(thepipe) == -1)		// get pipe
		oops2("Cannot get a pipe", 1);

	if ((pid = fork()) == -1)
		oops2("Cannot fork", 2);
	if (pid == 0) {
		close(thepipe[0]);
		if (dup2(thepipe[1], 1) == -1)
			oops2("couldn't redirect stdout", 4);
		close(thepipe[1]);
		pidstat(argc, argv);
		stexecute(argc, argv);
	}
	else {
		close(thepipe[1]);
		if (dup2(thepipe[0], 0) == -1)
			oops2("couldn't redirect stdin", 3);
		close(thepipe[0]);
		if ((newfd = creat("result.txt", COPYMODE)) == -1)
			oops2("Cannot open ", 1);
		
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