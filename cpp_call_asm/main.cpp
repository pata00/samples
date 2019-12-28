#include <cstdio>
extern "C" int neg_use_asm(int a);

int neg_use_c(int a) 
{ 
	return -a;
}

int main()
{
	printf("%d\n", neg_use_c(-2147483648));
	printf("%d\n", neg_use_asm(-2147483648));
	printf("%d\n", neg_use_c(-2147483647));
	printf("%d\n", neg_use_asm(-2147483647));
	return 0;
}