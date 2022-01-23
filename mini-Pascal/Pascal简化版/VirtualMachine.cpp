#include "VirtualMachine.h"

VirtualMachine::StackData::StackData()
{
	Type=NONE;
	Value.IntegerData=0;
	Value.RealData=0;
	Value.BooleanData=0;
}

VirtualMachine::StackData::StackData(DataType TheType, LytWString Data)
{
	Type=TheType;
	if (Type==BOOLEAN)
	{
		if (Data==L"true") Value.BooleanData=true;
		else if (Data==L"false") Value.BooleanData=false;
		else throw L"虚拟机运行出错";
	}
	else if (Type==INTEGER) Value.IntegerData=Data.ToInt();
	else if (Type==REAL) Value.RealData=Data.ToDouble();
	else throw L"虚拟机运行出错";
}

VirtualMachine::StackData::StackData(const StackData& Object)
{
	Type=Object.Type;
	if (Type==BOOLEAN) Value.BooleanData=Object.Value.BooleanData;
	else if (Type==REAL) Value.RealData=Object.Value.RealData;
	else Value.IntegerData=Object.Value.IntegerData;
}

VirtualMachine::VirtualMachine():Position(9999) //最多只能有9999个标记
{
}

LytWString VirtualMachine::Excuse(LytPtr<Program> TheProgram, List<InstructionSet> TheInstructionSet)
{
	Initialize(TheProgram, TheInstructionSet);
	int i=0;
	while (i<=TheInstructionSet.Size()-1)
	{
		InstructionSet Temp=TheInstructionSet[i];
		switch(TheInstructionSet[i].TheInstruction)
		{
			case PUSH: Push(Temp); i++; break;
			case POP: Pop(); i++; break;
			case ADD: Add(Temp); i++; break;
			case SUB: Sub(Temp); i++; break;
			case MUL: Mul(Temp); i++; break;
			case REAL_DIV: RealDiv(Temp); i++; break;
			case DIV: Div(Temp); i++; break;
			case MOD: Mod(Temp); i++; break;
			case NEGATIVE: Negative(Temp); i++; break;
			case GREATER_THAN: GreaterThan(Temp); i++; break;
			case LESS_THAN: LessThan(Temp); i++; break;
			case EQUAL_GREATER_THAN: EqualGreaterThan(Temp); i++; break;
			case EQUAL_LESS_THAN: EqualLessThan(Temp); i++; break;
			case EQUAL: Equal(Temp); i++; break;
			case NOT_EQUAL: NotEqual(Temp); i++; break;
			case AND: And(Temp); i++; break;
			case OR: Or(Temp); i++; break;
			case XOR: Xor(Temp); i++; break;
			case NOT: Not(Temp); i++; break;
			case JMP: i=Jmp(Temp); break;
			case JMP_TRUE:
			{
				int MoveStep=JmpTrue(Temp);
				if (MoveStep!=-1) i=MoveStep;
				else i++;
				break;
			}
			case JMP_FALSE:
			{
				int MoveStep=JmpFalse(Temp);
				if (MoveStep!=-1) i=MoveStep;
				else i++;
				break;
			}
			case LOAD: Load(Temp); i++; break;
			case STORE: Store(Temp); i++; break;
			case CALL:
			{
				if (Temp.Operand==L"main") i=Call(Temp, -1);
				else i=Call(Temp, i+1);
				break;
			}
			case RETURN_FUNCTION:
			{
				i=Return_Function(Temp);
				if (i==-1) return GetResult(TheStack.Pop());
				break;
			}
			case RETURN_PROCEDURE:
			{
				i=Return_Procedure(Temp);
				if (i==-1) return L"";
				break;
			}
			case VAR: Var(Temp); i++; break;
			default: i++;
		}
	}
	if (TheStack.Data.Size()) return GetResult(Pop());
	else return L"";
}

