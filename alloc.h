#pragma once

#include<iostream>
using namespace std;

//一级空间配置器（大内存的分配）
typedef void(*OOMHandle)();

template<int inst>
class MallocAllocTemplate
{
public:
	// 空间申请函数
	static void* Allocate(size_t n)
	{
		void* res = malloc(n);
		if (ret == 0)
			return OOM_Malloc(n);//系统空间不足处理函数
		return res;
	}
	//空间释放函数
	static void* DeAllocate(void* p, size_t size)
	{
		free(p);
	}

	//重新申请空间函数
	static void* Rellocate(void* p, size_t oldsize, size_t newsize)
	{
		void* res = realloc(p, newsize);
		if (res == 0)
			return OOM_Realloc(newsize);
		return res;
	}

	//Rellocate申请空间失败处理函数
	static void* OOM_Realloc(void*, size_t n)
	{
		for (;;)//用while(1)来循环的效率没有用for(;;)高，因为前一种方法需要判断
		{
			if (0 == _pOOMHandle)
				exit(1);
			_pOOMHandle();
			void* res = realloc(p, n);
			if (res)
				return res;
		}
	}

	//Allocate申请空间失败函数
	static void* OOM_Malloc(size_t n)
	{
		for (;;)
		{
			if (0 == _pOOMHanle)
				exit(1);
			_pOOMHandle();
			void* res = malloc(n);
			if (res)
				return res;
		}
	}
	//设置函数指针_pOOMHandle,只有设置后才能进行空间不足时的处理
	static OOMHandle setMallocHandle(OOMHandle f)
	{
		OOMHandle old = _pOOMHanle;
		_pOOMHanle = f;
		return old;
	}
public:
	OOMHandle _pOOMHanle;//一级空间配置器的成员
};
template<int inst>
OOMHandle MallocAllocTemplate<inst>::_pOOMHanle = 0;
typedef MallocAllocTemplate<0> MallocAlloc;

//二级空间配置器
template<int inst>
class DefaultAllocTemplate
{
public:
	
	//该字节应该申请空间的大小对应链表的位置
	size_t FREELIST_INDEX(size_t n)
	{
		return (n+8-1) / _ALIGN-1;
	}

	//向上对齐到8的整数倍（因为空间都是8的整数倍）
	size_t ROUND_UP(size_t size)
	{
		//return (size + _ALIGN-1)&(~_ALIGN);//?????
		return ((size - 1) / _ALIGN + 1)*_ALIGN;
	}
	//申请空间
	static void* Allocate(size_t size)
	{
		if (size > _MAX_BYTES)//申请大块空间（>128）调用一级空间配置器
			return MallocAlloc::Allocate(size);

		//二级空间配置器申请空间，首先从链表中找到对应大小小块空间
		size_t index = FREELIST_INDEX(size);
		
		Obj* res = freeList[index];
		if (NULL == res)
		{
			//链表中没有小块内存,对应的处理函数
			return ReFill(ROUND_UP(size));
		}
		_freeList[index] = res->_FreeListLink;
		return res;
	}
	//当 index位置下链表中没有对应大小的小块的空间时,向内存池索要空间，
	//因为下次有可能还需要这么大的空间，所以我们一次索要20个size字节的空间。
	//第一个size返回给用户，剩下19个挂在相应的链表下。
	static void* ReFill(size_t size)
	{
		size_t nObjs = 20;
		char* chunk = (char*)ChunkAlloc(size, nObjs);
		//内存池的空间只够为我们分配了一个size字节的空间，直接返回给用户
		if (1 == nObjs)
		{
			return chunk;
		}
		//nObjs>1，将第一个返回给用户，剩余挂在对应位置的链表下
		size_t index = FREELIST_INDEX(size);
		void* res = chunk;
		for (size_t i = 1; i<nObjs; ++i)
		{
			chunk += size;
			(Obj*)chunk->_FreeListLink = _freeList[index];
			_freeList[index] = (Obj*)chunk;
		}
		return res;
	}

