#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "SymbolList.h"
#include "Stack.h"

using namespace std;

class VirtualMachine			//虚拟机
{
public:
	class StackData				//堆栈内容数据类型
	{
	public:
		DataType Type;			//数据类型
		struct Data
		{
			int IntegerData;	//整型数据
			double RealData;	//浮点型数据
			bool BooleanData;	//布尔型数据
		}Value;

		StackData();
		StackData(DataType TheType, LytWString Data);
		StackData(const StackData& Object);
	};

	VirtualMachine();
	LytWString Excuse(LytPtr<Program> TheProgram, List<InstructionSet> TheInstructionSet);	//执行中间代码

private:
	int BaseIndex;				//当前Base值
	Stack<StackData> TheStack;	//堆栈
	SymbolList TheSymbolList;	//符号表
	List<int> Position;			//标志对应行号，下表为标志号

	LytWString GetResult(VirtualMachine::StackData Data);	//将执行结果转换为字符串
	void Initialize(LytPtr<Program> TheProgram, List<InstructionSet> TheInstructionSet);	//初始化虚拟机
	void Push(InstructionSet TheInstructionSet);
	void Push(StackData Object);
	StackData Pop();
	void Add(InstructionSet TheInstructionSet);
	void Sub(InstructionSet TheInstructionSet);
	void Mul(InstructionSet TheInstructionSet);
	void RealDiv(InstructionSet TheInstructionSet);
	void Div(InstructionSet TheInstructionSet);
	void Mod(InstructionSet TheInstructionSet);
	void Negative(InstructionSet TheInstructionSet);
	void GreaterThan(InstructionSet TheInstructionSet);
	void LessThan(InstructionSet TheInstructionSet);
	void EqualGreaterThan(InstructionSet TheInstructionSet);
	void EqualLessThan(InstructionSet TheInstructionSet);
	void Equal(InstructionSet TheInstructionSet);
	void NotEqual(InstructionSet TheInstructionSet);
	void And(InstructionSet TheInstructionSet);
	void Or(InstructionSet TheInstructionSet);
	void Not(InstructionSet TheInstructionSet);
	void Xor(InstructionSet TheInstructionSet);
	void Var(InstructionSet TheInstructionSet);
	void Load(InstructionSet TheInstructionSet);
	void Store(InstructionSet TheInstructionSet);
	int Jmp(InstructionSet TheInstructionSet);
	int JmpTrue(InstructionSet TheInstructionSet);
	int JmpFalse(InstructionSet TheInstructionSet);
	int Call(InstructionSet TheInstructionSet, int Return);
	int Return_Function(InstructionSet TheInstructionSet);
	int Return_Procedure(InstructionSet TheInstructionSet);
};

#endif