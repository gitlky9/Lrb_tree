#include "testRb_tree.h"

Lr_Rb_tree::Lr_Rb_tree():m_NodeInfo(0)
{
	m_head._M_color = Lr_Rb_tree_color(-1);
}

Lr_Rb_tree::~Lr_Rb_tree()
{
	if (m_NodeInfo)
		delete m_NodeInfo;
}

bool  Lr_Rb_tree::CreateHead(unsigned int val)
{
	return Init_node(val);
}

//初始化
bool Lr_Rb_tree::Init_node(unsigned int val)
{
	if (_S_red != m_head._M_color && _S_black != m_head._M_color)
	{
		m_head._M_left = &m_head;
		m_head._M_right = &m_head;
		m_head._M_parent = 0;
		m_head._M_color = _S_red;
		m_head._val = val;
		m_node_count = 0;
		return true;
	}
	return false;
}

//创建节点 
Lr_Rb_tree_node_base* Lr_Rb_tree::Create_Rb_tree_node(unsigned int val)
{
	Lr_Rb_tree_node_base* ptr = new Lr_Rb_tree_node_base;
	memset(ptr,0,sizeof(ptr));
	ptr->_M_color = _S_red;
	ptr->_val = val;
	return ptr;
}

//添加树数据
bool Lr_Rb_tree::Lr_insert(unsigned int val)
{
	//判断是否有头节点 如果没有创建头节点
	if(CreateHead(val))
		return true;

	Lr_Rb_tree_Ptr x = Begin();
	Lr_Rb_tree_Ptr y = End();
	Lr_Rb_tree_Ptr pbound;
	//首先检查val是否与之前插入过的数据有重复
	if (0 == (pbound = Lr_M_lower_bound(x,y,val)))
		return false;

	//增加数节点以及插入数据
	return Lr_M_insert_unique_(pbound,val);
	return true;
}

//
bool Lr_Rb_tree::Lr_M_insert_unique_(Lr_Rb_tree_Ptr position, const unsigned int & __v)
{

	// end()
	if (position == End())
	{
		if (size() > 0 && 1 == compare_less(Right(), __v))
			return Lr_M_insert_(0, Right(), __v);
		else
			return Lr_M_insert_unique(__v);
	}
	else if (-1 == compare_less(position, __v))
	{
		if (position == Left()) // begin()
			return Lr_M_insert_(Left(), Left(), __v);
		else if (position->_M_parent)
		{
			if (position->_M_parent->_M_right == 0)
				return Lr_M_insert_(0, position->_M_parent, __v);
			else
				return Lr_M_insert_(position, position, __v);
		}
		else
			return Lr_M_insert_unique(__v);
	}
	else if (1 == compare_less(position, __v))
	{
		// ... then try after.
		if (position == Right())
			return Lr_M_insert_(0, Right(), __v);

		else if (1 == compare_less(position->_M_right, __v))
		{
			if (Right(position->_M_right) == 0)
				return Lr_M_insert_(0, position->_M_right, __v);
			else
				return Lr_M_insert_(position->_M_right, position->_M_right, __v);
		}
		else
			return Lr_M_insert_unique(__v);
	}
		return false;
}

//
bool Lr_Rb_tree::Lr_M_insert_unique(const unsigned int& __v)
{
	Lr_Rb_tree_Ptr __x = Begin();
	Lr_Rb_tree_Ptr __y = End();
	bool __comp = true;
	while (__x != 0)
	{
		__y = __x;
		__comp = compare_less(__x,__v);
		__x = __comp ? Left(__x) : Right(__x);
	}
	Lr_Rb_tree_Ptr __j = __y;
	if (__comp)
	{
		if (__y == Begin())
			return Lr_M_insert_(__x, __y, __v);
		else
			__j = __j->_M_left;
	}
	if (1 == compare_less(__j,__v))
		return Lr_M_insert_(__x, __y, __v);
	return false;
}

//
long long Lr_Rb_tree::size()
{
	return m_node_count;
}

