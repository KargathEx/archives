#include "Program.h"

// InstructionSet
//仨构造函数，结果还是构造对象本身，暂时不理。
InstructionSet::InstructionSet()
{
	Position=0;
}

InstructionSet::InstructionSet(const Instruction TempInstruction, LytWString TempOperand, DataType TempOperandType, int Pos=0)
{
	TheInstruction=TempInstruction;
	Operand=TempOperand;
	OperandType=TempOperandType;
	Position=Pos;
}

InstructionSet::InstructionSet(const InstructionSet& Object)
{
	TheInstruction=Object.TheInstruction;
	Operand=Object.Operand;
	OperandType=Object.OperandType;
	Position=Object.Position;
}


//Base

Base::~Base()
{
}
//获取数据类型，派生类不需要重写
DataType Base::GetDataType(Token Content)
{
	if (Content.Content==L"boolean") return BOOLEAN;
	else if (Content.Content==L"integer") return INTEGER;
	else if (Content.Content==L"real") return REAL;
	else throw LytPtr<Error>(new Error(Content, L"该数据类型不存在"));
}

//OperatorBase : Base
//这个是干嘛的?
OperatorBase::OperatorBase()
{
}

OperatorBase::OperatorBase(Token TempContent, OperatorPrecedence TempPrecedence)
{
	Content=TempContent;
	Precedence=TempPrecedence;
}

OperatorBase::OperatorBase(const OperatorBase& Object)
{
	Content=Object.Content;
	Precedence=Object.Precedence;
}

OperatorBase::~OperatorBase()
{
}

LytWString OperatorBase::ToString(LytWString Prefix)
{
	return Content.Content;
}

//Expression : Base

Expression::~Expression()
{
}

//PrimitiveData : Expression

PrimitiveData::~PrimitiveData()
{
}

// LiteralConstant : 

LiteralConstant::~LiteralConstant()
{
}

bool LiteralConstant::IsLeftValue()
{
	return false;
}

LytWString LiteralConstant::ToString(LytWString Prefix)
{
	return Content.Content;
}

DataType LiteralConstant::GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	if (Content.Type== RegularExpressionType::regularINTEGER) return Type=INTEGER;
	else if (Content.Type== RegularExpressionType::regularID) return Type=BOOLEAN;
	else if (Content.Type== RegularExpressionType::regularREAL) return Type=REAL;
	else throw LytPtr<Error>(new Error(Content, L"常量表达式类型出错"));
}

List<InstructionSet> LiteralConstant::GetInstructionSet()
{
	List<InstructionSet> Result;
	InstructionSet TheInstructionSet(PUSH, Content.Content, Type, Content.Position);
	Result.Add(TheInstructionSet);
	return Result;
}

//VariableExpression : PrimitiveData

VariableExpression::~VariableExpression()
{
}
//直接类似特化的方式来定义是否为左值
bool VariableExpression::IsLeftValue()
{
	return true;
}

LytWString VariableExpression::ToString(LytWString Prefix)
{
	return Id.Content;
}

DataType VariableExpression::GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)	//TheProgram 这个函数好像没用啊?
{
	Node<LytPtr<Parameter>>* TheParameter=TheFunction->TheFunctionHead->ParameterList.GetHead();	//取函数形参的头
	while (TheParameter)	//哦，这个是处理形参的，那么，就是说，如果这个变量表达式是函数调用的结果，则只有形参跟结果值一样才会被调用?
		//我觉得一定是这个VariableExpression 在之前的环境里已经被定义了吧？ 没有. 暂时挂起.
	{
		if (Id.Content==TheParameter->Data->Id.Content) return GetDataType(TheParameter->Data->Type);	//当此ID跟找到的函数里的ID相同的时候，返回哪个里面存了的类型，这个Id一定是被构造的时候定义好了的哈哈哈。

		else TheParameter=TheParameter->Next;
	}
	if (TheFunction->TheVar)	//变量声明
	{
		Node<LytPtr<Variable>>* TheVariable=TheFunction->TheVar->Content.GetHead();
		while (TheVariable)
		{
			if (Id.Content==TheVariable->Data->Id.Content) return GetDataType(TheVariable->Data->Type);	//这里又是一个Id.content，到底是干嘛的?
			else TheVariable=TheVariable->Next;
		}
	}
	throw LytPtr<Error>(new Error(Id, L"该变量未被定义"));
}

