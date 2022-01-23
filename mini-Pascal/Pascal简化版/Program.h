#ifndef PROGRAM_H
#define PROGRAM_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "List.h"
#include "LytPtr.h"
#include "Error.h"

using namespace std;

enum DataType // Pascal简化版支持的数据类型
{
	INTEGER,	//整型
	REAL,		//浮点型
	BOOLEAN,	//布尔型
	NONE,
};

enum OperatorPrecedence //操作符优先级，数值越大优先级越高
{
	opXOR=0,
	opOR=0,
	opAND=0,
	opADD=0,
	opRELATION=1,
	opSUB=2,
	opMUL=4,
	opDIV=4,		//整除
	opREAL_DIV=4,	//除法（浮点型）
	opMOD=4,
};

enum Instruction //指令集
{
	PUSH,				//入栈
	POP,				//出栈
	ADD,				//加
	SUB,				//减
	MUL,				//乘
	REAL_DIV,			//除法（浮点型）
	DIV,				//整除
	MOD,				//取余
	NEGATIVE,			//取反
	GREATER_THAN,		//大于
	LESS_THAN,			//小于
	EQUAL_GREATER_THAN,	//大于等于
	EQUAL_LESS_THAN,	//小于等于
	EQUAL,				//等于
	NOT_EQUAL,			//不等
	AND,				//与
	OR,					//或
	XOR,				//异或
	NOT,				//非
	JMP,				//无条件跳转
	JMP_TRUE,			//如果为真则跳转
	JMP_FALSE,			//如果为假则跳转
	LOAD,				//读取变量的内容
	STORE,				//给变量赋值
	CALL,				//调用函数
	RETURN_FUNCTION,	//function函数的return语句
	RETURN_PROCEDURE,	//procedure函数的return语句
	VAR,				//声明变量
	FUN,				//定义函数
	POS,				//标记，最多只能有9999个
};

class InstructionSet
{
public:
	Instruction TheInstruction;	//指令集
	LytWString Operand;			//操作数，一个指令最多对应一个操作数
	DataType OperandType;		//如果操作数有数据类型则记录
	int Position;				//行号

	InstructionSet();
	InstructionSet(const Instruction TempInstruction, LytWString TempOperand, DataType TempOperandType, int Pos);
	InstructionSet(const InstructionSet& Object);
};

class Base
{
public:
	virtual ~Base();
	virtual LytWString ToString(LytWString Prefix)=0;	//将语法树转化为字符串

protected:
	DataType GetDataType(Token Content);	//根据记号串判断数据类型，包括boolean integer real
};

class OperatorBase : public Base	//操作符
{
public:
	DataType Type;	//操作符对应的操作数数据类型
	OperatorPrecedence Precedence;	//操作符优先级
	Token Content;	//操作符的记号串
	OperatorBase();
	OperatorBase(Token TempContent, OperatorPrecedence TempPrecedence);
	OperatorBase(const OperatorBase& Object);
	virtual ~OperatorBase();
	virtual LytWString ToString(LytWString Prefix);
};

class Program;
class Function;

class Expression : public Base	//表达式
{
public:
	virtual ~Expression();
	virtual bool IsLeftValue()=0;	//判断该表达式是否可称为左值
	virtual LytWString ToString(LytWString Prefix)=0;
	virtual DataType GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)=0;	//返回表达式类型
	virtual List<InstructionSet> GetInstructionSet()=0;	//生成中间代码
};

class PrimitiveData : public Expression	//基本表达式
{
public:
	virtual ~PrimitiveData();
	virtual bool IsLeftValue()=0;
	virtual LytWString ToString(LytWString Prefix)=0;
	virtual DataType GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)=0;
	virtual List<InstructionSet> GetInstructionSet()=0;
};

class LiteralConstant : public PrimitiveData	//常量表达式
{
public:
	Token Content;
	DataType Type;
	
	virtual ~LiteralConstant();
	virtual bool IsLeftValue();
	virtual LytWString ToString(LytWString Prefix);
	virtual DataType GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet();
};

class VariableExpression : public PrimitiveData	//变量表达式
{
public:
	Token Id;	//但是这玩意都没被初始化啊.. 哦，查表查出来的吗?

	virtual ~VariableExpression();
	virtual bool IsLeftValue();
	virtual LytWString ToString(LytWString Prefix);
	virtual DataType GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet();
};

class SimpleExpression : public Expression	//简单表达式
{
public:
	LytPtr<PrimitiveData> Content;

	virtual ~SimpleExpression();
	virtual bool IsLeftValue();
	virtual LytWString ToString(LytWString Prefix);
	virtual DataType GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet();
};

class UnaryExpression : public Expression	//一元表达式
{
public:
	Token Operator;				//操作符
	LytPtr<Expression> Operand;	//操作数
	
	virtual ~UnaryExpression();
	virtual bool IsLeftValue();
	virtual LytWString ToString(LytWString Prefix);
	virtual DataType GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet();
};

