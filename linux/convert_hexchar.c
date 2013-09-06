#include <stdio.h>
#include <stdlib.h>

int convert_hexchar(char hexchar)
{
	char int_str[5] = {0};
	int num = -1;
	sprintf(int_str, "%c", hexchar);
	sscanf(int_str, "%x", &num);
	return num;
}

int main()
{
	printf("0xf=%d\n", just_for_fun('L'));

	return 0;
}