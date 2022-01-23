#include "GrammarTree.h"

GrammarTree::GrammarTree()
{
	ExpressionBuffer=0;
}

GrammarTree::GrammarTree(const LytWString Input)
{
	Initialize(Input);
}

void GrammarTree::Initialize(const LytWString Input)
{
	Expression=Input;
	ExpressionBuffer=Expression.Buffer();
}

void GrammarTree::GetGrammarTree()
{
	GetA(Data.TreeHead);
}

void GrammarTree::SetErrorMessage(const LytWString ErrorInformation)
{
	ErrorMessage=L"正则表达式出错 : "+ErrorInformation;
}

LytWString GrammarTree::GetErrorMessage()const
{
	return ErrorMessage;
}

bool GrammarTree::IsError()const
{
	if (ErrorMessage==L"")
		return false;
	else return true;
}

// A = A '|' B
// A = B

TreeNode<GrammarTreeNodeData>* GrammarTree::GetA(TreeNode<GrammarTreeNodeData>* Current)
{
	if (IsError())
		return 0;
	else
	{
		TreeNode<GrammarTreeNodeData>* Base=Current;
		if (ExpressionBuffer!=0 && (ExpressionBuffer[0]==L'|' || ExpressionBuffer[0]==L'+' || ExpressionBuffer[0]==L'*' || ExpressionBuffer[0]==L'?' || ExpressionBuffer[0]==L')' || ExpressionBuffer[0]==L']'))
		{
			SetErrorMessage(L"表达式中 "+(LytWString)ExpressionBuffer[0]+L" 出现在错误的位置");
			return 0;
		}
		Current=GetB(Current);
		if (IsError())
			return 0;
		else
		{
			Base=Current;
			while (ExpressionBuffer!=0 && ExpressionBuffer[0]==L'|')
			{
				++ExpressionBuffer;
				if (ExpressionBuffer==0 || ExpressionBuffer[0]==L'\0')
				{
					SetErrorMessage(L"表达式非法结束，| 操作符缺少右操作数");
					return 0;
				}
				else if (ExpressionBuffer[0]==L'|' || ExpressionBuffer[0]==L'?' || ExpressionBuffer[0]==L'*' || ExpressionBuffer[0]==L'+' || ExpressionBuffer[0]==L')' || ExpressionBuffer[0]==L']')
				{
					SetErrorMessage(L"表达式非法结束，| 操作符非法连接右操作数 "+LytWString(ExpressionBuffer[0]));
					return 0;
				}
				--ExpressionBuffer;
				Current=Data.MoveLeft(Base);
				Base=Current;
				Current->Data.Data=ExpressionBuffer[0];
				Current->Data.Type=Operator;
				++ExpressionBuffer;
				Current=Data.AddRight(Current);
				Current=GetB(Current);
			}
			return Base;
		}
	}
}


// B = BC
// B = C

TreeNode<GrammarTreeNodeData>* GrammarTree::GetB(TreeNode<GrammarTreeNodeData>* Current)
{
	if (IsError())
		return 0;
	else
	{
		TreeNode<GrammarTreeNodeData>* Base=Current;
		Current=GetC(Current);
		if (IsError())
			return 0;
		else
		{
			Base=Current;
			if (ExpressionBuffer!=0 && (ExpressionBuffer[0]==L'-' || ExpressionBuffer[0]==L'^'))
			{
				SetErrorMessage(L"表达式非法连接 "+LytWString(ExpressionBuffer[0]));
				return 0;
			}
			while (ExpressionBuffer!=0 && ExpressionBuffer[0]!=L'\0' && ExpressionBuffer[0]!=L'|' && ExpressionBuffer[0]!=L'+' && ExpressionBuffer[0]!=L'*' && ExpressionBuffer[0]!=L'?' && ExpressionBuffer[0]!=L'|' && ExpressionBuffer[0]!=L')' && ExpressionBuffer[0]!=']')
			{
				Current=Data.MoveLeft(Base);
				Base=Current;
				Current->Data.Data=L'\0';
				Current->Data.Type=Operator;
				Current=Data.AddRight(Current);
				Current=GetC(Current);
			}
			return Base;
		}
	}
}


// C = C '+'
// C = C '*'
// C = C '?'
// C = '(' A ')'
// C = D

