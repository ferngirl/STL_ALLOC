#pragma once
//�����������ͣ�����Ҫִ����������,(���磺int ��float...)
//�����Զ������ͣ�һ��Ҫִ����������(���磺string)

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

//�ػ�
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