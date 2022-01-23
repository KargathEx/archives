#include "Lexer.h"

//Token

Token::Token(LytWString TheContent, RegularExpressionType TheType, int ThePosition)
{
	Content=TheContent;
	Type=TheType;
	Position=ThePosition;
}

Token::Token(const Token& Object)
{
	Content=Object.Content;
	Type=Object.Type;
	Position=Object.Position;
}

//RegexToken

RegexToken::RegexToken(LytWString Expression, RegularExpressionType TempType, bool IsExist)
{
	TheRegex=new Regex(Expression);
	Type=TempType;
	Exist=IsExist;
}

RegexToken::~RegexToken()
{
	delete TheRegex;
}

//Lexer

Lexer::Lexer()	
{	
	//这里先定义了一堆Token的正则表达式来匹配对应的词法单元类型. 其实如果自己写的话，用标准库组件也能搞出来对吧?
	Regexs.AddLast()->Data=new RegexToken(L"/\\*([^\\*]|\\*+[^\\*/])*\\*+/", RegularExpressionType::regularCOMMENT, true);
	Regexs.AddLast()->Data=new RegexToken(L"\"([^\"]|[\\\\]\")*\"", RegularExpressionType::regularSTRING, true);
	Regexs.AddLast()->Data=new RegexToken(L"[0-9]+.[0-9]+", RegularExpressionType::regularREAL, true);
	Regexs.AddLast()->Data=new RegexToken(L"[0-9]+", RegularExpressionType::regularINTEGER, true);
	Regexs.AddLast()->Data=new RegexToken(L"[_a-zA-Z]+([_0-9a-zA-Z])*", RegularExpressionType::regularID, true);
	Regexs.AddLast()->Data=new RegexToken(L"\\(", RegularExpressionType::regularOPERATOR ,true);
	Regexs.AddLast()->Data=new RegexToken(L"\\)", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L"\\+", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L"\\-", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L"\\*", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L"/", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L">(=)?", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L"<(=)?", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L"(!)?=", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L":(=)?", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L",", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L";", RegularExpressionType::regularOPERATOR, true);
	Regexs.AddLast()->Data = new RegexToken(L"( )+", RegularExpressionType::regularSPACE, false); //不知道"\\s"为啥错
	Regexs.AddLast()->Data = new RegexToken(L"\r\n", RegularExpressionType::regularENTER ,false);
}

void Lexer::RunLexer(LytWString Expression)
{
	int Position=1;
	const wchar_t* Content=Expression.Buffer();		//直接艹char,这姐姐好棒...
	while (Content[0])	//如果为空的话0会是'\0',所以这里用来判空? 是的没错. 但是里面会修改这个值吗?
	{
		bool IsSuccessful=false;
		Node<RegexToken*>* TempRegex=Regexs.GetHead();	//虽然我不知道这个's是干嘛的，获得个头.
		while (TempRegex)
		{
			Regex::MatchResult TheMatchResult=TempRegex->Data->TheRegex->Match(Content);	//获得匹配结果
			if (TempRegex->Data->TheRegex->IsError()==false)
			{
				if (TempRegex->Data->Exist)
				{
					LytWString Temp=Content;
					Temp=Temp.Sub(0, TheMatchResult.Length);
					Tokens.AddLast()->Data=new Token(Temp, TempRegex->Data->Type, Position);	//Token拿走.
				}
				IsSuccessful=true;
				Content=Content+TheMatchResult.Length;	//获得去除此token之后的下一个位置.
				if (TempRegex->Data->Type == RegularExpressionType::regularENTER) 
				{ 
					Position++; 
				}	//regularENTER 代表啥? 遇到回车就行数增加,我都分不清那个是我写的，哪个注释是原来的了，就是这样，继续！
				break;
			}
			TempRegex=TempRegex->Next;
		}
		if (IsSuccessful==false)
		{
			LytWString Temp=Content;
			Temp=Temp.Sub(0, 1);	//把当前位置塞到错误列表里，然后前进一格继续。 这样的错误恢复策略，不如那些在ast里加节点的靠谱..
			ErrorTokens.AddLast()->Data=new Token(Temp, RegularExpressionType::regularERROR, Position);
			++Content;
		}
	}
}

