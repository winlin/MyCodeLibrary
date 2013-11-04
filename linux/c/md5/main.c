#include "md5func.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[])
{
	
	char md5str[33] = {0};
	time_t start = 0, end = 0;
	start = time(NULL);
	md5_file(argv[1], md5str);
	end = time(NULL);
	printf("Get file:%s  MD5:\n%s\n  Time:%lld\n", argv[1], md5str, end-start);

	memset(md5str, 0, 33);
	start = time(NULL);
	md5_file2(argv[1], md5str);
	end   = time(NULL);
	printf("Get file:%s  MD5:\n%s\n Time:%lld\n", argv[1], md5str, end-start);

	memset(md5str, 0, 33);
	start = time(NULL);
	md5_file3(argv[1], md5str);
	end   = time(NULL);
	printf("Get file:%s  MD5:\n%s\n Time:%lld\n", argv[1], md5str, end-start);

	return 0;

}