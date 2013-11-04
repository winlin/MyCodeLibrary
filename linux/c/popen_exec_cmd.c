#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define MAXLINE 1024

int pipe_exec_cmd(const char *cmd_line)
{
	char result_buf[MAXLINE];
	int rc = 0;
	FILE *fp = popen(cmd_line, "r");
	if(NULL == fp) {
		perror("popen()failed");
		return -1;
	}
	while(fgets(result_buf, sizeof(result_buf), fp) != NULL)
	{
		if('\n' == result_buf[strlen(result_buf)-1]) {
			result_buf[strlen(result_buf)-1] = '\0';
		}
		printf("[cmd_line]:%s output:%s\n", cmd_line, result_buf);
		memset(result_buf, 0, MAXLINE);
	}
	// wait for the child process return
	rc = pclose(fp);
	if(-1 == rc) {
		perror("pclose() failed");
		return -1;
	}

	return rc;
}

int main(int argc, char *argv[])
{
	int ret = pipe_exec_cmd(argv[1]);
	printf("get func ret:%d\n", ret);
	return 0;
}