//
bool Lr_Rb_tree::Lr_M_insert_(Lr_Rb_tree_Ptr x,Lr_Rb_tree_Ptr y,unsigned int val)
{
	bool __insert_left = (x != 0 || y == End()
		
		|| -1 == compare_less(y,val));

	Lr_Rb_tree_Ptr z = Create_Rb_tree_node(val);
	Lr_Rb_tree_insert_and_rebalance(__insert_left, z,
		y,  this->m_head);
	++m_node_count;
	return true;
}


//
int Lr_Rb_tree::compare_less(Lr_Rb_tree_Ptr x,unsigned int val)
{
	if(0 == x)
	return 0;

	if (x->_val < val)
	{
		return 1;
	}
	else if (x->_val > val)
	{
		return -1;
	}
	return 0;
}


//返回需要插入节点的位置
Lr_Rb_tree_node_base* Lr_Rb_tree::Lr_M_lower_bound(Lr_Rb_tree_Ptr x,Lr_Rb_tree_Ptr y,unsigned int val)
{
	while (x != 0)
	{
		unsigned int nless = compare_less(x, val);

		if (1 == nless)
			x = Right(x);
		else if(-1 == nless)
		{
			y = x,x = Left(x);
		}
		else//如果相等，这条数据不再插入，直接返回
			return 0;	
	}

	return y;
}

//返回查找到的节点位置，如果没有返回空
Lr_Rb_tree_node_base* Lr_Rb_tree::Lr_FindNode(unsigned int val)
{
	//临时保存需要插入的节点位置
	Lr_Rb_tree_Ptr x = Begin();
	while (x != 0)
	{
		int nless = compare_less(x, val);
		if (-1 == nless)
			x = Left(x);
		else if (1 == nless)
			x = Right(x);
		else//如果相等，返回这条数据
			return x;
	}
	return 0;
}


//
Lr_Rb_tree_node_base* Lr_Rb_tree::Begin()
{
	return m_head._M_parent;
}

//
Lr_Rb_tree_node_base* Lr_Rb_tree::End()
{
	return &m_head;
}

//
Lr_Rb_tree_node_base* Lr_Rb_tree::Left()
{
	return m_head._M_left;
}

//
Lr_Rb_tree_node_base* Lr_Rb_tree::Left(Lr_Rb_tree_Ptr ptr)
{
	return ptr->_M_left;
}

//
Lr_Rb_tree_node_base* Lr_Rb_tree::Right()
{
	return m_head._M_right;
}

//
Lr_Rb_tree_node_base* Lr_Rb_tree::Right(Lr_Rb_tree_Ptr ptr)
{
	return ptr->_M_right;
}

//
Lr_Rb_tree_node_base* Lr_Rb_tree::Minimum(Lr_Rb_tree_Ptr ptr)
{
	while (ptr->_M_left != 0) ptr = ptr->_M_left;
	return ptr;
}

//
Lr_Rb_tree_node_base* Lr_Rb_tree::Maximum(Lr_Rb_tree_Ptr ptr)
{
	while (ptr->_M_right != 0) 
		ptr = ptr->_M_right;
	return ptr;
}

void Lr_Rb_tree::Lr_Rb_tree_insert_and_rebalance(const bool __insert_left,
	Lr_Rb_tree_node_base* __x,
	Lr_Rb_tree_node_base* __p,
	Lr_Rb_tree_node_base& __header)//红黑树结构内核实现
{
	Lr_Rb_tree_node_base **rootptrptr = &__header._M_parent;

	__x->_M_parent = __p;
	__x->_M_left = 0;
	__x->_M_right = 0;
	__x->_M_color = _S_red;

	if(__insert_left){
		__p->_M_left = __x;
		if(__p == &__header){
			__header._M_parent = __x;
			__header._M_right = __x;
		}else if( __p == __header._M_left )
			__header._M_left = __x;
	}else{
		__p->_M_right = __x;
		if(__p == __header._M_right)
			__header._M_right = __x;
	}

	while( __x != *rootptrptr && __x->_M_parent->_M_color==_S_red ){
		Lr_Rb_tree_node_base* const xpp = __x->_M_parent->_M_parent;
		if(__x->_M_parent == xpp->_M_left){
			Lr_Rb_tree_node_base* const y = xpp->_M_right;
			if(y && y->_M_color == _S_red){
				__x->_M_parent->_M_color = _S_black;
				y->_M_color = _S_black;
				xpp->_M_color = _S_red;
				__x = xpp;
			}else{
				if( __x==__x->_M_parent->_M_right){
					__x = __x->_M_parent;

					_Rb_tree_rotate_left(__x,*rootptrptr);
				}
				__x->_M_parent->_M_color = _S_black;
				xpp->_M_color = _S_red;
				_Rb_tree_rotate_right(xpp,*rootptrptr);
			}
		}else{
			Lr_Rb_tree_node_base* const y = xpp->_M_left;
			if(y && y->_M_color == _S_red){
				__x->_M_parent->_M_color = _S_black;
				y->_M_color = _S_black;
				xpp->_M_color = _S_red;
				__x = xpp;
			}else{
				if(__x == __x->_M_parent->_M_left){
					__x = __x->_M_parent;
					_Rb_tree_rotate_right(__x,*rootptrptr);
				}
				__x->_M_parent->_M_color = _S_black;
				xpp->_M_color = _S_red;
				_Rb_tree_rotate_left(xpp,*rootptrptr);
			}
		}
	}
	(*rootptrptr)->_M_color = _S_black;
	return ;
}


