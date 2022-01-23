#include "Parser.h"

LytPtr<Program> Parse(Node<Token*>* Current)
{
	if (Current) return GetProgram(Current);
	else return 0;
}

//不是存到map里然后找在不在，而是直接比较字符串.
bool IsKeyWord(LytWString Content)
{
	if (Content==L"function" || Content==L"procedure" || Content==L"var" || Content==L"integer" || Content==L"boolean" || Content==L"real" || Content==L"true" || Content==L"false" || Content==L"var" || Content==L"begin" || Content==L"end" || Content==L"if" || Content==L"then" || Content==L"else" || Content==L"while" || Content==L"do" || Content==L"return" || Content==L"break" || Content==L"continue") return true;
	else return false;
}

//同上
bool IsDataType(LytWString Content)
{
	if (Content==L"integer" || Content==L"boolean" || Content==L"real") return true;
	else return false;
}

//程序就是一大坨函数，哪里都一样~
LytPtr<Program> GetProgram(Node<Token*>*& Current)
{
	LytPtr<Program> Result=new Program;
	if (!Current)return 0;	
	else
	{
		LytPtr<FunctionHead> TheFunctionHead;	//【函数头是关键字function+函数名 这里把这些放到一起了.
		LytPtr<Function> TheFunction;			//【函数包括头，局部变量，和体
		int Status=1;
		while (Current)
		{
			switch (Status)
			{
				case 1:
					if (Current->Data->Content==L"function" || Current->Data->Content==L"procedure") TheFunctionHead=GetFunctionHead(Current);
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数类型出错"));
					Status=2;
					if (!Current)	//0跟前面一样，如果现在没有值了，则!nullptr为真，暂时知道什么是函数声明了.
					{
						Result->FunctionDeclarations.AddLast()->Data=new FunctionDeclaration(TheFunctionHead);
						return Result;
					}
					break;
				case 2:
					Status=1;
					if (Current->Data->Content==L";")	//0如果遇到; 就说明当前函数声明完结，拉起下一个准备塞入.
					{
						Result->FunctionDeclarations.AddLast()->Data=new FunctionDeclaration(TheFunctionHead);
						Current=Current->Next;
						if (!Current) return Result;
					}
					else if (Current->Data->Content==L"var" || Current->Data->Content==L"begin")//当前下一个Token,则再造一个函数出来？
					{
						TheFunction=GetFunction(Current);		//为什么函数里会需要var 开头?
						//根据 https://www.tutorialspoint.com/pascal/pascal_functions.htm var确实是函数体的一部分.
						TheFunction->TheFunctionHead=TheFunctionHead;	//TheFunctionHead的生命周期?
						Result->Functions.AddLast()->Data=TheFunction;
						if (!Current) return Result;
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数声明缺少分号"));
					break;
			}
		}
	}
	return 0;
}

//代表的是函数体,没有了.
LytPtr<Function> GetFunction(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<Function> Result=new Function;
		if (Current->Data->Content==L"var")	//https://www.tutorialspoint.com/pascal/pascal_functions.htm
		{
			if (Current->Next) Current=Current->Next;
			else throw LytPtr<Error>(new Error(*(Current->Data),L"缺少函数体"));
			Result->TheVar=GetVar(Current);
		}
		if (!Current) throw LytPtr<Error>(new Error(Result->TheVar->Content.Tail->Data->Type,L"缺少函数体"));
		else if (Current->Data->Content==L"begin")
		{
			Result->Body=GetCompoundStatement(Current);
			return Result;
		}
		else return 0;
	}
}

