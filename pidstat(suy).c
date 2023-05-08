#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys.types.h>
#include <pwd.h>
#include <grp.h>


void function(char* dirname)           
{
	DIR* dir_ptr;                      // 폴더 파일 디스크립터
	struct dirent* direntp;			   // each entry
	char path[256];                    // 현재 폴더의 경로, 길이는 최대 256

	if (chdir(dirname) != 0)           // 현재 보고있는 폴더 위치 변경
	{
		perror(dirname);               // 만약 폴더가 존재하지 않는다면 에러출력 (error handling)
	}

	getcwd(path, sizeof(path));        // 현재 보고 있는 폴더 위치를 path변수에 저장

	
	if ((dir_ptr = opendir(path)) == NULL)  // 폴더가 열리지 않을 때
		fprintf(stderr, "폴더를 열 수 없습니다.");    // error handling
	else
	{
		while ((direntp = readdir(dir_ptr)) != NULL)   // 폴더 내의 파일을 탐색
		{
			if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
			{
				continue; 
			}
			/*********************************************************
			*  폴더내의 원하는 파일을 찾으면 나오는 조건문 추가    ex) /proc에서 /proc/pid]/stat(특정 프로세스에 대한 정보)을 찾아서 확인
			*  struct stat을 이용해서 해당 파일의 정보를 조회가능
			* 
			*  st_mode type and permission
			*  st_uid ID of owner
			*  st_gid ID of group
			*  st_size number of bytes in file
			*  st_nlink number of links to file
			*  st_mtime last content-modified time
			*  st_atime last-accessed time
			*  st_ctime last properties-changed time
			* 
			*  해당 UID를 사용하여 struct passwd에 정의된 정보를 가져올 수 있음
			*  
			*  struct passwd
			*  char* pw_name;  // 유저이름
			*  char* pw_passwd // 비밀번호
			*  uid_t pw_uid;   // 유저 ID
			*  gid_t pw_gid;   // 그룹 ID
			*  char* pw_gecos; // 실제 이름
			*  char* pw_dir    // 홈 디렉토리
			*  char* pw_shell  // 쉘
			* 
			* 
			* *******************************************************/
		}
		
	}





}

int main(int argc, char* argv[])
{
	char* arglist[3];                  // 인자의 내용을 저장하는 문자열
	for (int i = 1; i < argc, i++)     // 인자를 저장
	{
		strcpy(arglist[i], argv[i]);
	}
	/*******************************************************************
	* 
	*  fork()로 자식 프로세스를 나누어서, 인자로 들어온 명령을 실행함
	*  pid_t pid = fork(); // 자식 프로세스 생성
	* 
	*   if (pid < 0)
	*   { // fork 실패
    *      fprintf(stderr, "fork failed\n");
    *      exit(1);
	*   } 
	*   else if (pid == 0)
	*   { // 자식 프로세스
    *       printf("Child process started\n");
    *       exit(2); // 종료 상태 설정
    *   } 
    *   else
		{ // 부모 프로세스
            int status;
            pid_t child_pid = waitpid(pid, &status, 0); // 자식 프로세스의 종료 상태 읽기 
            // waitpid()실행 후, status 인자에, 종료된 자식 프로세스의 상태정보가 저장이 됨. by. 구글
            if (child_pid < 0)
			{ // waitpid 실패
                fprintf(stderr, "waitpid failed\n");
                exit(1);
            }
			else 
			{ // 자식 프로세스가 종료되었음
                if (WIFEXITED(status)) 
				{ // 자식 프로세스가 정상 종료되었을 경우
                    printf("Child process exited with status %d\n", WEXITSTATUS(status));
                } 
			    else if (WIFSIGNALED(status)) 
				{ // 자식 프로세스가 시그널에 의해 종료되었을 경우
                    printf("Child process terminated by signal %d\n", WTERMSIG(status));
                }
				else
				{
                    fprintf(stderr, "Unknown exit status\n");
                    exit(1);
                }
            }
        }
	*  
	* 
	* 
	* 위의 코드를 통해 fork()를 통해 나눈 자식 프로세스에서
	* 인자로 들어온 명령을 실행하고 종료한 뒤
	* 해당 종료 프로세스의 정보를 부모프로세스에서 읽어올 수 있다
	* 
	* 
	* 
	* *****************************************************************/



	execvp(arglist[0], arglist);       // 해당 인자를 바탕으로 파일 실행


}
