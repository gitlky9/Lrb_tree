/**
*李坤昱
*QQ：326087275@qq.com
*/

#include "LMemoryPool.h"

Lm_MemoryPool::Lm_MemoryPool(unsigned long AllocSize):m_Memory(0),m_MemoryEnd(0),
	m_nInitPoolSize(AllocSize),m_nCompareMini(1),m_nCount(0),
	m_nPoolSize(MEMPOOLSIZE),m_bRun(true)
{
	memset(m_LastError,0,sizeof(m_LastError));
}

Lm_MemoryPool::~Lm_MemoryPool()
{
	m_bRun = false;
	Lm_MemPoolFree();
}

bool Lm_MemoryPool::Lm_SetComPareMemMini(int nMini)
{
	if (1 >= nMini)
	{
		return false;
	}
	m_nCompareMini = nMini;
	return true;
}

//设置单个内存池大小
bool Lm_MemoryPool::Lm_SetPoolSize(unsigned long nSize)
{
	if (nSize < MEMPOOLMINI)
		return false;
	m_nPoolSize = nSize;
	return true;
}

char *Lm_MemoryPool::Lm_GetLastError()
{
	return m_LastError;
}

bool Lm_MemoryPool::WriteLastError(const char *data)
{
	if (0 == data)
		return false;
	memset(m_LastError,0,sizeof(m_LastError));
	memcpy(m_LastError,data,sizeof(data));
	return true;
}
//初始化内存池
/*首先创建内存池，内存池起始地址为内存池管理表链表节点的地址加1个字节(比如第一个内存池的起始地址加1个字节为10字节，
 前10字节为第一内存池的链表信息，从10字节开始一直到内存池末尾地址为数据空间地址，所有内存池都将以这种形式存储)*/
/*这种储存方式有以下优点：1.只需要管理内存池就可以保证数据的完整与安全；2.不需要关心链表节点的分配与释放；
 3.避免链接节点释放造成的内存碎片*/
bool Lm_MemoryPool::InitMemPool(unsigned long AllocSize)
{
	//构建池子
	if (0 < AllocSize)
	{
		//多分配出内存池结构描述表内存和1个数据链表节点，这样可以保证一次分配出超出内存池大小的内存
		char *Mem = (char *)malloc(AllocSize + sizeof(MemoryStore) + 1 + sizeof(MemList) + 1);
		if (0 == Mem)
			return false;

		if (0 == m_Memory)
		{
			m_Memory = (PMemoryStore)Mem;
			m_Memory->Init();
		}
		m_Memory->StartAddress = (unsigned long)Mem;
		m_Memory->EndAddress = (m_Memory->StartAddress + AllocSize + m_Memory->MemoryStoreSize + sizeof(MemList) + 1);
		m_Memory->MemVolumeDose = (AllocSize + m_Memory->MemoryStoreSize + sizeof(MemList) + 1);
	}
	else
	{
		char *Mem = (char *)malloc(m_nPoolSize + sizeof(MemoryStore) + 1 + sizeof(MemList) + 1);
		if (0 == Mem)
			return false;

		if (0 == m_Memory)
		{
			m_Memory = (PMemoryStore)Mem;
			m_Memory->Init();
		}
		m_Memory->StartAddress = (unsigned long)Mem;
		m_Memory->EndAddress = (m_Memory->StartAddress + m_nPoolSize + m_Memory->MemoryStoreSize + sizeof(MemList) + 1);
		m_Memory->MemVolumeDose = (m_nPoolSize + m_Memory->MemoryStoreSize + sizeof(MemList) + 1);
	}

 	if (0 == m_Memory)
 		return false;

	m_Memory->Count = 1;
	m_Memory->CurrentUsageAmount = m_Memory->MemoryStoreSize;
	//数据存储起始地址
	m_Memory->StartDataAddress = (m_Memory->StartAddress + m_Memory->MemoryStoreSize);
	//内存池剩余容量
	m_Memory->SurplusVolumeDose = (m_Memory->MemVolumeDose - m_Memory->MemoryStoreSize);
	
	//分配内存失败
	if (0 ==  m_Memory->StartAddress)
	{
		WriteLastError("this MemoryAlloc is Not Valid");
		return false;
	}
	m_MemoryEnd = m_Memory;
	//记录当前内存池信息
	m_PoolCal.PoolTotal += m_Memory->MemVolumeDose;
	++m_PoolCal.PoolAmount;
	m_PoolCal.PoolUsage += m_Memory->CurrentUsageAmount;
	return true; 
}