//获得一个变量怎么会这么麻烦 =_= ,这个Var里有一个list的Variable，看来是用来吃掉，无论是全局还是Local,那里面一大堆变量声明的? 没有TheDataType，struct的?
LytPtr<Var> GetVar(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<Var> Result=new Var;
		Link<Token> Ids;
		Token Type;
		int Status=1;
		while (Current)
		{
			switch (Status)
			{
				case 1:
					if (Current->Data->Type== RegularExpressionType::regularID)	//熟悉的开场，先吃一个变量名，这个变量名是由正则机器确定出来的.
					{
						if (IsKeyWord(Current->Data->Content)) throw LytPtr<Error>(new Error(*(Current->Data),L"var变量名不能为关键字"));
						Ids.AddLast()->Data=*(Current->Data);
						Status=2;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"var变量名缺少类型说明"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"var变量名出错"));
					break;
				case 2:
					if (Current->Data->Content==L":")	//类型出场
					{
						Status=3;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"var变量名缺少类型说明"));
					}
					else if (Current->Data->Content==L",")	//如果多个，直接回到1继续处理就好，写成文法的话，就是在rest里递归了.
					{
						Status=1;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"var逗号后缺少变量"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"var变量名后缺少冒号"));
					break;
				case 3:
					if (Current->Data->Type== RegularExpressionType::regularID)
					{
						if (!IsDataType(Current->Data->Content)) throw LytPtr<Error>(new Error(*(Current->Data),L"var中变量类型出错"));
						Type=*(Current->Data);
						Node<Token>* Temp=Ids.GetHead();	//哦，这个是，要把前面每个的类型都塞进去.
						while (Temp)
						{
							Result->Content.AddLast()->Data=new Variable(Temp->Data, Type);
							Temp=Temp->Next;
						}
						Ids.Clear();
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"var后缺少函数体"));
						if (Current->Data->Content==L";")	//剩下的参数:function name(argument(s): type1; argument(s): type2; ...): function_type;
						{
							Status=1;
							Current=Current->Next;
							if (!Current) return Result;
							else if (Current->Data->Content==L"begin") return Result;
						}
						else if (Current->Data->Content==L"begin") return Result;
						else Status=1;
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"var变量类型出错"));
					break;
			}
		}
	}
	return 0;
}

