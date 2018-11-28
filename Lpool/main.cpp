/*作者：大昱 
来源：CSDN 
原文：https://blog.csdn.net/a29562268/article/details/82846305 
版权声明：本文为博主原创文章，转载请附上博文链接！*/

#include <stdio.h>
#include "LMemoryPool.h"

int main()
{
	Lm_MemoryPool pool;
	char *Test1 = (char *)pool.Lm_MemAlloc(100);
	memcpy(Test1,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test1);
	memset(Test1,0,sizeof(Test1));
	char *Test2 = (char *)pool.Lm_MemAlloc(100);
	memcpy(Test2,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test2);
	memset(Test2,0,sizeof(Test2));
	char *Test3 = (char *)pool.Lm_MemAlloc(1024 * 1024 * 1024);
	memcpy(Test3,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test3);
	//memset(Test3,0,sizeof(Test3));
	char *Test4 = (char *)pool.Lm_MemAlloc(100 * 1024);
	memcpy(Test4,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test4);

	pool.Lm_MemFree(Test1);
	Test1 = 0;
	pool.Lm_MemFree(Test2);
	Test2 = 0;
	pool.Lm_MemFree(Test3);
	Test3 = 0;
	pool.Lm_MemFree(Test4);
	Test4 = 0;
	pool.Lm_MemPoolFree();

	Test1 = (char *)pool.Lm_MemAlloc(100);
	memcpy(Test1,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test1);
	memset(Test1,0,sizeof(Test1));
	Test2 = (char *)pool.Lm_MemAlloc(100);
	memcpy(Test2,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test2);
	memset(Test2,0,sizeof(Test2));
	Test3 = (char *)pool.Lm_MemAlloc(100 * 1024 * 1024);
	memcpy(Test3,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test3);
	memset(Test3,0,sizeof(Test3));
	Test4 = (char *)pool.Lm_MemAlloc(100 * 1024);
	memcpy(Test4,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test4);
	pool.Lm_MemPoolFree();

	Test1 = (char *)pool.Lm_MemAlloc(100);
	memcpy(Test1,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test1);
	memset(Test1,0,sizeof(Test1));
	Test2 = (char *)pool.Lm_MemAlloc(100);
	memcpy(Test2,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test2);
	memset(Test2,0,sizeof(Test2));
	Test3 = (char *)pool.Lm_MemAlloc(100 * 1024 * 1024);
	memcpy(Test3,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test3);
	memset(Test3,0,sizeof(Test3));
	Test4 = (char *)pool.Lm_MemAlloc(100 * 1024);
	memcpy(Test4,"您好12312321312",sizeof("您好12312321312"));
	printf("%s \r\n",Test4);
	return 0;
}