//创建下一个内存池
bool Lm_MemoryPool::CreateNextMemPool(unsigned long AllocSize)
{
	PMemoryStore memoryPool = GetPoolHead();
	if (m_bRun && 0 == memoryPool)
	{
		InitMemPool(((AllocSize + m_nCompareMini >= m_nPoolSize) ? (AllocSize + m_nCompareMini) : m_nPoolSize));
		return true;
	}
	while (m_bRun && memoryPool && 0 != memoryPool->Next)
		memoryPool = memoryPool->Next;

	//构建池子
	if (0 < AllocSize)
	{
		//多分配出内存池结构描述表内存和1个数据链表节点，这样可以保证一次分配出超出内存池大小的内存
		char *Mem = (char *)malloc(AllocSize + sizeof(MemoryStore) + 1 + sizeof(MemList) + 1);
		if (0 == Mem)
			return false;
		if (!m_bRun)
		{
			delete Mem;
			return false;
		}
			

		memoryPool->Next = (PMemoryStore)Mem;
		memoryPool->Next->Init();
		memoryPool->Next->MemVolumeDose = (AllocSize + memoryPool->Next->MemoryStoreSize + sizeof(MemList) + 1);

		memoryPool->Next->StartAddress = (unsigned long)Mem;
		memoryPool->Next->EndAddress = (memoryPool->Next->StartAddress + AllocSize + memoryPool->Next->MemoryStoreSize + sizeof(MemList) + 1);
	}
	else
	{
		char *Mem = (char *)malloc(m_nPoolSize + sizeof(MemoryStore) + 1 + sizeof(MemList) + 1);
		if (0 == Mem)
			return false;

		memoryPool->Next = (PMemoryStore)Mem;
		memoryPool->Next->Init();
		memoryPool->Next->MemVolumeDose = (m_nPoolSize + memoryPool->Next->MemoryStoreSize + sizeof(MemList) + 1);

		memoryPool->Next->StartAddress = (unsigned long)Mem;
		memoryPool->Next->EndAddress = (memoryPool->Next->StartAddress + m_nPoolSize + memoryPool->Next->MemoryStoreSize + sizeof(MemList) + 1);
	}
	if (0 == memoryPool->Next)
		return false;

	memoryPool->Next->Count = (memoryPool->Count + 1);
	memoryPool->Next->CurrentUsageAmount = memoryPool->Next->MemoryStoreSize;
	//数据存储起始地址
	memoryPool->Next->StartDataAddress = (memoryPool->Next->StartAddress + memoryPool->Next->MemoryStoreSize);
	//内存池剩余容量
	memoryPool->Next->SurplusVolumeDose = (memoryPool->Next->MemVolumeDose - memoryPool->Next->MemoryStoreSize);
	//分配内存失败
	if (0 ==  memoryPool->Next->StartAddress)
	{
		WriteLastError("this MemoryAlloc is Not Valid");
		return false;
	}
	m_MemoryEnd = memoryPool->Next;
	m_MemoryEnd->Prev = memoryPool;
	//记录当前内存池信息
	m_PoolCal.PoolTotal += m_MemoryEnd->MemVolumeDose;
	++m_PoolCal.PoolAmount;
	m_PoolCal.PoolUsage += m_MemoryEnd->CurrentUsageAmount;
	return true; 
}