List<InstructionSet> VariableExpression::GetInstructionSet()	//这个class代表的是什么样的语法片段? 应该是类似 var sss:type1 这样吧.
{
	List<InstructionSet> Result;
	InstructionSet TheInstructionSet(LOAD, Id.Content, NONE, Id.Position);	//指令集里设置为"加载内容",嗯.
	Result.Add(TheInstructionSet);
	return Result;
}

//SimpleExpression : Expression

SimpleExpression::~SimpleExpression()
{
}

bool SimpleExpression::IsLeftValue()
{
	return Content->IsLeftValue();
}

LytWString SimpleExpression::ToString(LytWString Prefix)
{
	return Content->ToString(Prefix);
}

DataType SimpleExpression::GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	return Content->GetType(TheProgram, TheFunction);
}

List<InstructionSet> SimpleExpression::GetInstructionSet()
{
	return Content->GetInstructionSet();
}

//UnaryExpression : Expression

UnaryExpression::~UnaryExpression()
{
}

bool UnaryExpression::IsLeftValue()
{
	return false;
}

LytWString UnaryExpression::ToString(LytWString Prefix)
{
	return (Operator.Content+L"("+Operand->ToString(Prefix)+L")");
}

DataType UnaryExpression::GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	DataType Temp=Operand->GetType(TheProgram, TheFunction);
	if (Operator.Content==L"+" || Operator.Content==L"-")
	{
		if (Temp==INTEGER || Temp==REAL) return Temp;
		else throw LytPtr<Error>(new Error(Operator, L"一元表达式操作数与操作符类型不匹配"));
	}
	else if (Operator.Content==L"not")
	{
		if (Temp==BOOLEAN) return Temp;
		else throw LytPtr<Error>(new Error(Operator, L"一元表达式操作数与操作符类型不匹配"));
	}
	else throw LytPtr<Error>(new Error(Operator, L"一元表达式操作符出错"));
}

List<InstructionSet> UnaryExpression::GetInstructionSet()
{
	List<InstructionSet> Result;
	Result=Operand->GetInstructionSet();
	Instruction TheInstruction;
	if (Operator.Content==L"+") return Result;
	else if (Operator.Content==L"-") TheInstruction=NEGATIVE;
	else if (Operator.Content==L"not") TheInstruction=NOT;
	Result.Add(InstructionSet(TheInstruction, L"", NONE, Operator.Position));
	return Result;
}

//BinaryExpression : Expression

BinaryExpression::~BinaryExpression()
{
}

bool BinaryExpression::IsLeftValue()
{
	return false;
}

LytWString BinaryExpression::ToString(LytWString Prefix)
{
	return (L"("+Left->ToString(Prefix)+L")"+Operator->ToString(Prefix)+L"("+Right->ToString(Prefix)+L")");
}