//然后就是函数头了.. 等等，总感觉这么熟悉一定有问题，我读SimpleC的时候也这样啊.. 学到了啥呢?  别的先不管，此函数很好读懂.
LytPtr<FunctionHead> GetFunctionHead(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<FunctionHead> Result=new FunctionHead;	//回忆一下，函数头是"函数头由function关键字和此函数的名字组成"
		int Status=1;
		Token Id;
		Token Type;
		while (Current)
		{
			switch(Status)
			{
				case 1:
					if (Current->Data->Content==L"function" || Current->Data->Content==L"procedure")	//这个吃掉了函数头的关键字
					{
						Result->FunctionType=*(Current->Data);
						if (Current->Data->Content==L"function") Result->IsFunction=true;
						else Result->IsFunction=false;
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数类型出错"));
					Status=2;
					if (Current->Next) Current=Current->Next;
					else throw LytPtr<Error>(new Error(*(Current->Data),L"缺少函数名"));
					break;
				case 2:		//这个吃掉了函数名. regex的一堆事待会再说.
					if (Current->Data->Type!= RegularExpressionType::regularID) throw LytPtr<Error>(new Error(*(Current->Data),L"函数名出错"));
					else if (IsKeyWord(Current->Data->Content)) throw LytPtr<Error>(new Error(*(Current->Data),L"函数名不能为关键字"));
					Result->FunctionName=*(Current->Data);
					Status=3;
					if (Current->Next) Current=Current->Next;
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"缺少参数表"));
					break;
				case 3:		//这个不需要?
					if (Current->Data->Content==L"(")
					{
						Status=4;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数表缺少右括号"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数表缺少左括号"));
					break;
				case 4:		//哦，这个是无参类型的，函数类型在下面。
					if (Current->Data->Content==L")")
					{
						Status=8;
						if (Result->IsFunction)
						{
							if (Current->Next) Current=Current->Next;
							else LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"缺少返回类型"));
						}
						else
						{
							Current=Current->Next;
							return Result;
						}
					}		//有参数的处理分支在这里呀！ 人家的格式是，先参数名再类型
					else if (Current->Data->Type== RegularExpressionType::regularID)
					{
						if (IsKeyWord(Current->Data->Content)) throw LytPtr<Error>(new Error(*(Current->Data),L"函数参数名不能为关键字"));	//这里就是参数名字了?
						Id=*(Current->Data);
						Status=5;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数缺少返回类型"));	//应该是这里.
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数表含有非法字符"));
					break;
				case 5:
					//函数声明是这样，函数头跟他是一回事吗 
					//function name(argument(s): type1; argument(s): type2; ...): function_type; 前面吃完type 这里就? 下面的提示说得清清楚楚了，这个:是type1前面的那个！
					if (Current->Data->Content==L":")
					{
						Status=6;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数缺少类型说明"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数表中缺少冒号"));
					break;
				case 6:
					if (Current->Data->Type== RegularExpressionType::regularID)	//这里处理type1.
					{
						if (!IsDataType(Current->Data->Content)) LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数类型出错"));
						Type=*(Current->Data);
						Result->ParameterList.AddLast()->Data=new Parameter(Id, Type);	//好吧这里确实有Parameter的，原来在这里，刚刚没看到.
						Status=7;		
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"缺少右括号"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数类型出错"));
					break;
				case 7:		//然后为啥不管是否有其他的参数就直接)了? 没有，这里是if 啊。
					if (Current->Data->Content==L")")
					{
						Status=8;	//如果已经闭合了，就判断是否为函数，然后返回.. ，前几个分支有return 的吗? 只有4 有，为什么呢? 因为4代表了一种终结状态的到达(废话..)
						if (Result->IsFunction)
						{
							if (Current->Next) Current=Current->Next;
							else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"缺少返回类型说明"));
						}
						else
						{
							Current=Current->Next;
							return Result;
						}
					}
					else if (Current->Data->Content==L";")	//如果没有完，就跳回4继续处理剩下的函数.
					{
						Status=4;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数表中分号后缺少参数"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"缺少右括号"));
					break;
				case 8:
					if (Current->Data->Content==L":")	//终于是返回类型了.
					{
						Status=9;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"缺少返回类型说明"));
					}
					else
					{
						if (Result->IsFunction) throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"参数表后缺少冒号"));
						else return Result;
					}
					break;
				case 9:
					if (Current->Data->Type== RegularExpressionType::regularID)
					{
						if (!IsDataType(Current->Data->Content)) LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"返回类型出错"));
						Result->ReturnType=*(Current->Data);
						Current=Current->Next;
						return Result;
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"函数"+Result->FunctionName.Content+L"返回类型出错"));
					break;
			}
		}
	}
	return 0;
}

//这个就是把那些综合起来的一个非终结符,递归大法好
LytPtr<Statement> GetStatement(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<Statement> Result;
		if (Current->Data->Content==L"if")
			Result=GetIfStatement(Current);
		else if (Current->Data->Content==L"while")
			Result=GetWhileDoStatement(Current);
		else if (Current->Data->Content==L"do")
			Result=GetDoWhileStatement(Current);
		else if (Current->Data->Content==L"begin")
			Result=GetCompoundStatement(Current);
		else if (Current->Data->Content==L"return")
			Result=GetReturnStatement(Current);
		else if (Current->Data->Content==L"break" || Current->Data->Content==L"continue")
			Result=GetControlStatement(Current);
		else Result=GetAssignStatementOrSimpleStatement(Current);
		return Result;
	}
}

