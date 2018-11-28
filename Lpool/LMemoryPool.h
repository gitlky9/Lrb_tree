/*
*李坤昱
*QQ：326087275@qq.com
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//默认单个内存池大小
#define MEMPOOLSIZE 100 * 1024 * 1024
//单个内存池预设值最小值
#define MEMPOOLMINI 1024 * 1024

//标记链表节点在内存池中的信息
struct MemNode
{
	//起始地址
	unsigned long StartAddress;
	//当前使用量
	unsigned long CurrentUsgeAmount;
};

typedef struct MemList
{
	//起始地址
	unsigned long StartAddress;
	//末尾地址
	unsigned long EndAddress;
	//当前使用量
	unsigned long CurrentUsgeAmount;
	//标记是否已经保存了数据
	bool bValid;
	MemList *Prev, *Next;
	//当前链表节点在内存池中的信息
	MemNode nodeInfo;
	void Init()
	{
		StartAddress = EndAddress = CurrentUsgeAmount = 0;
		bValid = 0;
		Prev = Next = 0;
	}
}* PMemList;


//内存管理表
//允许使用者追加分配内存，追加的内存将会保存在下一个结构中
typedef struct MemoryStore
{
	int Count;
	//总容量
	unsigned long MemVolumeDose;
	//起始地址
	unsigned long StartAddress;
	//末尾地址
	unsigned long EndAddress;
	//当前使用量
	unsigned long CurrentUsageAmount;
	//剩余容量
	unsigned long SurplusVolumeDose;
	MemoryStore *Prev, *Next;
	//存储数据起始地址
	unsigned long StartDataAddress;
	//当前存储数据偏移地址
	unsigned long StartDataOffsetAddress;
	//内存管理表大小 用来计算内存的使用情况
	unsigned long MemoryStoreSize;
	//数据管理
	PMemList pMemHead,pMemEnd;
	void Init()
	{
		Count = MemVolumeDose = StartAddress = EndAddress = CurrentUsageAmount = SurplusVolumeDose = StartDataAddress = StartDataOffsetAddress = 0;
		Prev = Next = 0;
		//内存管理表大小多分配一个字节,防止内存管理表信息与后面的数据空间连续
		MemoryStoreSize = (sizeof(MemoryStore) + 1);
		//每个内存池都有自己的数据管理表，这样方便内存回收
		pMemHead = pMemEnd = 0;
	}
	//获取节点头
	PMemList GetHeadList()
	{
		return pMemHead;
	}

	//获取最后一个节点
	PMemList GetEndList()
	{
		return pMemEnd;
	}

}* PMemoryStore;

//记录内存池使用率
struct PoolCalculate
{
	//内存池总容量
	unsigned long long PoolTotal;
	//当前内存池使用量
	unsigned long long PoolUsage;
	//内存池数量
	int PoolAmount;

	PoolCalculate():PoolTotal(0),PoolUsage(0),PoolAmount(0)
	{
	}
};

struct Lm_MemoryPool
{
	Lm_MemoryPool(unsigned long AllocSize = MEMPOOLSIZE);
	virtual ~Lm_MemoryPool();

	//内存分配
	void *Lm_MemAlloc(unsigned long nSize);
	//内存释放
	bool Lm_MemFree(void * ptr);
	//释放内存池 类对象销毁前进行自动释放
	bool Lm_MemPoolFree();
	char *Lm_GetLastError();
	//匹配的内存比需要分配的内存多的字节数(最小值 比如默认匹配到的内存比需要分配的内存多一个字节)
	bool Lm_SetComPareMemMini(int nMini);
	//设置单个内存池大小
	bool Lm_SetPoolSize(unsigned long nSize);
private:
	//内存池管理
	PMemoryStore m_Memory,m_MemoryEnd;
	char m_LastError[256];
	//匹配最小值差距
	int m_nCompareMini;
	//初始化内存池大小
	unsigned long m_nInitPoolSize;
	//标记被解除占用的一节
	int m_nCount;
	//内存池大小
	unsigned long m_nPoolSize;
	//内存池使用率，使用率低于百分之四十的情况下释放未使用的内存池
	PoolCalculate m_PoolCal;
	//析构函数调用时，终止正在运行的结构
	bool m_bRun;
private:
	//执行中最后出现的错误进行保存
	bool WriteLastError(const char *data);
	//初始化内存池
	bool InitMemPool(unsigned long AllocSize);
	//创建下一个内存池
	bool CreateNextMemPool(unsigned long AllocSize);
	//分配一节内存管理链表信息
	//从内存池中进行分配链表需要的空间和数据空间，方便后续管理(比如释放多余的空间等)
	PMemList CreateMemList(PMemoryStore pool,PMemList list,int nSize);
	//从内存池中分配一块内存
	void *GetPoolMem(unsigned long nSize);
	//获取首个内存池信息
	PMemoryStore GetPoolHead();
	//获取最后一个内存池信息
	PMemoryStore GetPoolEnd();
	//从一块内存池中获取数据
	void *GetAPoolofMem(PMemoryStore obj,unsigned long nSize);
	//获取最后一个节点
	PMemList GetEndList();
	//创建新的节点以及分配内存 bValid为true标记为已使用内存 ，false标记为未使用
	void *CreateNextMem(PMemoryStore obj,unsigned long nSize,bool bValid);
	//释放指定编号内存池
	bool MemPoolFree_(int nCount);
	//修改指定内存块当前使用量
	PMemList RevampMemUsgeAmount(PMemoryStore pool,PMemList list,unsigned long nSize);
	//回收废弃的内存
	void recycleMemUsgeAmount(PMemoryStore pool,unsigned long nSize);
	//判断链表中上一个节点与下一个节点是否为未使用的内存(把连续未使用的内存放在一起)
	void ContinuousMemory(PMemList list,PMemoryStore pool);
	//合并当前链表中与上一个链表中未使用的内存
	PMemList mergePrivMemory(PMemList list,PMemoryStore pool);
	//合并当前链表中与下一个链表中未使用的内存
	PMemList mergeNextMemory(PMemList list,PMemoryStore pool);
	//如果当前数据链表节点后面没有分配的节点，那么把这部分内存归还给内存池
	void GiveBackMemory(PMemList list,PMemoryStore pool);
	//数据链表分配内存，如果当前数据链表节点内存多出需要分配的一部分，那么内存分配后，再创建一个新的节点管理多余的当前节点内存
	PMemList MemAllocOfList(PMemList list,unsigned long nSize,unsigned long & nLen);
	//内存池使用率低于百分之四十的情况下释放未使用的内存池
	void FreeNeedlessPool();
};
