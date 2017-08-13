#pragma once
#include"alloc.h"
#include"IteratorTraits.h"
#include"STLconstruct.h"
#include"TypeTraits.h"
#include<assert.h>

// 没有任何意义
template<class T, class Ref, class Pointer>
class VectorIterator
{
public:
	typedef VectorIterator<T, Ref, Pointer> Self;
	typedef T ValueType;

	VectorIterator()
		: _pData(NULL)
	{}

	VectorIterator(T* pData)
		: _pData(pData)
	{}

	VectorIterator(const Self& s)
		: _pData(s._pData)
	{}

	Ref operator*()
	{
		return *_pData;
	}

	Pointer operator->()
	{
		return _pData;
	}

	Self& operator++()
	{
		++_pData;
		return *this;
	}

	Self operator++(int)
	{
		Self temp(*This);
		++_pData;
		return temp;
	}

	Self& operator--()
	{
		--_pData;
		return *this;
	}

	Self operator--(int)
	{
		Self temp(*this);
		--_pData;
		return temp;
	}

	bool operator=(const Self& s)
	{
		return _pData == s._pData;
	}

	bool operator!=(const Self& s)
	{
		return _pData != s._pData;
	}
	
private:
	T* _pData;
};
//Vector实现
template<class T,class Allocate = Alloc>
class Vector
{
public:
	typedef SimpleAlloc<T, Allocate> SimpleAlloc;
	typedef T* Iterator;
//	typedef VectorIterator<T, T*, T&> Iterator;
//	typedef T ValueType;
public:
	//构造函数
	Vector()
		:_start(NULL)
		, _finish(NULL)
		, _endOfStarge(NULL)
	{}

	Vector(size_t n, const T& data)
	{
		_start = SimpleAlloc::Allocate(n);
		for (size_t index = 0; index < n; ++index)
			Construct(_start+idx, data);
		_finish = _start + n;
		_endOfStarge = _finish;
	}
	//析构函数
	~Vector()
	{
		clear();
		SimpleAlloc::DeAllocate((T*)_start,Capacity());
	}

	/////////////////////Modifiers//////////////////////
	void PushBack(const T& data)
	{
		_CheckCapacity();
		Construct(_finish, data);
		++_finish;
	}
	void PopBack()
	{
		assert(_start == _finish);
		Destory(_finish - 1);
		--_finsih;
	}
	Iterator Insert(Iterator pos, const T& data)
	{
		assert(pos >= Begin() && pos <= end());//
		_CheckCapacity();
		for (Iterator it = _finish; it > pos; --it)
			*it = *(it - 1);
		*pos = data;
		++_finish;
		return pos;
	}
	Iterator Erase(Iterator pos)
	{
		assert(pos >= Begin() && pos <= end());//
		for (Iterator it = pos; it < _finish - 1; ++it)
			*it = *(it + 1);
		Destory(_finish - 1);
		--_finish;
		return pos;
	}

	void clear()
	{
		Destory(_start,_finish);
		_finish = _start;
	}

	///////////////////////////Capacity////////////////
	size_t Size()const
	{
		return _start - _finish;
	}
	size_t Capacity()const
	{
		return _endOfStarge -_start;
	}
	bool Empty()const
	{
		return _start == NULL;
	}
	void Resize(size_t newSize, const T& data)
	{
		if (newsize < Size())
		{
			Destory(_start + newSize, _finish);
			_finish = _start + newSize;
		}
		else if (_start + newsize < _endOfStarge)
		{
			for (size_t idx = Size(); idx < newSize; ++idx)
				Construct(_start + idx, data);
		}
		else
		{
			Iterator temp = SimpleAlloc::Allocate(newSize);
			size_t oldSize = Size();
			//搬移元素
			for (size_t idx = 0; idx < oldSize(); ++idx)
				Construct(temp + idx, _start [idx]);
			//销毁旧空间
			Destory(_start, _finish);
			SimpleAlloc::DeAllocate(_start, Capacity());
			_start = temp;
			_finish = _start + newSize;
			_endOfStarge = _finish;
		}
	}

	void Reserve(size_t n)
	{
		size_t oldSize = Size();
		//如果n小于容量，什么也不做
		if (n < Capacity())
		{
			return;
		}
		//如果n大于容量，则重新开辟空间,但不初始化
		Itertaor temp = SimpleAlloc::Allocate(n);		
		for (size_t it = 0; idx < oldSize; ++idx)
			Construct(temp + idx, _start [idx]);
		Destory(_start, Capacity());
		_start = temp;
		_finsih = _start + oldSize;
		_endOfStarge = _start + n;
	}

	///////////////////////Iterator//////////////////////
	Iterator Begin()
	{
		return _start;
	}
	Iterator End()
	{
		return _finish;
	}
private:
	void _CheckCapacity()
	{
		if (_finish == _endOfStarge)
		{
			size_t oldSize = Size();
			size_t newSize = 2 * oldSize+3;
			Iterator temp = SimpleAlloc::Allocate(newSize);
			for (size_t idx = 0; idx < oldSize; ++idx)
				Construct(temp + idx, _start[idx]);
			if (_start)
			{
				Destory(_start, _finish);
				SimpleAlloc::DeAllocate(_start, Capacity());
			}
			_start = temp;
			_finish = _start + oldSize;
			_endOfStarge = _start + newSize;
			
		}
	}
private:
	Iterator _start;
	Iterator _finish;
	Iterator _endOfStarge;
};