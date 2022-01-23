#include "Error.h"

Error::Error()
{
}

Error::Error(const Token& ErrorContent, LytWString ErrorMessage)
{
	Content=ErrorContent;
	Message=ErrorMessage;
}

bool Error::IsError()
{
	if (Message==L"") return false;
	else return true;
}

void Error::SetMessage(LytWString Content)
{
	Message=Content;
}

LytWString Error::GetMessage()
{
	return Message;
}