void *Lm_MemoryPool::Lm_MemAlloc(unsigned long nSize)
{
	return GetPoolMem(nSize);
}

//获取首个内存池信息
PMemoryStore Lm_MemoryPool::GetPoolHead()
{
	return m_Memory;
}
//获取最后一个内存池信息
PMemoryStore Lm_MemoryPool::GetPoolEnd()
{
	return m_MemoryEnd;
}
//从所有内存池中取出未使用的内存地址
void *Lm_MemoryPool::GetPoolMem(unsigned long nSize)
{
	PMemoryStore pool = GetPoolHead();
	
	while (m_bRun && pool)
	{
		char *pData = (char *)GetAPoolofMem(pool,nSize);
		if (0 != pData)
			return (void *)pData;
		pool = pool->Next;
	}	
	//如果没有创建内存池或者所有的池子都遍历了还是没有合适内存，那么创建一个新的池子
	if ((nSize + m_nCompareMini)  > m_nPoolSize)
		CreateNextMemPool(nSize + m_nCompareMini);
	else
		CreateNextMemPool(m_nPoolSize);

		char *pData = (char *)GetAPoolofMem(m_MemoryEnd,nSize);
		return (void *)pData;
}

//从一块内存池中获取数据
void *Lm_MemoryPool::GetAPoolofMem(PMemoryStore obj,unsigned long nSize)
{
	if (!m_bRun || 0 == obj || 0 >= obj->SurplusVolumeDose || nSize >= obj->SurplusVolumeDose)
	{
		return 0;
	}
		//如果达到查询的条件 开始遍历对应编号的内存池 ，为了最大利用内存选择从头开始遍历，如果没有数据插入到最后
		PMemList listData = obj->GetHeadList();
		while (m_bRun && listData && (0 != listData->Next))
		{

			//如果节点中保存的内存使用量大于或等于需要分配的内存，那么使用这块内存
			if ((nSize + m_nCompareMini  <= listData->CurrentUsgeAmount) && (0 == listData->bValid))
			{
				PMemList PList = RevampMemUsgeAmount(obj,listData,nSize);
				PList->bValid = 1;
				return (void *)PList->StartAddress;
			}
			listData = listData->Next;
		}
	//创建新的节点保存分配内存
	return CreateNextMem(obj,nSize,1);
}

//修改指定内存块当前使用量
PMemList Lm_MemoryPool::RevampMemUsgeAmount(PMemoryStore pool,PMemList list,unsigned long nSize)
{
	PMemList pList = list;
	if (0 != pool)
	{
		unsigned long nLen = 0;
		pList = MemAllocOfList(list,nSize,nLen);
		pool->CurrentUsageAmount += nLen;
		pool->SurplusVolumeDose = (pool->MemVolumeDose - pool->CurrentUsageAmount);
		m_PoolCal.PoolUsage += nLen;
	}

	return pList;
}

//回收废弃的内存
void Lm_MemoryPool::recycleMemUsgeAmount(PMemoryStore pool,unsigned long nSize)
{
	if (0 != pool)
	{
		pool->CurrentUsageAmount -= nSize;
		pool->SurplusVolumeDose = (pool->MemVolumeDose - pool->CurrentUsageAmount);
		m_PoolCal.PoolUsage -= nSize;
	}
}