TreeNode<GrammarTreeNodeData>* GrammarTree::GetC(TreeNode<GrammarTreeNodeData>* Current)
{
	if (IsError())
		return 0;
	else 
	{
		TreeNode<GrammarTreeNodeData>* Base=Current;
		if (ExpressionBuffer!=0 && ExpressionBuffer[0]==L'(')
		{
			++ExpressionBuffer;
			if (ExpressionBuffer==0 || ExpressionBuffer[0]==L'\0')
			{
				SetErrorMessage(L"表达式非法结束，( 操作符缺少右操作数");
				return 0;
			}
			Current=GetA(Current);
			if (IsError())
				return 0;
			else
			{
				Base=Current;
				if (ExpressionBuffer==0 || ExpressionBuffer[0]!=L')')
				{
					SetErrorMessage(L"表达式非法结束，缺少 )");
					return 0;
				}
				++ExpressionBuffer;
			}
		}
		else
		{
			Current=GetD(Current);
			if (IsError())
				return 0;
			else Base=Current;
		}
		while (ExpressionBuffer!=0 && (ExpressionBuffer[0]==L'+' || ExpressionBuffer[0]==L'*' || ExpressionBuffer[0]==L'?'))
		{
			Current=Data.MoveLeft(Base);
			Base=Current;
			Current->Data.Data=ExpressionBuffer[0];
			Current->Data.Type=Operator;
			++ExpressionBuffer;
		}
		return Base;
	}
}


// D = char
// D = '\' char
// D = '[' E ']'
// D = "[^" E ']'

TreeNode<GrammarTreeNodeData>* GrammarTree::GetD(TreeNode<GrammarTreeNodeData>* Current)
{
	if (IsError())
		return 0;
	else
	{
		TreeNode<GrammarTreeNodeData>* Base=Current;
		if (ExpressionBuffer!=0 && ExpressionBuffer[0]==L'\\')
		{
			++ExpressionBuffer;
			if (ExpressionBuffer==0 || ExpressionBuffer[0]==L'\0')
			{
				SetErrorMessage(L"表达式非法结束，\\ 操作符缺少右操作数");
				return 0;
			}
			if (ExpressionBuffer[0]==L'\\' || ExpressionBuffer[0]==L'|' || ExpressionBuffer[0]==L'+' || ExpressionBuffer[0]==L'*' || ExpressionBuffer[0]==L'?' || ExpressionBuffer[0]==L'(' || ExpressionBuffer[0]==L')' || ExpressionBuffer[0]==L'[' || ExpressionBuffer[0]==L']' || ExpressionBuffer[0]==L'^' || ExpressionBuffer[0]==L'-')
			{
				Current->Data.Data=ExpressionBuffer[0];
				Current->Data.Type=Char;
				++ExpressionBuffer;
			}
			else if (ExpressionBuffer[0]==L'r' || ExpressionBuffer[0]==L'n' || ExpressionBuffer[0]==L't')
			{
				Current->Data.Data=ExpressionBuffer[0];
				Current->Data.Type=Transferred;
				++ExpressionBuffer;
			}
			else if (ExpressionBuffer[0]==L'w' || ExpressionBuffer[0]==L'd' || ExpressionBuffer[0]==L'l' || ExpressionBuffer[0]==L'W' || ExpressionBuffer[0]==L'D' || ExpressionBuffer[0]==L'L')
			{
				if (ExpressionBuffer[0]==L'w')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;
	
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'_';
					Current->Right->Data.Type=Char;
	
					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'0';
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'9';
					Current->Right->Right->Data.Type=CharGroup;
	
					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=L'A';
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=L'Z';
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'a';
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'z';
					Current->Right->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'd')
				{
					Current->Data.Data=L'-';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'0';
					Current->Left->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'9';
					Current->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'l')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=L'A';
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=L'Z';
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'a';
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'z';
					Current->Right->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'W')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=(wchar_t)0;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'0'-1;
					Current->Right->Right->Data.Type=CharGroup;

					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'9'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'A'-1;
					Current->Right->Right->Data.Type=CharGroup;

					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'Z'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'_'-1;
					Current->Right->Right->Data.Type=CharGroup;

					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=L'z'+1;
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=(wchar_t)65535;
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'_'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'a'-1;
					Current->Right->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'D')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=(wchar_t)0;
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=L'0'-1;
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'9'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=(wchar_t)65535;
					Current->Right->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'L')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=(wchar_t)0;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'A'-1;
					Current->Right->Right->Data.Type=CharGroup;

					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=L'Z'+1;
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=L'a'-1;
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'z'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=(wchar_t)65535;
					Current->Right->Right->Data.Type=CharGroup;
				}
				++ExpressionBuffer;
			}
			else
			{
				SetErrorMessage(L"表达式非法结束，\\ 操作符非法连接右操作数");
				return 0;
			}
		}
		else if (ExpressionBuffer!=0 && ExpressionBuffer[0]==L'[')
		{
			++ExpressionBuffer;
			if (ExpressionBuffer==0 || ExpressionBuffer[0]==L'\0')
			{
				SetErrorMessage(L"表达式非法结束，[ 操作符缺少右操作数");
				return 0;
			}
			if (ExpressionBuffer!=0 && ExpressionBuffer[0]==L'^')
			{
				Current->Data.Data=ExpressionBuffer[0];
				Current->Data.Type=Operator;
				Current=Data.AddLeft(Current);
				++ExpressionBuffer;
				Current=GetE(Current);
				if (IsError())
					return 0;
				else
				{
					Base->Left=Current;
					Base->Right=0;
				}
			}
			else
			{
				Current=GetE(Current);
				if (IsError())
					return 0;
				else Base=Current;
			}
			if (ExpressionBuffer==0 || ExpressionBuffer[0]!=L']')
			{
				SetErrorMessage(L"表达式非法结束，缺少 ]");
				return 0;
			}
			++ExpressionBuffer;
		}
		else
		{
			Current->Data.Data=ExpressionBuffer[0];
			Current->Data.Type=Char;
			++ExpressionBuffer;
		}
		return Base;
	}
}


