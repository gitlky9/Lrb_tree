/*******************************************************************************
*红黑树完整结构
*调试这个程序的时候发现，windows VS2010编译器与g++4.8编译器有着巨大的差异
*VS2010上运行比较稳定，到这里执行失败，等移植下一个产品时，重点测试这些问题
********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum Lr_Rb_tree_color { _S_red = false, _S_black = true};
struct Lr_Rb_tree_node_base
{
	typedef Lr_Rb_tree_node_base* Lr_Base_ptr;

	Lr_Rb_tree_color	_M_color;//颜色
	Lr_Base_ptr		_M_parent;//父节点
	Lr_Base_ptr		_M_left;//左节点
	Lr_Base_ptr		_M_right;//右节点
	//树值
	unsigned int _val;
};

/*struct Lr_Rb_tree_node : public Lr_Rb_tree_node_base//红黑树数据结构与键值的绑定
{
	Lr_Rb_tree_node* _node;
	unsigned int _val;
};*/

class Lr_Rb_tree : public Lr_Rb_tree_node_base//红黑树的创建、增加以及删除节点
{
public:
	typedef Lr_Rb_tree_node_base* Lr_Rb_tree_Ptr;
	Lr_Rb_tree();
	virtual ~Lr_Rb_tree();

public:
	//添加树节点
	bool Lr_insert(unsigned int val);
	//删除树节点
	bool Lr_earse(unsigned int val);
	//删除所有节点
	void All_Delete_Rb_tree_node();
	//显示节点信息
	char *ShowNodeInfo();
private:
	bool CreateHead(unsigned int val);
	//初始化
	bool Init_node(unsigned int val);
	//创建节点 
	Lr_Rb_tree_node_base* Create_Rb_tree_node(unsigned int val);
	//删除节点
	void Delete_Rb_tree_node(Lr_Rb_tree_Ptr & ptr);
	//删除所有节点，不平衡树结构
	void Lr_M_erase(Lr_Rb_tree_Ptr ptr);
	//树结构增加节点
	void Lr_Rb_tree_insert();
	//增加或删除节点后平衡树结构
	void Lr_Rb_tree_rebalance();
	//返回需要插入值的位置
	Lr_Rb_tree_node_base* Lr_M_lower_bound(Lr_Rb_tree_Ptr x,Lr_Rb_tree_Ptr y,unsigned int val);
	//返回查找到的节点位置，如果没有返回空
	Lr_Rb_tree_node_base* Lr_FindNode(unsigned int val);
	//
	Lr_Rb_tree_node_base* Begin();
	//
	Lr_Rb_tree_node_base* End();
	//
	Lr_Rb_tree_node_base* Left();
	//
	Lr_Rb_tree_node_base* Left(Lr_Rb_tree_Ptr ptr);
	//
	Lr_Rb_tree_node_base* Right();
	//
	Lr_Rb_tree_node_base* Right(Lr_Rb_tree_Ptr ptr);
	//
	Lr_Rb_tree_node_base* Minimum(Lr_Rb_tree_Ptr ptr);
	//
	Lr_Rb_tree_node_base* Maximum(Lr_Rb_tree_Ptr ptr);
	//比较结果小于val为1,如果相等返回0，如果大于val返回-1
	int compare_less(Lr_Rb_tree_Ptr x,unsigned int val);
	//
	bool Lr_M_insert_(Lr_Rb_tree_Ptr x,Lr_Rb_tree_Ptr y,unsigned int val);
	//
	bool Lr_M_insert_unique_(Lr_Rb_tree_Ptr position, const unsigned int & __v);
	//
	long long size();
	//
	bool Lr_M_insert_unique(const unsigned int& __v);
	//
	void Lr_Rb_tree_insert_and_rebalance(const bool __insert_left,
		Lr_Rb_tree_node_base* __x,
		Lr_Rb_tree_node_base* __p,
		Lr_Rb_tree_node_base& __header);//红黑树结构内核实现
	Lr_Rb_tree_node_base*
		Lr_Rb_tree_rebalance_for_erase(Lr_Rb_tree_node_base* __z,
		Lr_Rb_tree_node_base& __root,
		Lr_Rb_tree_node_base*& __leftmost,
		Lr_Rb_tree_node_base*& __rightmost);

	void _Rb_tree_rotate_left(Lr_Rb_tree_node_base* __x, Lr_Rb_tree_node_base*& __root);//节点左旋转
	void _Rb_tree_rotate_right(Lr_Rb_tree_node_base* __x, Lr_Rb_tree_node_base*& __root);//节点右旋转
	//写入root信息
	char * WriteRootInfo(char *nodeInfo,Lr_Rb_tree_node_base * node,int & noffset,int & nVal);
	//节点信息写入字符串
	char * GetNodeInfo(char *nodeInfo,Lr_Rb_tree_node_base * node,int & noffset,int nVal,int left);
	//写入节点数据
	char * WriteNodeInfo(char *nodeInfo,Lr_Rb_tree_node_base * node,int & noffset,int nVal,bool left);
private:
	Lr_Rb_tree_node_base m_head;
	long long m_node_count;
	char *m_NodeInfo;
};