void VirtualMachine::Initialize(LytPtr<Program> TheProgram, List<InstructionSet> TheInstructionSet)
{
	BaseIndex=-1;
	TheSymbolList.Initialize(TheProgram);
	for (int i=0; i<=TheInstructionSet.Size()-1; i++)
	{
		if (TheInstructionSet[i].TheInstruction==POS)
		{
			int Index=TheInstructionSet[i].Operand.ToInt();
			Position[Index]=i;
		}
		else if (TheInstructionSet[i].TheInstruction==FUN)
		{
			int FunctionIndex=TheSymbolList.GetFunctionIndex(TheInstructionSet[i].Operand);
			TheSymbolList.Data[FunctionIndex]->FunctionIndex=i;
		}
	}
}

LytWString VirtualMachine::GetResult(VirtualMachine::StackData Data)
{
	if (Data.Type==BOOLEAN)
	{
		if (Data.Value.BooleanData) return L"true";
		else return L"false";
	}
	else if (Data.Type==INTEGER) return IntToWString(Data.Value.IntegerData);
	else if (Data.Type==REAL) return DoubleToWString(Data.Value.RealData);
	else throw L"虚拟机运行出错";
}

void VirtualMachine::Push(InstructionSet TheInstructionSet)
{
	TheStack.Push(StackData(TheInstructionSet.OperandType, TheInstructionSet.Operand));
}

void VirtualMachine::Push(StackData Object)
{
	TheStack.Push(Object);
}

VirtualMachine::StackData VirtualMachine::Pop()
{
	return TheStack.Pop();
}

void VirtualMachine::Add(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Right=Pop().Value.IntegerData;
		int Left=Pop().Value.IntegerData;
		StackData Result;
		Result.Value.IntegerData=Left+Right;
		Result.Type=TheDataType;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Right=Pop().Value.RealData;
		double Left=Pop().Value.RealData;
		StackData Result;
		Result.Value.RealData=Left+Right;
		Result.Type=TheDataType;
		Push(Result);
	}
}

void VirtualMachine::Sub(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Right=Pop().Value.IntegerData;
		int Left=Pop().Value.IntegerData;
		StackData Result;
		Result.Value.IntegerData=Left-Right;
		Result.Type=TheDataType;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Right=Pop().Value.RealData;
		double Left=Pop().Value.RealData;
		StackData Result;
		Result.Value.RealData=Left-Right;
		Result.Type=TheDataType;
		Push(Result);
	}
}

void VirtualMachine::Mul(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Right=Pop().Value.IntegerData;
		int Left=Pop().Value.IntegerData;
		StackData Result;
		Result.Value.IntegerData=Left*Right;
		Result.Type=TheDataType;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Right=Pop().Value.RealData;
		double Left=Pop().Value.RealData;
		StackData Result;
		Result.Value.RealData=Left*Right;
		Result.Type=TheDataType;
		Push(Result);
	}
}

void VirtualMachine::RealDiv(InstructionSet TheInstructionSet)
{
	double Right=Pop().Value.RealData;
	if (!Right) throw LytPtr<Error>(new Error(Token(L"/", RegularExpressionType::regularOPERATOR, TheInstructionSet.Position), L"/右操作数不能为0"));
	double Left=Pop().Value.RealData;
	StackData Result;
	Result.Value.RealData=(double)Left/Right;
	Result.Type=REAL;
	Push(Result);
}

void VirtualMachine::Div(InstructionSet TheInstructionSet)
{
	int Right=Pop().Value.IntegerData;
	if (!Right) throw LytPtr<Error>(new Error(Token(L"div", RegularExpressionType::regularID, TheInstructionSet.Position), L"div右操作数不能为0"));
	int Left=Pop().Value.IntegerData;
	StackData Result;
	Result.Value.IntegerData=Left/Right;
	Result.Type=INTEGER;
	Push(Result);
}

void VirtualMachine::Mod(InstructionSet TheInstructionSet)
{
	int Right=Pop().Value.IntegerData;
	int Left=Pop().Value.IntegerData;
	StackData Result;
	Result.Value.IntegerData=Left%Right;
	Result.Type=INTEGER;
	Push(Result);
}