/*
void Lexer::RunLexer(LytWString Expression)
{
	Regex::MatchResult TheMatchResult;
	int Position=1;
	const wchar_t* Content=Expression.Buffer();
	while (Content[0])//////////////////////////////////////Current完了还是能读到Current[0]
	{
		int Length=0;
		bool IsExist=false;
		Token* Final=new Token;
		Node<RegexToken*>* TempRegex=Regexs.GetHead();
		while (TempRegex)
		{
			TheMatchResult=TempRegex->Data->TheRegex->Match(Content);
			if (!(TempRegex->Data->TheRegex->IsError()))
			{
				if (Length<TheMatchResult.Length)
				{
					Length=TheMatchResult.Length;
					if (TempRegex->Data->Exist) IsExist=true;
					else IsExist=false;
					LytWString Temp=TheMatchResult.Start;
					Final->Content=Temp.Sub(0, TheMatchResult.Length);
					Final->Position=Position;
					Final->Type= TempRegex->Data->Type;
				}
				if (TempRegex->Data->Type==regularENTER) Position++;
			}
			TempRegex=TempRegex->Next;
		}
		if (!Length)
		{
			LytWString Temp=Content;
			Final->Content=Temp.Sub(0, 1);
			Final->Position=Position;
			Final->Type=regularERROR;
			++Content;
		}
		else
		{
			if (IsExist) Tokens.AddLast()->Data=Final;
			else delete Final;
			Content=Content+Length;
		}
	}
}
*/

Lexer::~Lexer()
{
	Node<RegexToken*>* Temp=Regexs.GetHead();
	while (Temp)
	{
		delete Temp->Data;
		Temp=Temp->Next;
	}

	Node<Token*>* TempToken=Tokens.GetHead();
	while (TempToken)
	{
		delete TempToken->Data;
		TempToken=TempToken->Next;
	}

	TempToken=ErrorTokens.GetHead();
	while (TempToken)
	{
		delete TempToken->Data;
		TempToken=TempToken->Next;
	}
}

bool Lexer::IsError()
{
	if (ErrorTokens.GetHead())
		return true;
	else return false;
}

void Lexer::Print()
{
	cout<<"Tokens :"<<endl;
	Node<Token*>* TempToken=Tokens.GetHead();
	while (TempToken)
	{
		wcout<<L"【"<<TempToken->Data->Content<<L"】\tPosition:"<<TempToken->Data->Position<<L"\tType:";
		switch (TempToken->Data->Type)
		{
			case RegularExpressionType::regularCOMMENT:
				cout<<"Comment";
				break;
			case RegularExpressionType::regularSTRING:
				cout<<"String";
				break;
			case RegularExpressionType::regularINTEGER:
				cout<<"Integer";
				break;
			case RegularExpressionType::regularREAL:
				cout<<"Real";
				break;
			case RegularExpressionType::regularID:
				cout<<"Id";
				break;
			case RegularExpressionType::regularOPERATOR:
				cout<<"Operator";
				break;
			case RegularExpressionType::regularSPACE:
				cout<<"Space";
				break;
			case RegularExpressionType::regularENTER:
				cout<<"ENTER";
				break;
		}
		cout<<endl;
		TempToken=TempToken->Next;
	}

	if (IsError())
	{
		cout<<"ErrorTokens :"<<endl;
		TempToken=ErrorTokens.GetHead();
		while (TempToken)
		{
			wcout<<L"【"<<TempToken->Data->Content<<L"】\tPosition:"<<TempToken->Data->Position<<endl;
			TempToken=TempToken->Next;
		}
	}
}