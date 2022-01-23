#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "List.h"

using namespace std;

template<typename _Type>
class Stack
{
public:
	List<_Type> Data;

	~Stack()
	{
	}

	void Push(_Type Temp)
	{
		Data.Add(Temp);
	}
	_Type Pop()
	{
		_Type Temp=Data[Data.Size()-1];
		Data.Delete(Data.Size()-1, 1);
		return Temp;
	}
};

#endif