DataType BinaryExpression::GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	DataType LeftType=Left->GetType(TheProgram, TheFunction);
	DataType RightType=Right->GetType(TheProgram, TheFunction);
	if(LeftType!=RightType) throw LytPtr<Error>(new Error(Operator->Content, L"二元表达式两个操作数类型不匹配"));
	if (Operator->Content.Content==L"+" || Operator->Content.Content==L"-" || Operator->Content.Content==L"*")
	{
		if (LeftType==INTEGER || LeftType==REAL) return LeftType;
		else throw LytPtr<Error>(new Error(Operator->Content, L"二元表达式操作符与操作数类型不匹配"));
	}
	else if (Operator->Content.Content==L"/")
	{
		if (LeftType==REAL) return REAL;
		else throw LytPtr<Error>(new Error(Operator->Content, L"二元表达式操作符与操作数类型不匹配"));
	}
	else if (Operator->Content.Content==L"div" || Operator->Content.Content==L"mod")
	{
		if (LeftType==INTEGER) return INTEGER;
		else throw LytPtr<Error>(new Error(Operator->Content, L"二元表达式操作符与操作数类型不匹配"));
	}
	else if (Operator->Content.Content==L"<" || Operator->Content.Content==L"<=" || Operator->Content.Content==L">" || Operator->Content.Content==L">=")
	{
		if (LeftType==INTEGER || LeftType==REAL) return BOOLEAN;
		else throw LytPtr<Error>(new Error(Operator->Content, L"二元表达式操作符与操作数类型不匹配"));
	}
	else if (Operator->Content.Content==L"=" || Operator->Content.Content==L"!=") 
	{
		return BOOLEAN;
	}
	else if (Operator->Content.Content==L"and" || Operator->Content.Content==L"or" || Operator->Content.Content==L"xor")
	{
		if (LeftType==BOOLEAN) return BOOLEAN;
		else throw LytPtr<Error>(new Error(Operator->Content, L"二元表达式操作符与操作数类型不匹配"));
	}
	else throw LytPtr<Error>(new Error(Operator->Content, L"二元表达式操作数出错"));
}

List<InstructionSet> BinaryExpression::GetInstructionSet()
{
	List<InstructionSet> Result;
	Result=Left->GetInstructionSet()+Right->GetInstructionSet();
	Instruction TheInstruction;
	if (Operator->Content.Content==L"+") TheInstruction=ADD;
	else if (Operator->Content.Content==L"-") TheInstruction=SUB;
	else if (Operator->Content.Content==L"*") TheInstruction=MUL;
	else if (Operator->Content.Content==L"/") TheInstruction=REAL_DIV;
	else if (Operator->Content.Content==L"div") TheInstruction=DIV;
	else if (Operator->Content.Content==L"mod") TheInstruction=MOD;
	else if (Operator->Content.Content==L"<") TheInstruction=LESS_THAN;
	else if (Operator->Content.Content==L">") TheInstruction=GREATER_THAN;
	else if (Operator->Content.Content==L"<=") TheInstruction=EQUAL_LESS_THAN;
	else if (Operator->Content.Content==L">=") TheInstruction=EQUAL_GREATER_THAN;
	else if (Operator->Content.Content==L"=") TheInstruction=EQUAL;
	else if (Operator->Content.Content==L"!=") TheInstruction=NOT_EQUAL;
	else if (Operator->Content.Content==L"and") TheInstruction=AND;
	else if (Operator->Content.Content==L"or") TheInstruction=OR;
	else if (Operator->Content.Content==L"xor") TheInstruction=XOR;
	Result.Add(InstructionSet(TheInstruction, L"", NONE, Operator->Content.Position));
	return Result;
}

//CallFunctionExpression : Expression

CallFunctionExpression::~CallFunctionExpression()
{
}

bool CallFunctionExpression::IsLeftValue()
{
	return false; //如果返回值类型为引用就囧了
}

LytWString CallFunctionExpression::ToString(LytWString Prefix)
{
	LytWString Result=FunctionName.Content+L"(";
	Node<LytPtr<Expression>>* Argument=ArgumentList.GetHead();
	while (Argument)
	{
		Result=Result+Argument->Data->ToString(Prefix);
		Argument=Argument->Next;
		if (Argument) Result=Result+L", ";
	}
	Result=Result+L")";
	return Result;
}