// E = EF
// E = F

TreeNode<GrammarTreeNodeData>* GrammarTree::GetE(TreeNode<GrammarTreeNodeData>* Current)
{
	if (IsError())
		return 0;
	else
	{
		TreeNode<GrammarTreeNodeData>* Base=Current;
		Current=GetF(Current);
		if (IsError())
			return 0;
		else
		{
			Base=Current;
			if (ExpressionBuffer[0]==L'|' || ExpressionBuffer[0]==L'+' || ExpressionBuffer[0]==L'*' || ExpressionBuffer[0]==L'?')
			{
				SetErrorMessage(L"表达式[]中不可出现 "+LytWString(ExpressionBuffer[0]));
				return 0;
			}
			while (ExpressionBuffer!=0 && ExpressionBuffer[0]!=L'\0' && ExpressionBuffer[0]!=L')' && ExpressionBuffer[0]!=L']')
			{
				Current=Data.MoveLeft(Base);
				Base=Current;
				Current->Data.Data=L'|';
				Current->Data.Type=Operator;
				Current=Data.AddRight(Current);
				Current=GetF(Current);
			}
		}
		return Base;
	}
}


// F = char '-' char
// F = char
// F = '\' char

TreeNode<GrammarTreeNodeData>* GrammarTree::GetF(TreeNode<GrammarTreeNodeData>* Current)
{
	if (IsError())
		return 0;
	else
	{
		TreeNode<GrammarTreeNodeData>* Base=Current;
		if (ExpressionBuffer[0]==L'\\')
		{
			++ExpressionBuffer;
			if (ExpressionBuffer==0 || ExpressionBuffer[0]==L'\0')
			{
				SetErrorMessage(L"表达式非法结束，\\ 操作符缺少右操作数");
				return 0;
			}
			if (ExpressionBuffer[0]==L'\\' || ExpressionBuffer[0]==L'+' || ExpressionBuffer[0]==L'*' || ExpressionBuffer[0]==L'?' || ExpressionBuffer[0]==L'(' || ExpressionBuffer[0]==L')' || ExpressionBuffer[0]==L'[' || ExpressionBuffer[0]==L']' || ExpressionBuffer[0]==L'^' || ExpressionBuffer[0]==L'-')
			{
				Current->Data.Data=ExpressionBuffer[0];
				Current->Data.Type=Char;
				++ExpressionBuffer;
			}
			else if (ExpressionBuffer[0]==L'r' || ExpressionBuffer[0]==L'n' || ExpressionBuffer[0]==L't')
			{
				Current->Data.Data=ExpressionBuffer[0];
				Current->Data.Type=Transferred;
				++ExpressionBuffer;
			}
			else if (ExpressionBuffer[0]==L'w' || ExpressionBuffer[0]==L'd' || ExpressionBuffer[0]==L'l' || ExpressionBuffer[0]==L'W' || ExpressionBuffer[0]==L'D' || ExpressionBuffer[0]==L'L')
			{
				if (ExpressionBuffer[0]==L'w')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;
	
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'_';
					Current->Right->Data.Type=Char;
	
					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'0';
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'9';
					Current->Right->Right->Data.Type=CharGroup;

					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=L'A';
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=L'Z';
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'a';
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'z';
					Current->Right->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'd')
				{
					Current->Data.Data=L'-';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'0';
					Current->Left->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'9';
					Current->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'l')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=L'A';
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=L'Z';
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'a';
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'z';
					Current->Right->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'W')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=(wchar_t)0;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'0'-1;
					Current->Right->Right->Data.Type=CharGroup;

					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'9'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'A'-1;
					Current->Right->Right->Data.Type=CharGroup;

					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'Z'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'_'-1;
					Current->Right->Right->Data.Type=CharGroup;

					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=L'z'+1;
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=(wchar_t)65535;
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'_'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'a'-1;
					Current->Right->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'D')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=(wchar_t)0;
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=L'0'-1;
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'9'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=(wchar_t)65535;
					Current->Right->Right->Data.Type=CharGroup;
				}
				else if (ExpressionBuffer[0]==L'L')
				{
					Current->Data.Data=L'|';
					Current->Data.Type=Operator;

					Data.AddLeft(Current);
					Current->Left->Data.Data=L'|';
					Current->Left->Data.Type=Operator;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=(wchar_t)0;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=L'A'-1;
					Current->Right->Right->Data.Type=CharGroup;

					Current=Current->Left;
					Data.AddLeft(Current);
					Current->Left->Data.Data=L'-';
					Current->Left->Data.Type=Operator;
					Data.AddLeft(Current->Left);
					Current->Left->Left->Data.Data=L'Z'+1;
					Current->Left->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Left);
					Current->Left->Right->Data.Data=L'a'-1;
					Current->Left->Right->Data.Type=CharGroup;
					Data.AddRight(Current);
					Current->Right->Data.Data=L'-';
					Current->Right->Data.Type=Operator;
					Data.AddLeft(Current->Right);
					Current->Right->Left->Data.Data=L'z'+1;
					Current->Right->Left->Data.Type=CharGroup;
					Data.AddRight(Current->Right);
					Current->Right->Right->Data.Data=(wchar_t)65535;
					Current->Right->Right->Data.Type=CharGroup;
				}
				++ExpressionBuffer;
			}
			else
			{
				SetErrorMessage(L"表达式非法结束，\\ 操作符非法连接右操作数");
				return 0;
			}
		}
		else
		{
			Current->Data.Data=ExpressionBuffer[0];
			Current->Data.Type=Char;
			++ExpressionBuffer;
			if (ExpressionBuffer!=0 && ExpressionBuffer[0]==L'-')
			{
				++ExpressionBuffer;
				if (ExpressionBuffer==0 || ExpressionBuffer[0]==L'\0')
				{
					SetErrorMessage(L"表达式非法结束，- 操作符缺少右操作数");
					return 0;
				}
				Current->Data.Type=CharGroup;
				--ExpressionBuffer;
				Current=Data.MoveLeft(Current);
				Base=Current;
				Current->Data.Data=ExpressionBuffer[0];
				Current->Data.Type=Operator;
				++ExpressionBuffer;
				if (ExpressionBuffer[0]<=Current->Left->Data.Data)
				{
					SetErrorMessage(L"表达式非法结束，- 中左操作数不小于右操作数");
					return 0;
				}
				Current=Data.AddRight(Current);
				Current->Data.Data=ExpressionBuffer[0];
				Current->Data.Type=CharGroup;
				++ExpressionBuffer;
			}
		}
		return Base;
	}
}