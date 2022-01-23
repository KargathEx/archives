#include "Interpret.h"

void Interpret::Print()
{
	//TheLexer.Print();
	if (TheProgram) 
	{ 
		wcout << L"/*语法树遍历结果*/" << endl 
			<< TheProgram->ToString(L"") << endl;
	}
	wcout<<endl<<L"/*中间代码*/"<<endl<<endl<<
		InstructionSetToString()<<endl;

	wcout<<L"结果："<<Result<<endl;
}

Interpret::Interpret(LytWString Content)
{
	TheLexer.RunLexer(Content);
	if (TheLexer.IsError())
	{
		TheLexer.Print();
		return;
	}

	try
	{
		TheProgram=Parse(TheLexer.Tokens.GetHead());
		TheProgram->CheckType(TheProgram);
		TheInstructionSet=TheProgram->GetInstructionSet();
		Result=TheVirtualMachine.Excuse(TheProgram, TheInstructionSet);
	}
	catch (LytPtr<Error> ErrorInfo) 
	{
		throw ErrorInfo;
	}
}

//用来把指令转成字符串打印出来，不提.
LytWString Interpret::InstructionSetToString()	
{
	LytWString Result;
	for (int i=0; i<TheInstructionSet.Size(); i++)
	{
		InstructionSet Temp=TheInstructionSet[i];
		switch(Temp.TheInstruction)
		{
			case PUSH: Result=Result+L"push\t"+Temp.Operand; break;
			case POP: Result=Result+L"pop\t"; break;
			case ADD: Result=Result+L"add\t"; break;
			case SUB: Result=Result+L"sub\t"; break;
			case MUL: Result=Result+L"mul\t"; break;
			case DIV: Result=Result+L"div\t"; break;
			case MOD: Result=Result+L"mod\t"; break;
			case NEGATIVE: Result=Result+L"negative\t"; break;
			case GREATER_THAN: Result=Result+L"greater than\t"; break;
			case LESS_THAN: Result=Result+L"less than\t"; break;
			case EQUAL_GREATER_THAN: Result=Result+L"equal greater than\t"; break;
			case EQUAL_LESS_THAN: Result=Result+L"equal less than\t"; break;
			case EQUAL: Result=Result+L"equal\t"; break;
			case NOT_EQUAL: Result=Result+L"not equal\t"; break;
			case CALL: Result=Result+L"call\t"+Temp.Operand; break;
			case RETURN_FUNCTION: Result=Result+L"return_function\t"; break;
			case RETURN_PROCEDURE: Result=Result+L"return_procedure\t"; break;
			case AND: Result=Result+L"and\t"; break;
			case OR: Result=Result+L"or\t"; break;
			case NOT: Result=Result+L"not\t"; break;
			case XOR: Result=Result+L"xor\t"; break;
			case JMP: Result=Result+L"jmp\t"+Temp.Operand; break;
			case JMP_FALSE: Result=Result+L"jmp false\t"+Temp.Operand; break;
			case JMP_TRUE: Result=Result+L"jmp true\t"+Temp.Operand; break;
			case LOAD: Result=Result+L"load\t"+Temp.Operand; break;
			case STORE: Result=Result+L"store\t"+Temp.Operand; break;
			case VAR: Result=Result+L"var\t"+Temp.Operand; break;
			case FUN: Result=Result+L"fun\t"+Temp.Operand; break;
			case POS: Result=Result+L"pos\t"+Temp.Operand; break;
		}
		Result=Result+L"\n";
	}
	return Result;
}
//只是转成了字符串吗，那虚拟机在哪?