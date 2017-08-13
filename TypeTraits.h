#pragma once
//对于内置类型，不需要执行析构函数,(例如：int 、float...)
//对于自定义类型，一定要执行析构函数(例如：string)

struct TrueType
{};
struct FalseType
{};

template<class T>
struct TypeTriats
{
	typedef FalseType    hasTrivialDefaultConstructor;
	typedef FalseType    hasTrivialCopyConstructor;
	typedef FalseType    hasTrivialAssignmentOperator;
	typedef FalseType    hasTrivialDestructor;
	typedef FalseType    isPODType;
};

//特化
template<>
struct TypeTriats<int>
{
	typedef TrueType    hasTrivialDefaultConstructor;
	typedef TrueType    hasTrivialCopyConstructor;
	typedef TrueType    hasTrivialAssignmentOperator;
	typedef TrueType    hasTrivialDestructor;
	typedef TrueType    isPODType;
};
template<>
struct TypeTriats<char>
{
	typedef TrueType    hasTrivialDefaultConstructor;
	typedef TrueType    hasTrivialCopyConstructor;
	typedef TrueType    hasTrivialAssignmentOperator;
	typedef TrueType    hasTrivialDestructor;
	typedef TrueType    isPODType;
};