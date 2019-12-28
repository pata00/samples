#include <cstdio>

int add_one_v1(int num) 
{ 
	return num + 1;
}



/*
	一般形式:
	asm ( 指令模板 
		: 输出操作数 
		[:输入操作数
		:[逗号分割的寄存器列表 ]])
*/


/*
	https://gcc.gnu.org/onlinedocs/gcc-9.2.0/gcc/Constraints.html#Constraints
	常用约束速查
	r	通用寄存器
	=	意味着指令需要进行写操作，之前的值会被丢弃并被新值覆盖
	+ 	指令需要读写操作
	x86平台特有约束
	a	ax寄存器
	D	di寄存器
*/

int add_one_v2(int num) {
	int ret;
	asm("movl %1, %0 \n\t"
		"add $1, %0 \n\t"
		//下面的a和D也可以直接用r替代,让编译器自己选择合适的寄存器
		: "=a" (ret)
		: "D" (num)
	);
	return ret;
}



int main()
{
	printf("%d\n", add_one_v1(100));
	printf("%d\n", add_one_v2(100));
	return 0;
}