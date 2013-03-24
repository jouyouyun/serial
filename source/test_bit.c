#include <stdio.h>

int main()
{
	char str[] = "0123456789";
	char *p;

	p = str + 3;
	printf("p : %d\n", p[0] - '0');

	return 0;
}
