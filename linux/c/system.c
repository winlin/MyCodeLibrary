#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

typedef void (*sighandler_t)(int);

int posix_system(const char *cmd_line)
{
	int ret = 0;
	sighandler_t old_handler;
	old_handler = signal(SIGCHLD, SIG_DFL);
	ret = system(cmd_line);
	signal(SIGCHLD, old_handler);

	if(ret != 0) {
		printf("call system(%s) faild:%s", cmd_line, strerror(errno));
	}
	return ret;
}

int main(int argc, char *argv[])
{
	posix_system(argv[1]);
	return 0;
}

