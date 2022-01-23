#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "LytWString.h"
#include "Lexer.h"

using namespace std;

class Error
{
public:
	LytWString Message;
	Token Content;

	Error();
	Error(const Token& ErrorContent, LytWString ErrorMessage);
	bool IsError();
	void SetMessage(LytWString Content);
	LytWString GetMessage();
};

#endif