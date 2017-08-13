#pragma once
#include<string.h>
#include"STLconstruct.h"
#include"alloc.h"

//节点定义
template<class T>
struct Node
{
	Node(const T& data)
	:_data(_data)
	, _pNext(NULL)
	, _pRev(NULL)
	{}
	T _data;
	Node* _pNext;
	Node* _pRev;
};

//List的迭代器
template<class T, class Ref, class Pointer>
class _ListIterator_
{
public:
	typedef Node<T> Node;
	typedef _ListIterator_<T, Ref, Pointer> Self;
	typedef T ValueType;
public:
	//构造函数
	_ListIterator_()
		:_pNode(NULL)
	{}
	_ListIterator_(Node* pNode)
		:_pNode(pNode)
	{}
	//拷贝构造函数
	_ListIterator_(const Self& s)
		:_pNode(s._pNode)
	{}
	Ref operator*()
	{
		return _pNode->_data;
	}
	Pointer operator->()
	{
		return &(operator*());
	}
	Self& operator++()
	{
		_pNode = _pNode->_pNext;
		return *this;
	}
	Self operator++(int)
	{
		Self pTemp(*this);
		_pNode = _pNode->_pNext;
		return pTemp;
	}

	Self& operator--()
	{
		_pNode = _pNode->_pRev;
		return *this;
	}

	Self operator--(int)
	{
		Self temp(*this);
		_pNode = _pNode->_pRev;
		return temp;
	}

	bool operator==(const Self& s)
	{
		return s._pNode == _pNode;
	}
	bool operator!=(const Self& s)
	{
		return !(operator==(s));
	}
private:
	Node* _pNode;
};

//list类的定义---带头节点的双向链表
template<class T, class Allocate = Alloc>
class List
{
public:
	typedef Node<T> Node;
	typedef _ListIterator_<T, T&, T*> Iterator;
	typedef SimpleAlloc<Node, Alloc> SimpleAlloc;
	//构造函数
	List()
	{
		_pHead = CreateNode();//连表头 
		_pHead->_pNext = _pHead;
		_pHead->_pRev = _pHead;
	}
	~List()
	{
		Clear(Begin(), End());
		DestoryNode(_pHead);
	}

	//////////////////////Modifiers//////////////////////////
	void PushBack(const T& data)
	{
		Node* pNewNode = CreateNode(data);
		Node* pTailNode = _pHead->_pRev;
		pTailNode->_pNext = pNewNode;
		pNewNode->_pNext = _pHead;
		pNewNode->_pRev = pTailNode;
		_pHead->_pRev = pNewNode;
	}

	void PushFront(const T& data)
	{
		Node* pNewNode = CreateNode(data);
		pNewNode->_pNext = _pHead->_pNext;
		pNewNode->_pRev = _pHead;
		_pHead->_pNext = pNewNode;
	}

	void PopBack()
	{
		if (_pHead->_pNext == _pHead)
			return;
		Node* pDelNode = _pHead->_pRev;
		_pHead->_pRev = _pHead->_pRev->_pRev;
		_pHead->_pRev->_pRev->_pNext = _pHead;
		DestoryNode(pDelNode)//先析构在销毁空间
	}

	void PopFront()
	{
		if (_pHead->_pRev = _pHead)
			return;
		Node* pDelNode = _pHead->_pNext;
		_pHead->_pNext = pDelNode->_pNext;
		pDelNode->_pNext->_pRev = _pHead;
		DestoryNode(pDelNode)//先析构在销毁空间
	}

	void Clear(Iterator first, Iterator last)
	{
		Destory(first, last);
	}

	//*Iterator Insert(Iterator pos, const T& data);


	//Iterator Erase(Iterator pos);

	/////////////////////Capacity///////////////////////////
	size_t  Size()const
	{
		int count = 0;
		Node* pCur = _pHead;
		while (pCur->_pRev != _pHead)
		{
			count++;
			pCur = pCur->_pNext;
		}
		return count;
	}

	bool Empty()const
	{
		if (_pHead->_pRev != _pHead)
			return false;
		else
			return true;
	}
	size_t max_size()const
	{
		return -1;
	}

	////////////////////////element//////////////////////////
	T& Front()
	{
		return (_pHead->_pNext->_data);
	}

	const T& Front()const
	{
		return (_pHead->_pRev->_data);
	}

	///////////////////////////////Iterator//////////////////////////
	Iterator Begin()
	{
		return Iterator(_pHead->_pNext);
	}
	Iterator End()
	{
		return (Iterator)_pHead;
	}

private:
	Node* CreateNode(const T& data = T())
	{
		Node* p = SimpleAlloc::Allocate();//先申请空间---alloc.h
		Construct(p, data);//在调用构造函数为空间初始化---STLconstruct.h
		return p;
	}
	void DestoryNode(Node* p)
	{
		Destory(&(p->_data));
		size_t n = Size();
		if (n == 0)
			SimpleAlloc::DeAllocate(p);
		else
			SimpleAlloc::DeAllocate(p, n);
	}

private:
	Node* _pHead;
};