void VirtualMachine::Negative(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Temp=Pop().Value.IntegerData;
		StackData Result;
		Result.Value.IntegerData=-Temp;
		Result.Type=TheDataType;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Temp=Pop().Value.RealData;
		StackData Result;
		Result.Value.RealData=-Temp;
		Result.Type=TheDataType;
		Push(Result);
	}
}

void VirtualMachine::GreaterThan(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Right=Pop().Value.IntegerData;
		int Left=Pop().Value.IntegerData;
		StackData Result;
		if (Left>Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Right=Pop().Value.RealData;
		double Left=Pop().Value.RealData;
		StackData Result;
		if (Left>Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
}

void VirtualMachine::LessThan(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Right=Pop().Value.IntegerData;
		int Left=Pop().Value.IntegerData;
		StackData Result;
		if (Left<Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Right=Pop().Value.RealData;
		double Left=Pop().Value.RealData;
		StackData Result;
		if (Left<Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
}

void VirtualMachine::EqualGreaterThan(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Right=Pop().Value.IntegerData;
		int Left=Pop().Value.IntegerData;
		StackData Result;
		if (Left>=Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Right=Pop().Value.RealData;
		double Left=Pop().Value.RealData;
		StackData Result;
		if (Left>=Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
}

void VirtualMachine::EqualLessThan(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Right=Pop().Value.IntegerData;
		int Left=Pop().Value.IntegerData;
		StackData Result;
		if (Left<=Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Right=Pop().Value.RealData;
		double Left=Pop().Value.RealData;
		StackData Result;
		if (Left<=Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
}

void VirtualMachine::Equal(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Right=Pop().Value.IntegerData;
		int Left=Pop().Value.IntegerData;
		StackData Result;
		if (Left==Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Right=Pop().Value.RealData;
		double Left=Pop().Value.RealData;
		StackData Result;
		if (Left==Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
	else if (TheDataType==BOOLEAN)
	{
		bool Right=Pop().Value.BooleanData;
		bool Left=Pop().Value.BooleanData;
		StackData Result;
		if (Left==Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
}

void VirtualMachine::NotEqual(InstructionSet TheInstructionSet)
{
	DataType TheDataType=TheStack.Data[TheStack.Data.Size()-1].Type;
	if (TheDataType==INTEGER)
	{
		int Right=Pop().Value.IntegerData;
		int Left=Pop().Value.IntegerData;
		StackData Result;
		if (Left!=Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
	else if (TheDataType==REAL)
	{
		double Right=Pop().Value.RealData;
		double Left=Pop().Value.RealData;
		StackData Result;
		if (Left!=Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
	else if (TheDataType==BOOLEAN)
	{
		bool Right=Pop().Value.BooleanData;
		bool Left=Pop().Value.BooleanData;
		StackData Result;
		if (Left!=Right) Result.Value.BooleanData=true;
		else Result.Value.BooleanData=false;
		Result.Type=BOOLEAN;
		Push(Result);
	}
}

void VirtualMachine::And(InstructionSet TheInstructionSet)
{
	bool Right=Pop().Value.BooleanData;
	bool Left=Pop().Value.BooleanData;
	StackData Result;
	if (Left&&Right) Result.Value.BooleanData=true;
	else Result.Value.BooleanData=false;
	Result.Type=BOOLEAN;
	Push(Result);
}

void VirtualMachine::Or(InstructionSet TheInstructionSet)
{
	bool Right=Pop().Value.BooleanData;
	bool Left=Pop().Value.BooleanData;
	StackData Result;
	if (Left||Right) Result.Value.BooleanData=true;
	else Result.Value.BooleanData=false;
	Result.Type=BOOLEAN;
	Push(Result);
}

void VirtualMachine::Not(InstructionSet TheInstructionSet)
{
	bool Temp=Pop().Value.BooleanData;
	StackData Result;
	if (!Temp) Result.Value.BooleanData=true;
	else Result.Value.BooleanData=false;
	Result.Type=BOOLEAN;
	Push(Result);
}

void VirtualMachine::Xor(InstructionSet TheInstructionSet)
{
	bool Right=Pop().Value.BooleanData;
	bool Left=Pop().Value.BooleanData;
	StackData Result;
	if (Left^Right) Result.Value.BooleanData=true;
	else Result.Value.BooleanData=false;
	Result.Type=BOOLEAN;
	Push(Result);
}

void VirtualMachine::Var(InstructionSet TheInstructionSet)
{
	StackData Result;
	Push(Result);
}

void VirtualMachine::Load(InstructionSet TheInstructionSet)
{
	int FunctionIndex=TheStack.Data[BaseIndex-1].Value.IntegerData;
	int Index=TheSymbolList.Data[FunctionIndex]->GetMoveStep(TheInstructionSet.Operand)+BaseIndex;
	Push(TheStack.Data[Index]);
}

void VirtualMachine::Store(InstructionSet TheInstructionSet)
{
	int FunctionIndex=TheStack.Data[BaseIndex-1].Value.IntegerData;
	int Index=TheSymbolList.Data[FunctionIndex]->GetMoveStep(TheInstructionSet.Operand)+BaseIndex;
	TheStack.Data[Index]=Pop();
}

int VirtualMachine::Jmp(InstructionSet TheInstructionSet)
{
	int Index=TheInstructionSet.Operand.ToInt();
	return Position[Index];
}

int VirtualMachine::JmpTrue(InstructionSet TheInstructionSet)
{
	bool Temp=Pop().Value.BooleanData;
	if (Temp) return Jmp(TheInstructionSet);
	else return -1;
}

int VirtualMachine::JmpFalse(InstructionSet TheInstructionSet)
{
	bool Temp=Pop().Value.BooleanData;
	if (!Temp) return Jmp(TheInstructionSet);
	else return -1;
}

int VirtualMachine::Call(InstructionSet TheInstructionSet, int Return)
{
	int FunctionIndex=TheSymbolList.GetFunctionIndex(TheInstructionSet.Operand);
	StackData Temp;
	Temp.Type=INTEGER;
	Temp.Value.IntegerData=Return;
	Push(Temp);
	Temp.Value.IntegerData=FunctionIndex;
	Push(Temp);
	Temp.Value.IntegerData=BaseIndex;
	Push(Temp);
	BaseIndex=TheStack.Data.Size()-1;
	return TheSymbolList.Data[FunctionIndex]->FunctionIndex;
}

int VirtualMachine::Return_Function(InstructionSet TheInstructionSet)
{
	StackData Result=Pop();
	int FunctionIndex=TheStack.Data[BaseIndex-1].Value.IntegerData;
	int ReturnIndex=TheStack.Data[BaseIndex-2].Value.IntegerData;
	int OldBase=BaseIndex;
	BaseIndex=TheStack.Data[BaseIndex].Value.IntegerData;
	if (BaseIndex==-1) for (int i=TheStack.Data.Size()-1; i>=0; i--) Pop();
	else for (int i=TheStack.Data.Size()-1; i>=OldBase-2-TheSymbolList.Data[FunctionIndex]->ParameterList.Size(); i--) Pop();
	Push(Result);
	return ReturnIndex;
}

int VirtualMachine::Return_Procedure(InstructionSet TheInstructionSet)
{
	int FunctionIndex=TheStack.Data[BaseIndex-1].Value.IntegerData;
	int ReturnIndex=TheStack.Data[BaseIndex-2].Value.IntegerData;
	BaseIndex=TheStack.Data[BaseIndex].Value.IntegerData;
	if (BaseIndex==-1) for (int i=TheStack.Data.Size()-1; i>=0; i--) Pop();
	else for (int i=TheStack.Data.Size()-1; i>=BaseIndex-2-TheSymbolList.Data[FunctionIndex]->ParameterList.Size(); i--) Pop();
	return ReturnIndex;
}