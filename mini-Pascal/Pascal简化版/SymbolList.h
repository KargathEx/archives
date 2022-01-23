#ifndef SYMBOLLIST_H
#define SYMBOLLIST_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "List.h"
#include "LytPtr.h"
#include "Program.h"

using namespace std;

class Symbol
{
public:
	LytWString Id;
	DataType Type;

	Symbol();
	Symbol(LytWString TempId, DataType TempType);
	Symbol(const Symbol& Object);
};

class FunctionSymbolList
{
public:
	LytWString FunctionName;	//函数名
	int FunctionIndex;			//函数在中间代码中的下标
	List<LytPtr<Symbol>> ParameterList;
	List<LytPtr<Symbol>> VariableList;

	void Initialize(LytPtr<Function> TheFunction);
	const int GetMoveStep(LytWString TheId)const;	//返回变量或参数在Base基准下的偏移量，有正负
};

class SymbolList	//符号表
{
public:
	List<LytPtr<FunctionSymbolList>> Data;

	void Initialize(LytPtr<Program> TheProgram);					//初始化
	const int GetFunctionIndex(LytWString TheFunctionName)const;	//返回函数在Data中的下标
};

#endif