DataType CallFunctionExpression::GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	Node<LytPtr<Function>>* TheFun=TheProgram->Functions.GetHead();
	while (TheFun)
	{
		if (TheFun->Data->TheFunctionHead->FunctionName.Content==FunctionName.Content)
		{
			if (TheFun->Data->TheFunctionHead->IsFunction)
			{
				DataType Temp=GetDataType(TheFun->Data->TheFunctionHead->ReturnType);
				Node<LytPtr<Expression>>* Argument=ArgumentList.GetHead();
				Node<LytPtr<Parameter>>* TheParameter=TheFun->Data->TheFunctionHead->ParameterList.GetHead();
				if (Argument && TheParameter)
				{
					while (Argument)
					{
						DataType ArgumentType=Argument->Data->GetType(TheProgram, TheFunction);
						DataType ParameterType=TheParameter->Data->GetType(TheProgram);
						if (ArgumentType!=ParameterType) throw LytPtr<Error>(new Error(FunctionName, L"函数调用表达式的参数类型与函数原型的参数类型不匹配"));
						Argument=Argument->Next;
						TheParameter=TheParameter->Next;
						if ((Argument && !TheParameter) || (!Argument && TheParameter)) throw LytPtr<Error>(new Error(FunctionName, L"函数调用表达式与函数原型参数个数不匹配"));
					}
				}
				else if (!Argument && TheParameter || Argument && !TheParameter) throw LytPtr<Error>(new Error(FunctionName, L"函数调用表达式与函数原型参数个数不匹配"));
				return Temp;
			}
			else return NONE;
		}
		TheFun=TheFun->Next;
	}
	throw LytPtr<Error>(new Error(FunctionName, L"函数调用表达式类型出错，找不到相关函数定义"));
}

List<InstructionSet> CallFunctionExpression::GetInstructionSet()
{
	List<InstructionSet> Result;
	Node<LytPtr<Expression>>* Argument=ArgumentList.Tail;
	while (Argument)
	{
		Result=Result+Argument->Data->GetInstructionSet();
		Argument=Argument->Prev;
	}
	Result.Add(InstructionSet(CALL, FunctionName.Content, NONE, FunctionName.Position));
	return Result;
}

// Variable : Base

Variable::Variable()
{
}

Variable::Variable(const Token& ObjectId, const Token& ObjectType)
{
	Id=ObjectId;
	Type=ObjectType;
}

Variable::Variable(const Variable& Object)
{
	Id=Object.Id;
	Type=Object.Type;
}

Variable::~Variable()
{
}

bool Variable::IsLeftValue()
{
	return true;
}

LytWString Variable::ToString(LytWString Prefix)
{
	return (Id.Content+L":"+Type.Content);
}

DataType Variable::GetType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	return GetDataType(Type);
}

List<InstructionSet> Variable::GetInstructionSet()
{
	List<InstructionSet> Result;
	Result.Add(InstructionSet(VAR, Id.Content, NONE, Id.Position));
	return Result;
}

//Var : Base

Var::~Var()
{
}

LytWString Var::ToString(LytWString Prefix)
{
	LytWString Result=L"var";
	Node<LytPtr<Variable>>* Temp=Content.GetHead();
	while (Temp)
	{
		Result=Result+L"\n"+Temp->Data->ToString(L"  "+Prefix);
		Temp=Temp->Next;
	}
	return Result;
}

bool Var::CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	Node<LytPtr<Variable>>* TheVariable=Content.GetHead();
	while (TheVariable)
	{
		Node<LytPtr<Variable>>* Temp=Content.GetHead();
		while (Temp!=TheVariable)
		{
			if (Temp->Data->Id.Content==TheVariable->Data->Id.Content) throw LytPtr<Error>(new Error(TheVariable->Data->Id, L"var中该变量已被定义过"));
			Temp=Temp->Next;
		}
		Node<LytPtr<Parameter>>* TheParameter=TheFunction->TheFunctionHead->ParameterList.GetHead();
		while (TheParameter)
		{
			if (TheParameter->Data->Id.Content==TheVariable->Data->Id.Content) throw LytPtr<Error>(new Error(TheVariable->Data->Id, L"var中该变量已在参数表中被定义过"));
			TheParameter=TheParameter->Next;
		}
		TheVariable->Data->GetType(TheProgram, TheFunction);
		TheVariable=TheVariable->Next;
	}
	return true;
}

List<InstructionSet> Var::GetInstructionSet()
{
	List<InstructionSet> Result;
	Node<LytPtr<Variable>>* TheVariable=Content.GetHead();
	while (TheVariable)
	{
		Result=Result+TheVariable->Data->GetInstructionSet();
		TheVariable=TheVariable->Next;
	}
	return Result;
}

