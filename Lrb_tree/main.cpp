/*作者：大昱 
来源：CSDN 
原文：https://blog.csdn.net/a29562268/article/details/80809344 
版权声明：本文为博主原创文章，转载请附上博文链接！*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testRb_tree.h"

int main()
{

	printf("123\r\n");	

	Lr_Rb_tree tree;
	printf("%d \r\n",tree.Lr_insert(10));
	tree.Lr_insert(20);
	tree.Lr_insert(30);
	tree.Lr_insert(50);
	tree.Lr_insert(40);
	tree.Lr_insert(100);
	tree.Lr_insert(60);
	tree.Lr_insert(70);
	tree.Lr_insert(80);
	tree.Lr_insert(90);
	tree.Lr_insert(120);
	tree.Lr_insert(110);
	char * Test = tree.ShowNodeInfo();
	tree.Lr_earse(100);
	tree.Lr_earse(50);
	tree.Lr_earse(30);
	tree.Lr_earse(80);
	Test = tree.ShowNodeInfo();
	return 0;
}
