#ifndef REGEX_H
#define REGEX_H

#include <iostream>
#include <string.h>
#include <math.h>
#include <iomanip> 
#include "LytWString.h"
#include "GrammarTree.h"
#include "EpsilonNfa.h"
#include "Nfa.h"
#include "Dfa.h"
#include "DfaMatrix.h"

using namespace std;

class Regex
{
public:

	class MatchResult
	{
	public:
		const wchar_t* Start; //匹配的开始位置，如果匹配失败则这里是0
		int Length; //匹配的长度

		MatchResult();
		MatchResult(const MatchResult& Object);
		~MatchResult();
	};

	Regex();
	Regex(LytWString Expression); //初始化正则表达式
	LytWString GetErrorMessage()const; //返回错误信息
	bool IsError()const; //返回是否错误
	MatchResult Match(const LytWString& Object); //匹配字符串

private:
	LytWString ErrorMessage; //错误信息
	DfaMatrix DfaGraphMatrix; //DFA矩阵
	CharClassLink CharLink; //字符集

	void SetErrorMessage(const LytWString ErrorInformation); //设置错误信息
	int RunDfa(const wchar_t* Input);
	void ReleaseGraph(); //内存管理: 释放NFA、DFA
};

#endif