class BinaryExpression : public Expression	//二元表达式
{
public:
	LytPtr<OperatorBase> Operator;	//操作数
	LytPtr<Expression> Left;		//左操作数
	LytPtr<Expression> Right;		//右操作数
	
	virtual ~BinaryExpression();
	virtual bool IsLeftValue();
	virtual LytWString ToString(LytWString Prefix);
	virtual DataType GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet();
};

class CallFunctionExpression : public Expression	//函数调用表达式
{
public:
	Token FunctionName;						//函数名
	Link<LytPtr<Expression>> ArgumentList;	//参数表

	virtual ~CallFunctionExpression();
	virtual bool IsLeftValue();
	virtual LytWString ToString(LytWString Prefix);
	virtual DataType GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet();
};

class Variable : public Base	//变量
{
public:
	Token Id;				//标识符
	Token Type;				//类型记号串
	DataType TheDataType;	//变量数据类型
	
	Variable();
	Variable(const Token& ObjectId, const Token& ObjectType);
	Variable(const Variable& Object);
	virtual ~Variable();
	virtual bool IsLeftValue();
	virtual LytWString ToString(LytWString Prefix);
	virtual DataType GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet();
};

class Var : public Base		//变量声明
{
public:
	Link<LytPtr<Variable>> Content;
	
	virtual ~Var();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet();
};

class Parameter : public Base	//函数定义中的参数
{
public:
	Token Id;				//标识符
	Token Type;				//类型
	DataType TheDataType;	//参数的数据类型

	Parameter();
	Parameter(const Token& ObjectId, const Token& ObjectType);
	Parameter(const Parameter& Object);
	virtual ~Parameter();
	virtual LytWString ToString(LytWString Prefix);
	virtual DataType GetType(LytPtr<Program> TheProgram);
};

class FunctionHead : public Base	//函数头
{
public:
	bool IsFunction;						//function函数值为true，procedure函数值为false
	Token FunctionType;						//函数类型记号串
	Token FunctionName;						//函数名记号串
	Link<LytPtr<Parameter>> ParameterList;	//参数表
	Token ReturnType;						//返回类型记号串

	virtual ~FunctionHead();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram);
	virtual List<InstructionSet> GetInstructionSet();
};

class Statement : public Base	//语句
{
public:
	virtual ~Statement();
	virtual LytWString ToString(LytWString Prefix)=0;
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)=0;	//类型检查
	virtual List<InstructionSet> GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark)=0;
};

class SimpleStatement : public Statement	//表达式语句
{
public:
	LytPtr<Expression> Content;
	
	virtual ~SimpleStatement();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark);
};

class AssignmentStatement : public Statement	//赋值语句
{
public:
	Token Operator;
	LytPtr<Expression> Left;	//左操作数
	LytPtr<Expression> Right;	//右操作数

	virtual ~AssignmentStatement();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark);
};

class IfStatement : public Statement	//if语句
{
public:
	Token IfToken;
	Token ThenToken;
	Token ElseToken;
	LytPtr<Expression> Condition;	//条件表达式
	LytPtr<Statement> Then;
	LytPtr<Statement> Else;

	virtual ~IfStatement();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark);
};

class WhileStatement : public Statement	//While语句
{
public:
	Token WhileToken;
	Token DoToken;
	bool IsWhileDo;					//while-do语句为真，do-while语句为假
	LytPtr<Expression> Condition;	//条件表达式
	LytPtr<Statement> Do;

	virtual ~WhileStatement();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark);
};

class ControlStatement : public Statement	//跳转语句，包括break continue
{
public:
	Token Content;
	virtual ~ControlStatement();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark);
};

class ReturnStatement : public Statement	//return语句
{
public:
	Token ReturnToken;
	LytPtr<Expression> Return;

	virtual ~ReturnStatement();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark);
};

class Block : public Statement	//语句块
{
public:
	Link<LytPtr<Statement>> StatementList;
	virtual ~Block();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark);
};

class FunctionDeclaration	//函数声明
{
public:
	LytPtr<FunctionHead> Content;

	FunctionDeclaration(LytPtr<FunctionHead> Temp);
	virtual ~FunctionDeclaration();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram);
};

class Function : public Base	//函数
{
public:
	LytPtr<FunctionHead> TheFunctionHead;	//函数头
	LytPtr<Var> TheVar;						//变量声明
	LytPtr<Block> Body;						//函数体
	
	virtual ~Function();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction);
	virtual List<InstructionSet> GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark);
};

class Program : public Base	//程序
{
public:
	Link<LytPtr<Function>> Functions;
	Link<LytPtr<FunctionDeclaration>> FunctionDeclarations;

	virtual ~Program();
	virtual LytWString ToString(LytWString Prefix);
	virtual bool CheckType(LytPtr<Program> TheProgram);
	virtual List<InstructionSet> GetInstructionSet();
};

#endif