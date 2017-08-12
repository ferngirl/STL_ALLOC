#pragma once

#include<iostream>
using namespace std;

//һ���ռ������������ڴ�ķ��䣩
typedef void(*OOMHandle)();

template<int inst>
class MallocAllocTemplate
{
public:
	// �ռ����뺯��
	static void* Allocate(size_t n)
	{
		void* res = malloc(n);
		if (ret == 0)
			return OOM_Malloc(n);//ϵͳ�ռ䲻�㴦����
		return res;
	}
	//�ռ��ͷź���
	static void* DeAllocate(void* p, size_t size)
	{
		free(p);
	}

	//��������ռ亯��
	static void* Rellocate(void* p, size_t oldsize, size_t newsize)
	{
		void* res = realloc(p, newsize);
		if (res == 0)
			return OOM_Realloc(newsize);
		return res;
	}

	//Rellocate����ռ�ʧ�ܴ�����
	static void* OOM_Realloc(void*, size_t n)
	{
		for (;;)//��while(1)��ѭ����Ч��û����for(;;)�ߣ���Ϊǰһ�ַ�����Ҫ�ж�
		{
			if (0 == _pOOMHandle)
				exit(1);
			_pOOMHandle();
			void* res = realloc(p, n);
			if (res)
				return res;
		}
	}

	//Allocate����ռ�ʧ�ܺ���
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
	//���ú���ָ��_pOOMHandle,ֻ�����ú���ܽ��пռ䲻��ʱ�Ĵ���
	static OOMHandle setMallocHandle(OOMHandle f)
	{
		OOMHandle old = _pOOMHanle;
		_pOOMHanle = f;
		return old;
	}
public:
	OOMHandle _pOOMHanle;//һ���ռ��������ĳ�Ա
};
template<int inst>
OOMHandle MallocAllocTemplate<inst>::_pOOMHanle = 0;
typedef MallocAllocTemplate<0> MallocAlloc;

//�����ռ�������
template<int inst>
class DefaultAllocTemplate
{
public:
	
	//���ֽ�Ӧ������ռ�Ĵ�С��Ӧ�����λ��
	size_t FREELIST_INDEX(size_t n)
	{
		return (n+8-1) / _ALIGN-1;
	}

	//���϶��뵽8������������Ϊ�ռ䶼��8����������
	size_t ROUND_UP(size_t size)
	{
		//return (size + _ALIGN-1)&(~_ALIGN);//?????
		return ((size - 1) / _ALIGN + 1)*_ALIGN;
	}
	//����ռ�
	static void* Allocate(size_t size)
	{
		if (size > _MAX_BYTES)//������ռ䣨>128������һ���ռ�������
			return MallocAlloc::Allocate(size);

		//�����ռ�����������ռ䣬���ȴ��������ҵ���Ӧ��СС��ռ�
		size_t index = FREELIST_INDEX(size);
		
		Obj* res = freeList[index];
		if (NULL == res)
		{
			//������û��С���ڴ�,��Ӧ�Ĵ�����
			return ReFill(ROUND_UP(size));
		}
		_freeList[index] = res->_FreeListLink;
		return res;
	}
	//�� indexλ����������û�ж�Ӧ��С��С��Ŀռ�ʱ,���ڴ����Ҫ�ռ䣬
	//��Ϊ�´��п��ܻ���Ҫ��ô��Ŀռ䣬��������һ����Ҫ20��size�ֽڵĿռ䡣
	//��һ��size���ظ��û���ʣ��19��������Ӧ�������¡�
	static void* ReFill(size_t size)
	{
		size_t nObjs = 20;
		char* chunk = (char*)ChunkAlloc(size, nObjs);
		//�ڴ�صĿռ�ֻ��Ϊ���Ƿ�����һ��size�ֽڵĿռ䣬ֱ�ӷ��ظ��û�
		if (1 == nObjs)
		{
			return chunk;
		}
		//nObjs>1������һ�����ظ��û���ʣ����ڶ�Ӧλ�õ�������
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

	//һ�δ��ڴ��������20��С��ռ�
	static void* ChunkAlloc(size_t size, size_t& n)
	{
		size_t totalByte = n*size;
		size_t leftByte = endFree - _startFree;
		if (leftByte >= totalByte)//�ܹ�����20��size��С�Ŀռ�
		{
			void* res = _startFree;
			_startFree += totalByte;
			n = 20;
			return res;
		}
		else if (lefyByte >= size)//��������20��size��С�Ŀռ䣬������������һ��
		{
			n = leftByte / size;
			void* res = _startFree;
			_startFree = _startFree + n*size;
			return res;
		}
		else//�ڴ�صĿռ䲻��size�ֽڣ���ϵͳ�ռ���Ҫ�ڴ�
		{
			if (leftByte > 0)//��ʱ�ڴ����ʣ��ռ�
			{
				size_t index = FREELIST_INDEX(leftByte);
				(Obj*)_startFree->_FreeListLink = _freeList[index];
				_freeList[index] = (Obj*)_startFree;
			}
			//���ڴ����Ҫ�ռ�
			size_t get2Bytes = 2 * totalByte + ROUND_UP(_HeapSize >> 4);
			_startFree = (char*)mallco(get2Bytes);
			if (NULL == _startFree)//ϵͳ�ռ䲻��ʱ�������������в��ҿռ�
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
				//����������Ҳû���㹻��Ŀռ䣬��һ���ռ�����������ȡ
				_endFree = 0;//��ֹ�����쳣������һ���ռ���������Ҫ�ڴ�ʧ�ܣ���ʱ_satrtFreeΪ0���� _endFree��Ϊ0
				_startFree = (char*)MallocAlloc::Allocate(get2Bytes);
			}
			_HeapSize = get2Bytes;
			_endFree = _startFree + get2Bytes;
		}
		return ChunkAlloc(size, n);
	}

	static void DeAllocte(void* p, size_t size)
	{
		//����ڴ棬����һ���ռ������������ٺ���
		if (size > _MAX_BYTES)
		{
			MallocAlloc::DeAllocate(p.size);
			return;
		}
		//�����ռ����������ٺ�����ʵ�֣����ռ�ҵ���Ӧλ�õ�������
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
		Obj* _FreeListLink;//�����������һ���ڵ�
		char _clientData[1];
	};
	static Obj* _freeList[_NFREELISTS];//С�ռ�����
	static char* _startFree;//�ڴ�ص���ʼλ��
	static char* _endFree;//�ڴ�صĽ���λ��
	static size_t _HeapSize;//�ܹ���ϵͳ���ѣ�����Ŀռ�
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
//������ʵ�ֵĿռ����������з�װ����Ϊ�����Ŀռ��������ĺ����������ֽ���
//������ͨ���ڿ��ٿռ�ʱ������ǲ���������

//ֻ���пռ�ķ��䣬���캯��������������ֿ�д��Ŀ����Ϊ�����Ч��
template<class T,class Alloc>
class SimpleAlloc
{
public:
	static T* Allocate(size_t n)//nΪ���ٿռ�ĸ���
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