// Parameter : Base

Parameter::Parameter()
{
}

Parameter::Parameter(const Token& ObjectId, const Token& ObjectType)
{
	Id=ObjectId;
	Type=ObjectType;
}

Parameter::Parameter(const Parameter& Object)
{
	Id=Object.Id;
	Type=Object.Type;
}

Parameter::~Parameter()
{
}

LytWString Parameter::ToString(LytWString Prefix)
{
	return (Id.Content+L":"+Type.Content);
}

DataType Parameter::GetType(LytPtr<Program> TheProgram)
{
	return GetDataType(Type);
}

//FunctionHead : Base

FunctionHead::~FunctionHead()
{
}

LytWString FunctionHead::ToString(LytWString Prefix)
{
	LytWString Result=FunctionType.Content+L" "+FunctionName.Content+L"(";
	Node<LytPtr<Parameter>>* Parameter=ParameterList.GetHead();
	while (Parameter)
	{
		Result=Result+Parameter->Data->ToString(Prefix);
		Parameter=Parameter->Next;
		if (Parameter) Result=Result+L"; ";
	}
	Result=Result+L")";
	if (IsFunction) Result=Result+L" : "+ReturnType.Content;
	return Result;
}

bool FunctionHead::CheckType(LytPtr<Program> TheProgram)
{
	if (IsFunction) GetDataType(ReturnType);
	Node<LytPtr<Parameter>>* TheParameter=ParameterList.GetHead();
	while (TheParameter)
	{
		Node<LytPtr<Parameter>>* Temp=ParameterList.GetHead();
		while (Temp!=TheParameter)
		{
			if (Temp->Data->Id.Content==TheParameter->Data->Id.Content) throw LytPtr<Error>(new Error(TheParameter->Data->Id, L"函数头中该参数已被声明过"));
			Temp=Temp->Next;
		}
		TheParameter->Data->GetType(TheProgram);
		TheParameter=TheParameter->Next;
	}
	return true;
}

List<InstructionSet> FunctionHead::GetInstructionSet()
{
	List<InstructionSet> Result;
	Result.Add(InstructionSet(FUN, FunctionName.Content, NONE, FunctionName.Position));
	return Result;
}

//Statement : Base

Statement::~Statement()
{
}

//SimpleStatement : Statement

SimpleStatement::~SimpleStatement()
{
}

LytWString SimpleStatement::ToString(LytWString Prefix)
{
	return Prefix+Content->ToString(Prefix);
}

bool SimpleStatement::CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	Content->GetType(TheProgram, TheFunction);
	return true;
}

List<InstructionSet> SimpleStatement::GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark)
{
	List<InstructionSet> Result=Content->GetInstructionSet();
	Result.Add(InstructionSet(POP, L"", NONE));
	return Result;
}

//AssignmentStatement : Statement

AssignmentStatement::~AssignmentStatement()
{
}

LytWString AssignmentStatement::ToString(LytWString Prefix)
{
	return Prefix+Left->ToString(Prefix)+L":="+Right->ToString(Prefix);
}

bool AssignmentStatement::CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	if (!Left->IsLeftValue()) throw LytPtr<Error>(new Error(Operator, L"赋值号的左操作数不是左值表达式"));
	if ((Left->GetType(TheProgram, TheFunction)) != (Right->GetType(TheProgram, TheFunction))) throw LytPtr<Error>(new Error(Operator, L"赋值语句中左操作数与右操作数类型不匹配"));
	return true;
}

List<InstructionSet> AssignmentStatement::GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark)
{
	List<InstructionSet> Result=Right->GetInstructionSet();
	VariableExpression* Temp=(VariableExpression*)Left.Buffer();//赋值号左值只能是变量表达式
	Result.Add(InstructionSet(STORE, Temp->Id.Content, NONE, Operator.Position));
	return Result;
}

//IfStatement : Statement

IfStatement::~IfStatement()
{
}