//简单清脆小黄瓜
LytPtr<Statement> GetAssignStatementOrSimpleStatement(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<AssignmentStatement> Result;	//赋值是干嘛的?	哦，这是赋值表达式啊本币
		LytPtr<Expression> Left;
		LytPtr<Expression> Right;
		int Status=1;
		while (Current)
		{
			switch (Status)
			{
				case 1:
					Left=GetExpression(Current);	//这个里面是会递增的，嗯。
					if (Current && Current->Data->Content==L":=")
					{
						Result=new AssignmentStatement;
						Result->Left=Left;
						Status=2;
					}
					else
					{
						LytPtr<SimpleStatement> Result=new SimpleStatement;
						Result->Content=Left;
						return Result;
					}
					break;
				case 2:
					if (Current->Data->Content==L":=")
					{
						Status=3;
						Result->Operator=*(Current->Data);
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"赋值语句缺少右值"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"赋值语句缺少赋值号"));
					break;
				case 3:
					Result->Right=GetExpression(Current);
					return Result;
					break;
			}
		}
	}
	return 0;
}

//if else 的二义性问题在文法层面解决了，递归是间接.
LytPtr<IfStatement> GetIfStatement(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<IfStatement> Result=new IfStatement;
		LytPtr<Expression> Condition;
		LytPtr<Statement> Then;
		LytPtr<Statement> Else;
		int Status=1;
		while (Current)
		{
			switch (Status)
			{
				case 1:
					Status=2;
					Result->IfToken=*(Current->Data);
					if (Current->Next) Current=Current->Next;
					else throw LytPtr<Error>(new Error(*(Current->Data),L"if语句缺少条件表达式"));
					break;
				case 2:
					if (Current->Data->Content==L"(")	
					{
						Status=3;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"if语句缺少条件表达式"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"if语句条件表达式缺少左括号"));
					break;
				case 3:	
					Result->Condition=GetExpression(Current);	////这个就是它的"if语句缺少条件表达式".
					Status=4;
					break;
				case 4:
					if (Current->Data->Content==L")")
					{
						Status=5;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"if语句缺少then语句"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"if语句条件表达式缺少右括号"));
					break;
				case 5:
					if (Current->Data->Content==L"then")
					{
						Status=6;
						Result->ThenToken=*(Current->Data);
						if(Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"if语句then后缺少语句"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"if语句缺少\"then\""));
					break;
				case 6:
					Result->Then=GetStatement(Current);
					if (Current && Current->Data->Content==L"else") Status=7;
					else return Result;
					break;
				case 7:
					Status=8;
					Result->ElseToken=*(Current->Data);
					if(Current->Next) Current=Current->Next;
					else throw LytPtr<Error>(new Error(*(Current->Data),L"if语句else后缺少语句"));
					break;
				case 8:
					Result->Else=GetStatement(Current);
					return Result;
					break;
			}
		}
	}
	return 0;
}

//跟下面一个差不多.
LytPtr<WhileStatement> GetWhileDoStatement(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<WhileStatement> Result=new WhileStatement;
		Result->IsWhileDo=true;
		LytPtr<Expression> Condition;
		LytPtr<Statement> Do;
		int Status=1;
		while (Current)
		{
			switch (Status)
			{
				case 1:
					Status=2;
					Result->WhileToken=*(Current->Data);	//如果Do那几个分别代表对应的子表达式，While token是啥？ 存了字符串的？
					if (Current->Next) Current=Current->Next;
					else throw LytPtr<Error>(new Error(*(Current->Data),L"while语句缺少条件表达式"));
					break;
				case 2:
					if (Current->Data->Content==L"(")
					{
						Status=3;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"while语句缺少条件表达式"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"while语句条件表达式缺少左括号"));
					break;
				case 3:
					Result->Condition=GetExpression(Current);
					Status=4;
					break;
				case 4:
					if (Current->Data->Content==L")")
					{
						Status=5;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"while语句缺少do语句"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"while语句条件表达式缺少右括号"));
					break;
				case 5:
					if (Current->Data->Content==L"do")
					{
						Status=6;
						Result->DoToken=*(Current->Data);	//看起来就是这样.
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"while语句do后缺少语句"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"while语句条件表达式缺少\"do\""));
					break;
				case 6:
					Result->Do=GetStatement(Current);
					return Result;
					break;
			}
		}
	}
	return 0;
}

