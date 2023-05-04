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
	DIR* dir_ptr;                      // ���� ���� ��ũ����
	struct dirent* direntp;			   // each entry
	char path[256];                    // ���� ������ ���, ���̴� �ִ� 256

	if (chdir(dirname) != 0)           // ���� �����ִ� ���� ��ġ ����
	{
		perror(dirname);               // ���� ������ �������� �ʴ´ٸ� ������� (error handling)
	}

	getcwd(path, sizeof(path));        // ���� ���� �ִ� ���� ��ġ�� path������ ����

	
	if ((dir_ptr = opendir(path)) == NULL)  // ������ ������ ���� ��
		fprintf(stderr, "������ �� �� �����ϴ�.");    // error handling
	else
	{
		while ((direntp = readdir(dir_ptr)) != NULL)   // ���� ���� ������ Ž��
		{
			if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
			{
				continue; 
			}
			/*********************************************************
			*  �������� ���ϴ� ������ ã���� ������ ���ǹ� �߰�    ex) /proc���� /proc/pid]/stat(Ư�� ���μ����� ���� ����)�� ã�Ƽ� Ȯ��
			*  struct stat�� �̿��ؼ� �ش� ������ ������ ��ȸ����
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
			*  �ش� UID�� ����Ͽ� struct passwd�� ���ǵ� ������ ������ �� ����
			*  
			*  struct passwd
			*  char* pw_name;  // �����̸�
			*  char* pw_passwd // ��й�ȣ
			*  uid_t pw_uid;   // ���� ID
			*  gid_t pw_gid;   // �׷� ID
			*  char* pw_gecos; // ���� �̸�
			*  char* pw_dir    // Ȩ ���丮
			*  char* pw_shell  // ��
			* 
			* 
			* *******************************************************/
		}
		
	}





}

passwd* uid_uid(uid_t uid)             // UID�� ������� ���� �о����
{
	return getpwuid(uid);
}

passwd* gid_gid(gid_t gid)             // GID�� ������� ���� �о����
{
	return getgrgid(gid);
}

int main(int argc, char* argv[])
{
	char* arglist[3];                  // ������ ������ �����ϴ� ���ڿ�
	for (int i = 1; i < argc, i++)     // ���ڸ� ����
	{
		strcpy(arglist[i], argv[i]);
	}
	/*******************************************************************
	* 
	*  fork()�� �ڽ� ���μ����� �����, ���ڷ� ���� ����� ������
	*  pid_t pid = fork(); // �ڽ� ���μ��� ����
	* 
	*   if (pid < 0)
	*   { // fork ����
    *      fprintf(stderr, "fork failed\n");
    *      exit(1);
	*   } 
	*   else if (pid == 0)
	*   { // �ڽ� ���μ���
    *       printf("Child process started\n");
    *       exit(2); // ���� ���� ����
    *   } 
    *   else
		{ // �θ� ���μ���
            int status;
            pid_t child_pid = waitpid(pid, &status, 0); // �ڽ� ���μ����� ���� ���� �б�

            if (child_pid < 0)
			{ // waitpid ����
                fprintf(stderr, "waitpid failed\n");
                exit(1);
            }
			else 
			{ // �ڽ� ���μ����� ����Ǿ���
                if (WIFEXITED(status)) 
				{ // �ڽ� ���μ����� ���� ����Ǿ��� ���
                    printf("Child process exited with status %d\n", WEXITSTATUS(status));
                } 
			    else if (WIFSIGNALED(status)) 
				{ // �ڽ� ���μ����� �ñ׳ο� ���� ����Ǿ��� ���
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
	* ���� �ڵ带 ���� fork()�� ���� ���� �ڽ� ���μ�������
	* ���ڷ� ���� ����� �����ϰ� ������ ��
	* �ش� ���� ���μ����� ������ �θ����μ������� �о�� �� �ִ�
	* 
	* 
	* 
	* *****************************************************************/



	execvp(arglist[0], arglist);       // �ش� ���ڸ� �������� ���� ����


}