//删除树节点
bool Lr_Rb_tree::Lr_earse(unsigned int val)
{
	if (0 >= size())
		return false;

	Lr_Rb_tree_Ptr ptr = Lr_FindNode(val);
	if (0 == ptr)
		return false;
	Lr_Rb_tree_rebalance_for_erase(ptr,*Begin(),End()->_M_left,End()->_M_right);
	Delete_Rb_tree_node(ptr);
	--m_node_count;
	return true;
}

//删除节点
void Lr_Rb_tree::Delete_Rb_tree_node(Lr_Rb_tree_Ptr & ptr)
{
	if (ptr)
		delete ptr,ptr = 0;
}

//删除所有节点，不平衡树结构
void Lr_Rb_tree::Lr_M_erase(Lr_Rb_tree_Ptr ptr)
{
	while (ptr != 0)
	{
		Lr_M_erase(Right(ptr));
		Lr_Rb_tree_Ptr left = Left(ptr);
		Delete_Rb_tree_node(ptr);
		ptr = left;
	}
}

//删除所有节点
/**
*这个接口弃用，这个版本的红黑树做为实例参考用就行，后期把c++智能框架移植过来
*/
void Lr_Rb_tree::All_Delete_Rb_tree_node()
{
	Lr_M_erase(Begin());
	m_node_count = 0;
	m_head._M_color = Lr_Rb_tree_color(-1);
	m_head._M_parent = 0;
	m_head._M_left = &m_head;
	m_head._M_right = &m_head;
}