//作者自己加的do-while
LytPtr<WhileStatement> GetDoWhileStatement(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<WhileStatement> Result=new WhileStatement;
		Result->IsWhileDo=false;	//do-while语句为假
		LytPtr<Statement> Do;	//WhileStatement 里的俩结构体
		LytPtr<Expression> Condition;
		int Status=1;
		while (Current)
		{
			switch (Status)
			{
				case 1:	//跟以前一样，第一次先进1
					Status=2;
					Result->DoToken=*(Current->Data);	//吃掉名为关键词 的Do?
					if (Current->Next) Current=Current->Next;	//并且步进
					else throw LytPtr<Error>(new Error(*(Current->Data),L"dowhile语句缺少do语句"));
					break;
				case 2:
					Result->Do=GetStatement(Current);		
					//这个语句就是Do的除了关键词之外的部分了? 去看下语法~ 人家pascal好像没有这东西啊..
					// https://www.programming-idioms.org/idiom/78/do-while-loop/930/pascal 看起来似乎没有，去翻下设计者的博客.
					// 看起来是自己加的，直接照着对比下吧.. https://www.tutorialspoint.com/pascal/pascal_while_do_loop.htm
					//   while  a < 20  do 大概这样.	所以这里接受的是等同于 a<20 这条语句，大概.
					Status=3;
					if (!Current) throw LytPtr<Error>(new Error(Result->DoToken, L"dowhile语句缺少条件表达式"));
					break;
				case 3:
					if (Current->Data->Content==L"while")	//然后接受后面的while.
					{
						Status=4;
						Result->WhileToken=*(Current->Data);	//神了，竟然能停在这条上，我以为源代码没有这种表达式.. 难不成do-while 和while-do 有一腿？没有谢谢。
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"dowhile语句缺少条件表达式"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"dowhile语句缺少\"while\""));
					break;
				case 4:
					if (Current->Data->Content==L"(")
					{
						Status=5;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"dowhile语句缺少条件表达式"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"dowhile语句条件表达式缺少左括号"));
					break;
				case 5:
					Result->Condition=GetExpression(Current);
					Status=6;
					break;
				case 6:
					if (Current->Data->Content==L")")
					{
						Current=Current->Next;
						return Result;
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"while语句条件表达式缺少右括号"));
					break;
			}
		}
	}
	return 0;
}

//状态机登场！
LytPtr<Block> GetCompoundStatement(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<Block> Result=new Block;
		int Status=1;
		while (Current)
		{
			switch (Status)
			{
				case 1:
					if (Current->Data->Content==L"begin")
					{
						Status=2;	//0 这个状态是干嘛的？ 哪里会判断它等于几？
						if (Current) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"语句块缺少end"));	//0 只有在读入了bgein就达到文件末尾才会有这个.
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"语句块缺少begin"));	//0 所以这个status=1就是记录状态机状态的，要求第一个必须是begin. 并且设置为2,这样下次再调用? 不是，是while循环到下一次的时候就优先匹配end?
					break;
				case 2:
					if (Current->Data->Content==L"end")
					{
						Current=Current->Next;
						return Result;
					}
					else
					{
						Result->StatementList.AddLast()->Data=GetStatement(Current);	//是的，匹配到状态2之后不是begin之后必须end,还有这个分支可选，用来塞一堆状态，并且跳到状态三 ?且如果到此为止，就报错没有end. 可是如果不为空的话，哪里递增了curr? 是GetStatement()里的函数们，直接读有点累，改天点一个静态分析的理论知识！
						Status=3;	//这里设置了状态值有啥用？ 哦，下次进入的时候。 所以这一句放哪里都一样，就当是goto:state::three吧。下面的东西只是可有可无的错误检查.
						Token Temp;
						if (!Current) throw LytPtr<Error>(new Error(Temp,L"语句块缺少end")); //行号		
					}
					break;
				case 3:
					if (Current->Data->Content==L";")
					{
						Status=2;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data),L"语句块缺少end"));
						if (Current->Data->Content==L"end")
						{
							Current=Current->Next;
							return Result;
						}
					}
					else if (Current->Data->Content==L"end")
					{
						Current=Current->Next;
						return Result;
					}
					else throw LytPtr<Error>(new Error(*(Current->Data),L"语句含有非法字符"));
					break;
			}
		}//然后故事完结~,我觉得自己还是需要从语法层面来正向思考这些东西.
	}
	return 0;
}

