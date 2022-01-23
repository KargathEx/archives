#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "Link.h"
#include "Regex.h"

using namespace std;

enum class RegularExpressionType
{
	regularCOMMENT,		//注释
	regularSTRING,		//字符串
	regularINTEGER,		//整型
	regularREAL,		//浮点数
	regularID,			//标识符
	regularOPERATOR,	//操作符
	regularSPACE,		//空格
	regularENTER,		//回车
	regularERROR		//错误信息
};

class Token
{
public:
	int Position; //行
	LytWString Content;
	RegularExpressionType Type; //词素类型

	Token() :Position(0), Content(), Type() {};		//默认构造函数
	Token(LytWString TheContent, RegularExpressionType TheType, int ThePosition);
	Token(const Token& Object);
};

class RegexToken
{
public:
	Regex* TheRegex;
	RegularExpressionType Type;
	bool Exist; //该词素是否可在以后的分析中被忽略

	RegexToken(LytWString Expression, RegularExpressionType TempType, bool IsExist);
	~RegexToken();
};

class Lexer //词法分析器
{
private:
	Link<RegexToken*> Regexs;
public:
	Link<Token*> Tokens; //可识别的词素
	Link<Token*> ErrorTokens; //无法识别的词素

	Lexer();
	~Lexer();
	bool IsError();
	void RunLexer(LytWString Expression);
	void Print();
};

#endif