#ifndef SOMEDATATYPE_H
#define SOMEDATATYPE_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "Link.h"
#include "Set.h"

using namespace std;

enum CharType // 字符类型
{
	Char, // 字符
	CharGroup, // 字符集
	Transferred, // 转义符
	Operator // 操作符
};

const int Epsilon=0; // 内容为空

#endif