//数据链表分配内存，如果当前数据链表节点内存多出需要分配的一部分，那么内存分配后，再创建一个新的节点管理多余的当前节点内存
PMemList Lm_MemoryPool::MemAllocOfList(PMemList list,unsigned long nSize,unsigned long & nLen)
{
	//如果当前节点的内存空间大于需要分配内存加上节点描述内存加5个字节，那么创建一个新的节点
	if (m_bRun && list->CurrentUsgeAmount >= (nSize + sizeof(MemList) + 5))
	{
		PMemList Nextlist = list->Next;
		list->Next = (PMemList)(list->StartAddress + nSize + 2);
		list->Next->nodeInfo.StartAddress = (list->StartAddress + nSize + 2);
		list->Next->nodeInfo.CurrentUsgeAmount = (sizeof(MemList) + 1);
		list->Next->StartAddress = (list->Next->nodeInfo.StartAddress + list->Next->nodeInfo.CurrentUsgeAmount);
		list->Next->EndAddress = list->EndAddress;
		list->Next->CurrentUsgeAmount = (list->CurrentUsgeAmount - list->Next->nodeInfo.CurrentUsgeAmount - nSize - 2);

		list->Next->Next = Nextlist;
		list->Next->Next->Prev = list->Next;
		list->Next->Prev = list;

		list->CurrentUsgeAmount = (nSize + 1);
		list->EndAddress = (list->StartAddress + nSize + 1);
		nLen = (list->CurrentUsgeAmount + sizeof(MemList) + 2);
	}
	else
		nLen = list->CurrentUsgeAmount;
	//分配干净的内存
	memset((void *)list->StartAddress,0,list->CurrentUsgeAmount);
	return list;
}

//创建新的节点以及分配内存
void *Lm_MemoryPool::CreateNextMem(PMemoryStore obj,unsigned long nSize,bool bValid)
{
	PMemList list = obj->GetEndList();
	list = CreateMemList(obj,list,nSize);
	if (0 == list)
		return 0;

	list->StartAddress = (obj->StartDataAddress + obj->StartDataOffsetAddress);
	//多分配一个字节用来防止内存越界
	list->EndAddress = (list->StartAddress + nSize + 1);
	list->CurrentUsgeAmount = (nSize + 1);
	list->bValid = bValid;
	obj->CurrentUsageAmount += list->CurrentUsgeAmount;
	obj->SurplusVolumeDose -= list->CurrentUsgeAmount;
	obj->StartDataOffsetAddress += list->CurrentUsgeAmount;
	obj->pMemEnd = list;
	obj->pMemEnd->Next = 0;

	m_PoolCal.PoolUsage += list->CurrentUsgeAmount;
	//分配出一段干净的内存 上层方便使用
	memset((void *)list->StartAddress,0,list->CurrentUsgeAmount);
	return (void *)list->StartAddress;
}

PMemList Lm_MemoryPool::CreateMemList(PMemoryStore pool,PMemList list,int nSize)
{
	if (!m_bRun)
		return 0;

	if (0 == list)
	{
		list = (PMemList)(pool->StartDataAddress + pool->StartDataOffsetAddress);
		if (0 == list)
			return 0;

		list->Init();
		if (0 == list)
			return 0;

		list->nodeInfo.StartAddress = (pool->StartDataAddress + pool->StartDataOffsetAddress);
		list->nodeInfo.CurrentUsgeAmount = (sizeof(MemList) + 1);
		pool->pMemHead = list;

		pool->CurrentUsageAmount += list->nodeInfo.CurrentUsgeAmount;
		pool->SurplusVolumeDose -= list->nodeInfo.CurrentUsgeAmount;
		//当前数据偏移地址
		pool->StartDataOffsetAddress += list->nodeInfo.CurrentUsgeAmount;

		m_PoolCal.PoolUsage += list->nodeInfo.CurrentUsgeAmount;
		return list;
	}

	list->Next = (PMemList)(pool->StartDataAddress + pool->StartDataOffsetAddress);
	if (0 == list)
		return 0;

	list->Next->Init();
	if (0 == list->Next)
		return 0;

	list->Next->nodeInfo.StartAddress = (pool->StartDataAddress + pool->StartDataOffsetAddress);
	list->Next->nodeInfo.CurrentUsgeAmount = (sizeof(MemList) + 1);
	list->Next->Prev = list;

	pool->CurrentUsageAmount += list->Next->nodeInfo.CurrentUsgeAmount;
	pool->SurplusVolumeDose -= list->Next->nodeInfo.CurrentUsgeAmount;
	//当前数据偏移地址
	pool->StartDataOffsetAddress += list->Next->nodeInfo.CurrentUsgeAmount;

	m_PoolCal.PoolUsage += list->Next->nodeInfo.CurrentUsgeAmount;
	return list->Next;
}