LytWString IfStatement::ToString(LytWString Prefix)
{
	LytWString Result=Prefix+L"if ("+Condition->ToString(Prefix)+L") then "+Then->ToString(Prefix);
	if (Else) Result=Result+L"\n"+Prefix+L"else "+Else->ToString(Prefix);
	return Result;
}

bool IfStatement::CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	if (Condition->GetType(TheProgram, TheFunction) != BOOLEAN) throw LytPtr<Error>(new Error(IfToken, L"if语句中表达式类型不是布尔型"));
	if (!(Then->CheckType(TheProgram, TheFunction))) return false;
	if (Else && !(Else->CheckType(TheProgram, TheFunction))) return false;
	return true;
}

List<InstructionSet> IfStatement::GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark)
{
	LytWString False=IntToWString(++Mark);
	LytWString End=IntToWString(++Mark);
	List<InstructionSet> Result;
	Result=Result+Condition->GetInstructionSet();
	Result.Add(InstructionSet(JMP_FALSE, False, NONE, IfToken.Position));
	Result=Result+Then->GetInstructionSet(BeginLoop, EndLoop, Mark);
	Result.Add(InstructionSet(JMP, End, NONE, IfToken.Position));
	Result.Add(InstructionSet(POS, False, NONE, IfToken.Position));
	if (Else) Result=Result+Else->GetInstructionSet(BeginLoop, EndLoop, Mark);
	Result.Add(InstructionSet(POS, End, NONE, IfToken.Position));
	return Result;
}

//WhileStatement : Statement

WhileStatement::~WhileStatement()
{
}

LytWString WhileStatement::ToString(LytWString Prefix)
{
	LytWString Result;
	if (IsWhileDo) Result=Prefix+L"while ("+Condition->ToString(Prefix)+L") do "+Do->ToString(Prefix);
	else Result=Prefix+L"do "+Do->ToString(Prefix)+L"\n"+Prefix+L"while ("+Condition->ToString(Prefix)+L")";
	return Result;
}

bool WhileStatement::CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	if ((Condition->GetType(TheProgram, TheFunction)) != BOOLEAN) throw LytPtr<Error>(new Error(WhileToken, L"while语句中表达式类型不是布尔型"));
	else if (!(Do->CheckType(TheProgram, TheFunction))) return false;
	return true;
}

List<InstructionSet> WhileStatement::GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark)
{
	LytWString Begin=IntToWString(++Mark);
	LytWString End=IntToWString(++Mark);
	List<InstructionSet> Result;
	if (IsWhileDo)
	{
		Result.Add(InstructionSet(POS, Begin, NONE, WhileToken.Position));
		Result=Result+Condition->GetInstructionSet();
		Result.Add(InstructionSet(JMP_FALSE, End, NONE, WhileToken.Position));
		Result=Result+Do->GetInstructionSet(Begin, End, Mark);
		Result.Add(InstructionSet(JMP, Begin, NONE, WhileToken.Position));
		Result.Add(InstructionSet(POS, End, NONE, WhileToken.Position));
	}
	else
	{
		Result.Add(InstructionSet(POS, Begin, NONE, DoToken.Position));
		Result=Result+Do->GetInstructionSet(Begin, End, Mark);
		Result=Result+Condition->GetInstructionSet();
		Result.Add(InstructionSet(JMP_FALSE, End, NONE, DoToken.Position));
		Result.Add(InstructionSet(JMP, Begin, NONE, DoToken.Position));
		Result.Add(InstructionSet(POS, End, NONE, DoToken.Position));
	}
	return Result;
}

//ControlStatement : Statement

ControlStatement::~ControlStatement()
{
}

LytWString ControlStatement::ToString(LytWString Prefix)
{
	return Prefix+Content.Content;
}

bool ControlStatement::CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	return true;
}