//对于continue，break之类的处理
LytPtr<ControlStatement> GetControlStatement(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<ControlStatement> Result=new ControlStatement;
		if (Current->Data->Content==L"continue" || Current->Data->Content==L"break")
		{
			Result->Content=*(Current->Data);
			Current=Current->Next;	//这个更新是类似于tokens[++i]的作用
			return Result;
		}
		else throw LytPtr<Error>(new Error(*(Current->Data),L"控制语句出错"));
	}
}

//不是很熟悉pascal的返回值语句有哪些格式，所以实现里最后两行暂时不管.
LytPtr<ReturnStatement> GetReturnStatement(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<ReturnStatement> Result=new ReturnStatement;
		Result->ReturnToken=*(Current->Data);
		if (Current->Next) Current=Current->Next;
		else throw LytPtr<Error>(new Error(*(Current->Data),L"return语句出错"));
		if (Current->Data->Content==L";" || Current->Data->Content==L"end") return Result;
		else
		{
			Result->Return=GetExpression(Current);
			return Result;
		}
	}
}

//被各种更具体的表达式调用,这个过程可以以图的形式显示出来吗?
LytPtr<Expression> GetExpression(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		Tree<LytPtr<Base>> BinaryTree;
		GetBinaryExpTree(Current, BinaryTree);
		LytPtr<Expression> Result=GetBinaryExpression(BinaryTree.TreeHead);
		return Result;
	}
}

//二元树跟二元表达式有啥区别? 哦，这个是纯粹的遍历，看上边，先用ExpTree建合适的树，然后这个函数输出结果，嗯。
LytPtr<Base> GetBinaryExpression(TreeNode<LytPtr<Base>>* Current)
{
	if (Current->Left || Current->Right)
	{
		LytPtr<BinaryExpression> Result=new BinaryExpression;
		if (Current->Left) Result->Left=GetBinaryExpression(Current->Left);
		if (Current->Right) Result->Right=GetBinaryExpression(Current->Right);
		Result->Operator=Current->Data;
		return Result;
	}
	else
	{
		LytPtr<Expression> Result=Current->Data;
		return Result;
	}
}

