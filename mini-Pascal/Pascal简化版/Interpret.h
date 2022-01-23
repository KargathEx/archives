#ifndef INTERPRET_H
#define INTERPRET_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "Error.h"
#include "Lexer.h"
#include "Parser.h"
#include "VirtualMachine.h"

using namespace std;

class Interpret
{
private:
	Lexer TheLexer; // 词法分析器
	LytPtr<Program> TheProgram; // 语法树
	List<InstructionSet> TheInstructionSet; // 中间代码
	VirtualMachine TheVirtualMachine; // 虚拟机
	LytWString Result; //运行结果

public:
	void Print();
	Interpret(LytWString Content);
	LytWString InstructionSetToString(); // 将中间代码转换成字符串
};

#endif