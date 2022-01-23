#include "SymbolList.h"

//Symbol

Symbol::Symbol()
{
}

Symbol::Symbol(LytWString TempId, DataType TempType)
{
	Id=TempId;
	Type=TempType;
}

Symbol::Symbol(const Symbol& Object)
{
	Id=Object.Id;
	Type=Object.Type;
}

//FunctionSymbolList

void FunctionSymbolList::Initialize(LytPtr<Function> TheFunction)
{
	FunctionName=TheFunction->TheFunctionHead->FunctionName.Content;
	Node<LytPtr<Parameter>>* TheParameter=TheFunction->TheFunctionHead->ParameterList.GetHead();
	while (TheParameter)
	{
		ParameterList.Add(new Symbol(TheParameter->Data->Id.Content, TheParameter->Data->TheDataType));
		TheParameter=TheParameter->Next;
	}
	if (TheFunction->TheVar)
	{
		Node<LytPtr<Variable>>* TheVariable=TheFunction->TheVar->Content.GetHead();
		while (TheVariable)
		{
			VariableList.Add(new Symbol(TheVariable->Data->Id.Content, TheVariable->Data->TheDataType));
			TheVariable=TheVariable->Next;
		}
	}
}

const int FunctionSymbolList::GetMoveStep(LytWString TheId)const
{
	for (int i=0; i<ParameterList.Size(); i++)
		if (ParameterList[i]->Id==TheId) return -(i+3);
	for (int i=0; i<VariableList.Size(); i++)
		if (VariableList[i]->Id==TheId) return i+1;
	return 0;
}

//SymbolList

void SymbolList::Initialize(LytPtr<Program> TheProgram)
{
	Node<LytPtr<Function>>* TheFunction=TheProgram->Functions.GetHead();
	while (TheFunction)
	{
		LytPtr<FunctionSymbolList> Temp=new FunctionSymbolList;
		Temp->Initialize(TheFunction->Data);
		Data.Add(Temp);
		TheFunction=TheFunction->Next;
	}
}

const int SymbolList::GetFunctionIndex(LytWString TheFunctionName)const
{
	for (int i=0; i<Data.Size(); i++)
		if (Data[i]->FunctionName==TheFunctionName) return i;
	return -1;
}