List<InstructionSet> ControlStatement::GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark)
{
	List<InstructionSet> Result;
	if (BeginLoop!=L"")
	{
		if (Content.Content==L"break") Result.Add(InstructionSet(JMP, EndLoop, NONE, Content.Position));
		else if (Content.Content==L"continue") Result.Add(InstructionSet(JMP, BeginLoop, NONE, Content.Position));
	}
	else throw LytPtr<Error>(new Error(Content, L"该控制语句没有出现在循环中"));
	return Result;
}

//ReturnStatement : Statement

ReturnStatement::~ReturnStatement()
{
}

LytWString ReturnStatement::ToString(LytWString Prefix)
{
	LytWString Result;
	if (Return) return Prefix+L"return ("+Return->ToString(Prefix)+L")";
	else return Prefix+L"return";
}

bool ReturnStatement::CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	if (TheFunction->TheFunctionHead->IsFunction)
	{
		if (!Return) throw LytPtr<Error>(new Error(ReturnToken, L"return语句缺少返回值"));
		else
		{
			if ((GetDataType(TheFunction->TheFunctionHead->ReturnType)) != (Return->GetType(TheProgram, TheFunction))) throw LytPtr<Error>(new Error(ReturnToken, L"return语句中表达式类型与函数返回值类型不匹配"));
			else return true;
		}
	}
	else
	{
		if (Return) throw LytPtr<Error>(new Error(ReturnToken, L"return语句不需添加返回值"));
		else return true;
	}
}

List<InstructionSet> ReturnStatement::GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark)
{
	List<InstructionSet> Result;
	if (Return) 
	{
		Result=Result+Return->GetInstructionSet();
		Result.Add(InstructionSet(RETURN_FUNCTION, L"", NONE, ReturnToken.Position));
	}
	else Result.Add(InstructionSet(RETURN_PROCEDURE, L"", NONE, ReturnToken.Position));
	return Result;
}

//Block : Statement

Block::~Block()
{
}

LytWString Block::ToString(LytWString Prefix)
{
	LytWString Result=L"\n"+Prefix+L"begin";
	Node<LytPtr<Statement>>* Temp=StatementList.GetHead();
	while (Temp)
	{
		Result=Result+L"\n"+Temp->Data->ToString(Prefix+L"  ")+L";";
		Temp=Temp->Next;
	}
	Result=Result+L"\n"+Prefix+L"end";
	return Result;
}

bool Block::CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	Node<LytPtr<Statement>>* TheStatement=StatementList.GetHead();
	while (TheStatement)
	{
		if (!TheStatement->Data->CheckType(TheProgram, TheFunction)) return false;
		TheStatement=TheStatement->Next;
	}
	return true;
}

List<InstructionSet> Block::GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark)
{
	List<InstructionSet> Result;
	Node<LytPtr<Statement>>* Temp=StatementList.GetHead();
	while (Temp)
	{
		Result=Result+Temp->Data->GetInstructionSet(BeginLoop, EndLoop, Mark);
		Temp=Temp->Next;
	}
	return Result;
}

//FunctionDeclaration

FunctionDeclaration::FunctionDeclaration(LytPtr<FunctionHead> Temp)
{
	Content=Temp;
}

FunctionDeclaration::~FunctionDeclaration()
{
}

LytWString FunctionDeclaration::ToString(LytWString Prefix)
{
	return Prefix+Content->ToString(Prefix)+L";";
}

bool FunctionDeclaration::CheckType(LytPtr<Program> TheProgram)
{
	if(!Content->CheckType(TheProgram)) return false;
	else return true;
}

//Function : Base

Function::~Function()
{
}

LytWString Function::ToString(LytWString Prefix)
{
	LytWString Result=TheFunctionHead->ToString(Prefix);
	if (TheVar) Result=Result+L"\n"+TheVar->ToString(Prefix+L"  ");
	Result=Result+Body->ToString(Prefix);
	return Result;
}

bool Function::CheckType(LytPtr<Program> TheProgram, LytPtr<Function> TheFunction)
{
	if (!TheFunctionHead->CheckType(TheProgram)) return false;
	if (TheVar && !TheVar->CheckType(TheProgram, TheFunction)) return false;
	if (!Body->CheckType(TheProgram, TheFunction)) return false;
	return true;
}