	//一次从内存池中申请20个小块空间
	static void* ChunkAlloc(size_t size, size_t& n)
	{
		size_t totalByte = n*size;
		size_t leftByte = endFree - _startFree;
		if (leftByte >= totalByte)//能够申请20个size大小的空间
		{
			void* res = _startFree;
			_startFree += totalByte;
			n = 20;
			return res;
		}
		else if (lefyByte >= size)//不能申请20个size大小的空间，但至少能申请一个
		{
			n = leftByte / size;
			void* res = _startFree;
			_startFree = _startFree + n*size;
			return res;
		}
		else//内存池的空间不足size字节，向系统空间索要内存
		{
			if (leftByte > 0)//此时内存池中剩余空间
			{
				size_t index = FREELIST_INDEX(leftByte);
				(Obj*)_startFree->_FreeListLink = _freeList[index];
				_freeList[index] = (Obj*)_startFree;
			}
			//向内存池索要空间
			size_t get2Bytes = 2 * totalByte + ROUND_UP(_HeapSize >> 4);
			_startFree = (char*)mallco(get2Bytes);
			if (NULL == _startFree)//系统空间不足时，在自由链表中查找空间
			{
				size_t index = FREELIST_INDEX(size);
				for (; index < _MAX_BYTES; ++index)
				{
					if (_freeList[index])
					{
						_startFree = (char*)_freeList[index];
						_freeList[index] = _freeList[index]->_FreeListLink;
						_endFree = _startFree + (index + 1)*_ALIGN;
						return ChunkAlloc(size, n);
					}
				}
				//自由链表中也没有足够大的空间，到一集空间配置器中索取
				_endFree = 0;//防止发生异常，当向一级空间配置器索要内存失败，此时_satrtFree为0，而 _endFree不为0
				_startFree = (char*)MallocAlloc::Allocate(get2Bytes);
			}
			_HeapSize = get2Bytes;
			_endFree = _startFree + get2Bytes;
		}
		return ChunkAlloc(size, n);
	}

	static void DeAllocte(void* p, size_t size)
	{
		//大块内存，调用一级空间配置器的销毁函数
		if (size > _MAX_BYTES)
		{
			MallocAlloc::DeAllocate(p.size);
			return;
		}
		//二级空间配置器销毁函数的实现，将空间挂到对应位置的链表下
		size_t index = FREELIST_INDEX[size];
		(Obj*)p->_FreeListLink = _freeList[index];
		_ferrList[index]->_FreeListLink = (obj*)p;
	}

public:
	enum {_ALIGN = 8};
	enum {_MAX_BYTES = 128};
	enum {_NFREELISTS = _MAX_BYTES/_ALIGN};
	union Obj
	{
		Obj* _FreeListLink;//自由链表的下一个节点
		char _clientData[1];
	};
	static Obj* _freeList[_NFREELISTS];//小空间链表
	static char* _startFree;//内存池的起始位置
	static char* _endFree;//内存池的结束位置
	static size_t _HeapSize;//总共从系统（堆）申请的空间
};

template<int inst>
typename DefaultAllocTemplate<inst>::Obj* DefaultAllocTemplate<inst>::_freeList[_NFREELISTS] = { 0 };
template<int inst>
char* DefaultAllocTemplate<inst>::_startFree = 0;
template<int inst>
char* DefaultAllocTemplate<inst>::_endFree = 0;
template<int inst>
size_t DefaultAllocTemplate<inst>::_HeapSize = 0;

#ifdef USE_MALLOC
typedef MallocAlloc Alloc;
#else
typedef DefaultAllocTemplate<0> Alloc;
#endif
//对上述实现的空间配置器进行封装，因为上述的空间配置器的函数参数是字节数
//而我们通常在开辟空间时传入的是参数的类型

//只进行空间的分配，构造函数和析构函数则分开写，目的是为了提高效率
template<class T,class Alloc>
class SimpleAlloc
{
public:
	static T* Allocate(size_t n)//n为开辟空间的个数
	{
		return (0 == n) ? 0 : (T*)Alloc::_Allocate(n*sizeof(T));
	}
	static T* Allocate(void)
	{
		return (T*)Alloc::Allocate(sizeof(T));
	}
	static void DeAllocate(T* p, size_t n)
	{
		if (p)
		{
			Alloc::DeAllocate(p, n*sizeof(T));
		}
	}
	static void DeAllocate(T* p)
	{
		if (p)
		{
			Alloc::DeAllocate(p, sizeof(T));
		}
	}
};