//内存释放
bool Lm_MemoryPool::Lm_MemFree(void * ptr)
{
	//根据分配的地址在内存池中匹配，匹配到后 修改结构数据后，等待再次使用
	PMemoryStore pool = GetPoolHead();
	while (m_bRun && pool)
	{
		PMemList list = pool->GetHeadList();
		while (m_bRun && list)
		{
			//如果链表中其中一节数据与需要释放的地址相同 ，而且这段数据属于使用中，属于这块内存
			if ((list->StartAddress == (unsigned long)ptr) && (1 == list->bValid))
			{
				recycleMemUsgeAmount(pool,list->CurrentUsgeAmount);
				//回收的时候不需要初始化内存，因为再次使用的时候会进行初始化
				list->bValid = 0;
				ptr = 0;
				//判断链表中上一个节点与下一个节点是否为未使用的内存(把连续未使用的内存放在一起)
				if (pool)
					ContinuousMemory(list,pool);

				return true;
			}
			list = list->Next;
		}
		pool = pool->Next;
	}
	return false;
}

//判断链表中上一个节点与下一个节点是否为未使用的内存(把连续未使用的内存放在一起)
void Lm_MemoryPool::ContinuousMemory(PMemList list,PMemoryStore pool)
{
	PMemList pList = mergePrivMemory(list,pool);
	pList = mergeNextMemory(pList,pool);
	GiveBackMemory(pList,pool);
	//内存池使用率低于百分之四十的情况下释放未使用的内存池
	FreeNeedlessPool();
}

//合并当前链表中与上一个链表中未使用的内存
PMemList Lm_MemoryPool::mergePrivMemory(PMemList list,PMemoryStore pool)
{
	int nNum = 0;
	while (m_bRun && list->Prev && !list->Prev->bValid)
	{
		list->Prev->CurrentUsgeAmount += (list->nodeInfo.CurrentUsgeAmount + list->CurrentUsgeAmount);
		list->Prev->EndAddress = list->EndAddress;
		//如果是第一次合并内存，只需要减去链表描述结构大小即可
		if (0 == nNum)
		{
			pool->CurrentUsageAmount -= list->nodeInfo.CurrentUsgeAmount;
			pool->SurplusVolumeDose += list->nodeInfo.CurrentUsgeAmount;

			m_PoolCal.PoolUsage -= list->nodeInfo.CurrentUsgeAmount;
		}
		else
		{
			pool->CurrentUsageAmount -= (list->nodeInfo.CurrentUsgeAmount + list->CurrentUsgeAmount);
			pool->SurplusVolumeDose += (list->nodeInfo.CurrentUsgeAmount + list->CurrentUsgeAmount);

			m_PoolCal.PoolUsage -= (list->nodeInfo.CurrentUsgeAmount + list->CurrentUsgeAmount);
		}
		++nNum;

		if (list->Next)
		{
			list->Prev->Next = list->Next;
			list->Next->Prev = list->Prev;
		}
		else
		{
			list->Prev->Next = 0;
		}

		list = list->Prev;
		//如果上一个节点存在 而且是有效节点 或者 上一个节点不存在，直接返回本次执行结果
		if ((list->Prev  && list->Prev->bValid) || !list->Prev)
			return list;
	}
	return list;
}