//二元表达式建树搞定，SimpleC里是怎么做的?
void GetBinaryExpTree(Node<Token*>*& Current, Tree<LytPtr<Base>>& BinaryTree)
{
	if (!Current) return;
	else
	{
		TreeNode<LytPtr<Base>>* Root=BinaryTree.TreeHead;
		TreeNode<LytPtr<Base>>* Present=Root;

		LytPtr<Expression> Left=GetSimpleExpression(Current);	//这俩函数会更新current的值吗?会
		Present->Data=Left;

		while (Current)	//0 这个while其实代表了rest()的递归调用.		下一次循环的时候，current已经在line 727和GetSimpleExpression得到了更新，现在指向的是创建了树之后的下一个节点.
		{
			LytPtr<OperatorBase> BinaryOperator;
			if (Current->Data->Content==L"<" || Current->Data->Content==L"<=" || Current->Data->Content==L">=" || Current->Data->Content==L">" || Current->Data->Content==L"!=" || Current->Data->Content==L"=")
				BinaryOperator=new OperatorBase(*(Current->Data), opRELATION);
			else if (Current->Data->Content==L"xor")
				BinaryOperator=new OperatorBase(*(Current->Data), opXOR);
			else if (Current->Data->Content==L"or")
				BinaryOperator=new OperatorBase(*(Current->Data), opOR);
			else if(Current->Data->Content==L"and")
				BinaryOperator=new OperatorBase(*(Current->Data), opAND);
			else if(Current->Data->Content==L"+")
				BinaryOperator=new OperatorBase(*(Current->Data), opADD);
			else if(Current->Data->Content==L"-")
				BinaryOperator=new OperatorBase(*(Current->Data), opSUB);
			else if(Current->Data->Content==L"*")
				BinaryOperator=new OperatorBase(*(Current->Data), opMUL);
			else if(Current->Data->Content==L"/")
				BinaryOperator=new OperatorBase(*(Current->Data), opREAL_DIV);
			else if(Current->Data->Content==L"div")
				BinaryOperator=new OperatorBase(*(Current->Data), opDIV);
			else if(Current->Data->Content==L"mod")
				BinaryOperator=new OperatorBase(*(Current->Data), opMOD);
			else return;
			if (Current->Next) Current=Current->Next;	//这里则是那个循环的步进.
			else throw LytPtr<Error>(new Error(*(Current->Data),L"二元操作符缺少右操作数"));
			LytPtr<Expression> Right=GetSimpleExpression(Current);	//这里是对next取表达式.

			//如果Present代表根节点的话，哦，这里代表的是left,则它的Up一开始是不存在的,跳转到2 
			//直到第二次循环开始，才关心优先级问题.
			//第二圈里，Present指向的是啥呢?MoveDown()函数把Present头上塞了一个节点，并且把它设定成了操作符。
			while (Present->Up)		//下面这些仍在化身为while循环的rest()里. 不过在第一个之外，这棵树就不是简单的那啥了. 只要not null,就是说明这棵树已经有一个操作符了，这个Up其实是操作符，所以难免比较一番=_=
			{
				LytPtr<OperatorBase> Op=Present->Up->Data;	//Op为取出了上一次的操作符， 下面执行，如果本次摸到的操作符比Op的优先级大，就要
				if (Op->Precedence < BinaryOperator->Precedence) break;	//0如果当前表达式的优先级比二元的小就跳出，否则？
				else Present=Present->Up;								//0Present 本来是树的开头，和根，这里的意思是，如果优先级比二元的大，就移动当前的位置到上面，这样Op里的东西就可以查不到别处了，如下文? 这个是找位置
			}	//理解不了这个优先级，它也不给张图，到底是想表达什么??? 设定上是乘除大于加减的优先级..


			//2 第一次创建的时候到这里，果然！  
			BinaryTree.MoveDown(Present)->Data = BinaryOperator;	//0下面才是移动 MoveDown()返回的是新建的爹节点，此时Present已经沦为左孩子或者右孩子. 鉴于一开始Present不是谁的右孩子，所以应该自己就是左孩子吧，试一下? 确实.
			//我只是想知道在下面把Present变成右孩子之前，left被存到哪里了?
			//挂在该待的位置，只是没人指向它了而已
			//AddRight()返回的是新创建的右孩子，所以这圈的终末，Present指向右孩子.
			//第二圈的时候它要向上一下?line 738到底是什么意思? 哦，只要当前子树的操作符优先级不如，Op<下一个操作符，
			Present=BinaryTree.AddRight(Present->Up);//第一圈的时候，Present执行完毕之后代表的是right了. 那左子树的内容放到哪里了?哦
			Present->Data=Right;
		}	//已经写了个例子搞懂了，以后再说.
	}//然后二元表达式就被这么搞出来了.
}

