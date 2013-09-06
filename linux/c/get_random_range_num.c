#include <stdio.h>
#include <stdlib.h>

int get_random_range_num()
{
	static unsigned char sign = 0;
	if (sign == 0xf) {
		sign = 0;
	}
	while(1) {
		int index = random()%4;
		if ((1<<index & sign)) continue;
		sign |= 1<<index;
		return index+1;
	}
}

int main()
{
	for (int i = 0; i < 80; ++i) {
		printf("random num=%d\n", get_random_range_num());
		if ((i+1)%4 == 0) printf("\n");
		}
	}
	return 0;
}