Lr_Rb_tree_node_base* Lr_Rb_tree::Lr_Rb_tree_rebalance_for_erase(Lr_Rb_tree_node_base* __z,
	Lr_Rb_tree_node_base& __root,
	Lr_Rb_tree_node_base*& __leftmost,
	Lr_Rb_tree_node_base*& __rightmost)
{
	Lr_Rb_tree_node_base* __y = __z;
	Lr_Rb_tree_node_base* __x = 0;
	Lr_Rb_tree_node_base* __x_parent = 0;
	Lr_Rb_tree_node_base* __root_ = &__root;
	if (__y->_M_left == 0)     // __z has at most one non-null child. y == z.
		__x = __y->_M_right;     // __x might be null.
	else
		if (__y->_M_right == 0)  // __z has exactly one non-null child. y == z.
			__x = __y->_M_left;    // __x is not null.
		else {                   // __z has two non-null children.  Set __y to
			__y = __y->_M_right;   //   __z's successor.  __x might be null.
			while (__y->_M_left != 0)
				__y = __y->_M_left;
			__x = __y->_M_right;
		}
		if (__y != __z) {          // relink y in place of z.  y is z's successor
			__z->_M_left->_M_parent = __y; 
			__y->_M_left = __z->_M_left;
			if (__y != __z->_M_right) {
				__x_parent = __y->_M_parent;
				if (__x) __x->_M_parent = __y->_M_parent;
				__y->_M_parent->_M_left = __x;      // __y must be a child of _M_left
				__y->_M_right = __z->_M_right;
				__z->_M_right->_M_parent = __y;
			}
			else
				__x_parent = __y;  
			if (__root_ == __z)
				__root_ = __y;
			else if (__z->_M_parent->_M_left == __z)
				__z->_M_parent->_M_left = __y;
			else 
				__z->_M_parent->_M_right = __y;
			__y->_M_parent = __z->_M_parent;
			//swap(__y->_M_color, __z->_M_color);
			__y = __z;
			// __y now points to node to be actually deleted
		}
		else {                        // __y == __z
			__x_parent = __y->_M_parent;
			if (__x) __x->_M_parent = __y->_M_parent;   
			if (__root_ == __z)
				__root_ = __x;
			else 
				if (__z->_M_parent->_M_left == __z)
					__z->_M_parent->_M_left = __x;
				else
					__z->_M_parent->_M_right = __x;
			if (__leftmost == __z) 
				if (__z->_M_right == 0)        // __z->_M_left must be null also
					__leftmost = __z->_M_parent;
			// makes __leftmost == _M_header if __z == __root
				else
					__leftmost = Minimum(__x);
			if (__rightmost == __z)  
				if (__z->_M_left == 0)         // __z->_M_right must be null also
					__rightmost = __z->_M_parent;  
			// makes __rightmost == _M_header if __z == __root
				else                      // __x == __z->_M_left
					__rightmost = Maximum(__x);
		}
		if (__y->_M_color != _S_red) { 
			while (__x != __root_ && (__x == 0 || __x->_M_color == _S_black))
				if (__x == __x_parent->_M_left) {
					Lr_Rb_tree_node_base* __w = __x_parent->_M_right;
					if (__w->_M_color == _S_red) {
						__w->_M_color = _S_black;
						__x_parent->_M_color = _S_red;
						_Rb_tree_rotate_left(__x_parent, __root_);
						__w = __x_parent->_M_right;
					}
					if ((__w->_M_left == 0 || 
						__w->_M_left->_M_color == _S_black) &&
						(__w->_M_right == 0 || 
						__w->_M_right->_M_color == _S_black)) {
							__w->_M_color = _S_red;
							__x = __x_parent;
							__x_parent = __x_parent->_M_parent;
					} else {
						if (__w->_M_right == 0 || 
							__w->_M_right->_M_color == _S_black) {
								if (__w->_M_left) __w->_M_left->_M_color = _S_black;
								__w->_M_color = _S_red;
								_Rb_tree_rotate_right(__w, __root_);
								__w = __x_parent->_M_right;
						}
						__w->_M_color = __x_parent->_M_color;
						__x_parent->_M_color = _S_black;
						if (__w->_M_right) __w->_M_right->_M_color = _S_black;
						_Rb_tree_rotate_left(__x_parent, __root_);
						break;
					}
				} else {                  // same as above, with _M_right <-> _M_left.
					Lr_Rb_tree_node_base* __w = __x_parent->_M_left;
					if (__w->_M_color == _S_red) {
						__w->_M_color = _S_black;
						__x_parent->_M_color = _S_red;
						_Rb_tree_rotate_right(__x_parent, __root_);
						__w = __x_parent->_M_left;
					}
					if (__w && (__w->_M_right == 0 || 
						__w->_M_right->_M_color == _S_black) &&
						(__w->_M_left == 0 || 
						__w->_M_left->_M_color == _S_black)) {
							__w->_M_color = _S_red;
							__x = __x_parent;
							__x_parent = __x_parent->_M_parent;
					} else {
						if (__w && (__w->_M_left == 0 || 
							__w->_M_left->_M_color == _S_black)) {
								if (__w->_M_right) __w->_M_right->_M_color = _S_black;
								__w->_M_color = _S_red;
								_Rb_tree_rotate_left(__w, __root_);
								__w = __x_parent->_M_left;
						}
						__w->_M_color = __x_parent->_M_color;
						__x_parent->_M_color = _S_black;
						if (__w->_M_left) __w->_M_left->_M_color = _S_black;
						_Rb_tree_rotate_right(__x_parent, __root_);
						break;
					}
				}
				if (__x) __x->_M_color = _S_black;
		}
		return __y;
}

