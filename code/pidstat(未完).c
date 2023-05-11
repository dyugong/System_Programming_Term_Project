#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>


int main(int argc, char* argv[])
{
   pid_t pid;
   int status;
   struct rusage usage;
   
   pid = fork(); // 자식 프로세스 생성
   

   if (pid < 0)
   { // fork 실패
      fprintf(stderr, "fork failed\n");
        exit(1);
   } 
   else if (pid == 0)
   { // 자식 프로세스    
      execvp(argv[0], argv);
      exit(0);
    } 
   else
   { // 부모 프로세스
      wait(&status);
      if (getrusage(pid, &usage) == -1) 
      {
         perror("getrusage");
         exit(1);
      }

   }
   printf("maxrss  ixrss  idrss  isrss  inblock  outblock  command\n");
   printf("%ld  %ld  %ld  %ld  %ld  %ld  %s\n", usage.ru_maxrss, usage.ru_ixrss, usage.ru_idrss, usage.ru_isrss, usage.ru_inblock, usage.ru_oublock, argv[0]);


}