//简单表达式.. 来人呐，上文法！
//	SimpleExpression = Number | bool | CallFunctionExpression
LytPtr<Expression> GetSimpleExpression(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		if (Current->Data->Type== RegularExpressionType::regularINTEGER || Current->Data->Type== RegularExpressionType::regularREAL)//Number
		{
			LytPtr<LiteralConstant> Result=new LiteralConstant;
			Result->Content=*(Current->Data);
			Current=Current->Next;
			return Result;
		}
		else if (Current->Data->Content==L"true" || Current->Data->Content==L"false")	//bool
		{
			LytPtr<LiteralConstant> Result=new LiteralConstant;
			Result->Content=*(Current->Data);
			Current=Current->Next;
			return Result;
		}
		else if (Current->Data->Content==L"not" || Current->Data->Content==L"+" || Current->Data->Content==L"-")	//UnaryExpression,这个..文法换过了.
		{
			LytPtr<UnaryExpression> Result=new UnaryExpression;
			Result->Operator=*(Current->Data);
			if (Current->Next) Current=Current->Next;
			else  throw LytPtr<Error>(new Error(*(Current->Data),Result->Operator.Content+L"(一元)表达式缺少操作数"));
			Result->Operand=GetSimpleExpression(Current);
			return Result;
		}
		else if (Current->Data->Type== RegularExpressionType::regularID)	//CallFunctionExpression
		{
			if (IsKeyWord(Current->Data->Content)) throw LytPtr<Error>(new Error(*(Current->Data),L"标识符不能为关键字"));
			if (Current->Next && Current->Next->Data->Content==L"(") return GetCallFunctionExpression(Current);
			else
			{
				LytPtr<VariableExpression> Result=new VariableExpression;
				Result->Id=*(Current->Data);
				Current=Current->Next;
				return Result;
			}
		}
		else if (Current->Data->Content==L"(")	//这个基本表达式大概是给一元操作符用的? 那个里面调用了，看起来是的！
		{
			if (Current->Next) Current=Current->Next;
			else throw LytPtr<Error>(new Error(*(Current->Data),L"基本表达式中左括号后为空"));
			LytPtr<Expression> Result=GetExpression(Current);

			Token Temp;
			if (!Current) throw LytPtr<Error>(new Error(Temp,L"基本表达式缺少右括号")); //行号
			else if (Current->Data->Content==L")")
			{
				Current=Current->Next;
				return Result;
			}
			else throw LytPtr<Error>(new Error(*(Current->Data),L"基本表达式缺少右括号"));
		}
		else throw LytPtr<Error>(new Error(*(Current->Data),L"基本表达式出错"));
	}
}

//清楚！
LytPtr<CallFunctionExpression> GetCallFunctionExpression(Node<Token*>*& Current)
{
	if (!Current) return 0;
	else
	{
		LytPtr<CallFunctionExpression> Result=new CallFunctionExpression;
		int Status=1;
		while (Current)
		{
			switch (Status)
			{
				case 1:
					if (Current->Data->Type== RegularExpressionType::regularID)
					{
						if (IsKeyWord(Current->Data->Content)) throw LytPtr<Error>(new Error(*(Current->Data), L"函数名不能为关键字"));
						Result->FunctionName=*(Current->Data);
						Status=2;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data), L"函数名后缺少实参表"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data), L"函数名出错"));
					break;
				case 2:
					if (Current->Data->Content==L"(")
					{
						Status=3;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data), L"缺少右括号"));
					}
					else throw LytPtr<Error>(new Error(*(Current->Data), L"缺少左括号"));
					break;
				case 3:
					if (Current->Data->Content==L")")
					{
						Current=Current->Next;
						return Result;
					}
					else
					{
						LytPtr<Expression> Exp=GetExpression(Current);
						Result->ArgumentList.AddLast()->Data=Exp;
						Status=4;
					}
					break;
				case 4:
					if (Current->Data->Content==L",")
					{
						Status=3;
						if (Current->Next) Current=Current->Next;
						else throw LytPtr<Error>(new Error(*(Current->Data), L"实参表中逗号后缺少实参"));
					}
					else if (Current->Data->Content==L")")
					{
						Current=Current->Next;
						return Result;
					}
					else throw LytPtr<Error>(new Error(*(Current->Data), L"实参表含有非法字符"));
					break;
			}
		}
	}
	return 0;
}