void Lr_Rb_tree::_Rb_tree_rotate_left(Lr_Rb_tree_node_base* __x, Lr_Rb_tree_node_base*& __root)//节点左旋转
{
	Lr_Rb_tree_node_base* __y = __x->_M_right;
	__x->_M_right = __y->_M_left;
	if (__y->_M_left !=0)
		__y->_M_left->_M_parent = __x;
	__y->_M_parent = __x->_M_parent;

	if (__x == __root)
		__root = __y;
	else if (__x == __x->_M_parent->_M_left)
		__x->_M_parent->_M_left = __y;
	else
		__x->_M_parent->_M_right = __y;
	__y->_M_left = __x;
	__x->_M_parent = __y;
}

void Lr_Rb_tree::_Rb_tree_rotate_right(Lr_Rb_tree_node_base* __x, Lr_Rb_tree_node_base*& __root)//节点右旋转
{
	Lr_Rb_tree_node_base* __y = __x->_M_left;
	__x->_M_left = __y->_M_right;
	if (__y->_M_right != 0)
		__y->_M_right->_M_parent = __x;
	__y->_M_parent = __x->_M_parent;

	if (__x == __root)
		__root = __y;
	else if (__x == __x->_M_parent->_M_right)
		__x->_M_parent->_M_right = __y;
	else
		__x->_M_parent->_M_left = __y;
	__y->_M_right = __x;
	__x->_M_parent = __y;
}

//显示节点信息
char * Lr_Rb_tree::ShowNodeInfo()
{

	if (0 == m_node_count)
		return 0;
	if (0 != m_NodeInfo)
	{
		delete m_NodeInfo;
		m_NodeInfo = new char[m_node_count * 50];
		memset(m_NodeInfo,0,sizeof(m_NodeInfo));
	}
	else
	{
		m_NodeInfo = new char[m_node_count * 50];
		memset(m_NodeInfo,0,sizeof(m_NodeInfo));
	}
	int noffset = 0;
	int nVal = 0;
	return GetNodeInfo(WriteRootInfo(m_NodeInfo,&m_head,noffset,nVal),&m_head,noffset,nVal,-1);
}

//写入root信息
char * Lr_Rb_tree::WriteRootInfo(char *nodeInfo,Lr_Rb_tree_node_base * node,int & noffset,int & nVal)
{
	if (0 == nodeInfo)
		return 0;
	if (node)
	{
		sprintf(nodeInfo,"The root node is %d\r\n",node->_val);
		noffset = strlen(nodeInfo);
		nVal = node->_val;
		*(nodeInfo + nVal) = 0;
	}
	return nodeInfo;
}

//节点信息写入字符串
char * Lr_Rb_tree::GetNodeInfo(char *nodeInfo,Lr_Rb_tree_node_base * node,int & noffset,int nVal,int left)
{
	if (0 == nodeInfo)
		return 0;
	if (node)
	{
		if (-1 != left)
			WriteNodeInfo(nodeInfo,node,noffset,nVal,left);

		if (node->_M_left)
			GetNodeInfo(nodeInfo,node->_M_left,noffset,node->_val,1);
		if (node->_M_right)
			GetNodeInfo(nodeInfo,node->_M_right,noffset,node->_val,0);
	}
	return nodeInfo;
}

//写入节点数据
char * Lr_Rb_tree::WriteNodeInfo(char *nodeInfo,Lr_Rb_tree_node_base * node,int & noffset,int nVal,bool left)
{
	char TempData[100] = {0};
	if (_S_red == node->_M_color)	
		sprintf(TempData,"%d is the red %s node of %d\r\n",node->_val,(left ? "left" : "right"),nVal);
	else if (_S_black == node->_M_color)
		sprintf(TempData,"%d is the black %s node of %d\r\n",node->_val,(left ? "left" : "right"),nVal);
		int nLen = strlen(TempData);
		memcpy(nodeInfo + noffset,TempData,nLen);
		noffset += nLen;
		*(nodeInfo + noffset) = 0;
		
		printf("%s",TempData);
		return nodeInfo;
}