List<InstructionSet> Function::GetInstructionSet(LytWString BeginLoop, LytWString EndLoop, int& Mark)
{
	List<InstructionSet> Result;
	Result=Result+TheFunctionHead->GetInstructionSet();
	if (TheVar) Result=Result+TheVar->GetInstructionSet();
	Result=Result+Body->GetInstructionSet(BeginLoop, EndLoop, Mark);
	if (TheFunctionHead->IsFunction)
	{
		DataType Temp=GetDataType(TheFunctionHead->ReturnType);
		if (Temp==INTEGER) Result.Add(InstructionSet(PUSH, L"0", INTEGER, TheFunctionHead->FunctionName.Position));
		else if (Temp==REAL) Result.Add(InstructionSet(PUSH, L"0.0", REAL, TheFunctionHead->FunctionName.Position));
		else if (Temp==BOOLEAN) Result.Add(InstructionSet(PUSH, L"false", BOOLEAN, TheFunctionHead->FunctionName.Position));
		Result.Add(InstructionSet(RETURN_FUNCTION, L"", NONE, TheFunctionHead->FunctionName.Position));
	}
	else Result.Add(InstructionSet(RETURN_PROCEDURE, L"", NONE, TheFunctionHead->FunctionName.Position));
	return Result;
}

//Program : Base

Program::~Program()
{
}

LytWString Program::ToString(LytWString Prefix)
{
	LytWString Result;
	Node<LytPtr<FunctionDeclaration>>* TheFunctionDeclaration=FunctionDeclarations.GetHead();
	while (TheFunctionDeclaration)
	{
		Result=Result+TheFunctionDeclaration->Data->ToString(Prefix);
		TheFunctionDeclaration=TheFunctionDeclaration->Next;
		if (TheFunctionDeclaration) Result=Result+L"\n";
	}

	if (FunctionDeclarations.GetHead()) Result=Result+L"\n\n";

	Node<LytPtr<Function>>* TheFunction=Functions.GetHead();
	while (TheFunction)
	{
		Result=Result+TheFunction->Data->ToString(Prefix);
		TheFunction=TheFunction->Next;
		if (TheFunction) Result=Result+L"\n\n";
	}
	return Result;
}

bool Program::CheckType(LytPtr<Program> TheProgram)
{
	bool MainExist=false;
	Node<LytPtr<Function>>* Fun=Functions.GetHead();
	while (Fun)
	{
		if (Fun->Data->TheFunctionHead->FunctionName.Content==L"main") if (!MainExist) MainExist=true;
		Node<LytPtr<Function>>* Temp=Functions.GetHead();
		while (Temp!=Fun)
		{
			if (Temp->Data->TheFunctionHead->FunctionName.Content==Fun->Data->TheFunctionHead->FunctionName.Content) throw LytPtr<Error>(new Error(Fun->Data->TheFunctionHead->FunctionName, L"程序出错 => 该函数已被定义过"));
			Temp=Temp->Next;
		}
		if (!Fun->Data->CheckType(TheProgram, Fun->Data)) return false;
		Fun=Fun->Next;
	}
	Node<LytPtr<FunctionDeclaration>>* FunDeclare=FunctionDeclarations.GetHead();
	while (FunDeclare)
	{
		if (!FunDeclare->Data->CheckType(TheProgram)) return false;
		FunDeclare=FunDeclare->Next;
	}
	Token Temp;
	if (MainExist) return true;
	else throw LytPtr<Error>(new Error(Temp, L"找不到main函数"));
}

List<InstructionSet> Program::GetInstructionSet()
{
	LytWString BeginLoop;
	LytWString EndLoop;
	int Mark=0;
	List<InstructionSet> Result;
	Result.Add(InstructionSet(CALL, L"main", NONE));
	Node<LytPtr<Function>>* Temp=Functions.GetHead();
	while (Temp)
	{
		Result=Result+Temp->Data->GetInstructionSet(BeginLoop, EndLoop, Mark);
		Temp=Temp->Next;
	}
	return Result;
}