//合并当前链表中与下一个链表中未使用的内存
PMemList Lm_MemoryPool::mergeNextMemory(PMemList list,PMemoryStore pool)
{
	while (m_bRun && list->Next && !list->Next->bValid)
	{
		//两块连续未使用的内存合并，并且去掉后面未使用内存的链表节点(一起放入合并的内存中)
		list->CurrentUsgeAmount += (list->Next->nodeInfo.CurrentUsgeAmount + list->Next->CurrentUsgeAmount);
		list->EndAddress = list->Next->EndAddress;
		//计算内存池当前使用量和可用量
		pool->CurrentUsageAmount -= (list->Next->nodeInfo.CurrentUsgeAmount + list->Next->CurrentUsgeAmount);
		pool->SurplusVolumeDose += (list->Next->nodeInfo.CurrentUsgeAmount + list->Next->CurrentUsgeAmount);

		m_PoolCal.PoolUsage -= (list->Next->nodeInfo.CurrentUsgeAmount + list->Next->CurrentUsgeAmount);

		if (list->Next->Next)
		{
			list->Next->Next->Prev = list;
			list->Next = list->Next->Next;
		}
		else
			list->Next = 0;
		//如果下一个节点存在 而且是有效节点 或者 下一个节点不存在，直接返回本次执行结果
		if ((list->Next  && list->Next->bValid) || !list->Next)
			return list;
	}
	return list;
}

//如果当前数据链表节点后面没有分配的节点，那么把这部分内存信息描述擦除，由内存池分配使用
void Lm_MemoryPool::GiveBackMemory(PMemList list,PMemoryStore pool)
{
	if (m_bRun && list && !list->Next)
	{
		pool->CurrentUsageAmount -= list->nodeInfo.CurrentUsgeAmount;
		pool->SurplusVolumeDose += list->nodeInfo.CurrentUsgeAmount;
		pool->StartDataOffsetAddress = (list->nodeInfo.StartAddress - pool->StartDataAddress);

		m_PoolCal.PoolUsage -= list->nodeInfo.CurrentUsgeAmount;

		if (list->Prev)
			list->Prev->Next = 0;
		else //数据链表已经没有节点了，完全由内存池分配使用
			pool->pMemHead = pool->pMemEnd = 0;
	}
}

//释放内存池 ，这个不需要手动释放 ，类对象销毁前会进行释放
bool Lm_MemoryPool::Lm_MemPoolFree()
{
	PMemoryStore memPool = GetPoolHead();
	while (0 != memPool)
	{
		PMemoryStore next =  memPool->Next;
		MemPoolFree_(memPool->Count);
		memPool = next;
	}
	return true;
}

//释放指定编号内存池
bool Lm_MemoryPool::MemPoolFree_(int nCount)
{
	PMemoryStore memPool = GetPoolHead();
	while (memPool->Count != nCount)
		memPool = memPool->Next;

	if (0 == memPool)
		return false;

	PMemoryStore Prev = 0,next = 0;
	if (0 != memPool->Prev)
		Prev = memPool->Prev;
	if (0 != memPool->Next)
		next = memPool->Next;

	//修改内存池使用率信息
	m_PoolCal.PoolTotal -= memPool->MemVolumeDose;
	m_PoolCal.PoolUsage -= memPool->CurrentUsageAmount;
	--m_PoolCal.PoolAmount;

	delete memPool;
	memPool = 0;
	if (0 != Prev)
		Prev->Next = next;
	else
		m_Memory = next;
	if (0 != next)
		next->Prev = Prev;
	else
		m_MemoryEnd = m_Memory;
	return true;
}

//内存池使用率低于百分之四十的情况下释放未使用的内存池
void Lm_MemoryPool::FreeNeedlessPool()
{
	ReRun:
	if (40 >= ((double)m_PoolCal.PoolUsage / (double)m_PoolCal.PoolTotal * 100) && 2 <= m_PoolCal.PoolAmount)
	{
		//找到未使用的内存池进行释放
		PMemoryStore memPool = GetPoolHead();
		while (0 != memPool)
		{
			if (0 == memPool->StartDataOffsetAddress)
			{
				MemPoolFree_(memPool->Count);
				//释放内存池后，继续检查内存使用率是否大于百分之四十
				goto ReRun;
			}
			else
				memPool = memPool->Next;
		}
	}
}
