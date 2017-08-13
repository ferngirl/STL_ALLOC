#pragma once
struct InputIteratorTag
{};
struct OutputIteratortAG
{};
struct ForwardIteratorTag :public InputIteratorTag
{};
struct BidirectionalIteratorTag :public ForwardIteratorTag
{};
struct RandomAccessIteratorTag :public BidirectionalIteratorTag
{};

template<class Iterator>
class IteratorTraits
{
public:
	typename typedef Iterator::ValueType ValueType;//对象的类型
};
template<class T>
struct IteratorTraits<T*>
{
	typedef T ValueType;
};
