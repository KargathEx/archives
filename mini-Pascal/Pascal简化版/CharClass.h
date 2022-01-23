#ifndef CHARCLASS_H
#define CHARCLASS_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "LytWString.h"
#include "Link.h"
#include "Tree.h"
#include "SomeDataType.h"
#include "GrammarTree.h"

using namespace std;

class CharClass
{
public:
	wchar_t Start;
	wchar_t End;
	int Type;
	int Number; // 编号

	CharClass();
	~CharClass();
	bool operator==(CharClass& Temp); //忽略编号Number
	bool IsCharGroup(); //是否为CharGroup
};

class CharClassLink
{
public:
	Link<CharClass*> CharLink; // 字符集链表
	int Size;

	CharClassLink();
	void SetCharNumber(GrammarTree& Expression); //设置字符集编号
	void Print();
	void Clear(); // 清空字符集链表CharLink
	void GetCharNumber(CharClass& Temp_CharClass, Set<int>& CharNumber); // 找字符集对应编号，存在CharNumber
	int GetCharNumber(const GrammarTreeNodeData& Input); // 返回字符集对应编号
	~CharClassLink();

private:
	void CutIn(CharClass Temp); // 插入字符集
	void GetCharClass(TreeNode<GrammarTreeNodeData>* Temp_GrammarTree); //生成字符集链表
};

#endif