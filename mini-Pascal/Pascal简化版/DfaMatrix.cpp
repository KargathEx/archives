#include "DfaMatrix.h"

DfaMatrix::DfaMatrix()
{
	StatusSize=0;
	CharLinkSize=0;
	Final=0;
}

DfaMatrix::DfaMatrix(const int TempStatusSize,const int TempCharLinkSize)
{
	New(TempStatusSize, TempCharLinkSize);
}

void DfaMatrix::New()
{
	Data.New(StatusSize+1, CharLinkSize+1);
	Data.Initialize(Epsilon);

	Final=new bool[StatusSize+1];
	for (int i=0; i<=StatusSize; ++i)
		Final[i]=false;
}

void DfaMatrix::New(const int TempStatusSize,const int TempCharLinkSize)
{
	StatusSize=TempStatusSize;
	CharLinkSize=TempCharLinkSize;
	New();
}

void DfaMatrix::GetDfaMatrix()
{
	Node<DfaEdgeBase*>* Temp_AllEdge=DfaEdgeBase::AllEdge.GetHead();
	while (Temp_AllEdge)
	{
		//SetFinal
		if (Temp_AllEdge->Data->From->FinalStatus)
			Final[Temp_AllEdge->Data->From->Data.StatusNumber]=true;
		if (Temp_AllEdge->Data->To->FinalStatus)
			Final[Temp_AllEdge->Data->To->Data.StatusNumber]=true;

		//SetData
		int FromStatusNumber=Temp_AllEdge->Data->From->Data.StatusNumber;
		int ToStatusNumber=Temp_AllEdge->Data->To->Data.StatusNumber;
		Node<int>* TempNode_MatchContent=Temp_AllEdge->Data->Data.Data.GetHead();
		while (TempNode_MatchContent)
		{
			Data(FromStatusNumber, TempNode_MatchContent->Data)=ToStatusNumber;
			TempNode_MatchContent=TempNode_MatchContent->Next;
		}
		Temp_AllEdge=Temp_AllEdge->Next;
	}
}

void DfaMatrix::Print()
{
	cout<<"DfaMatrix:"<<endl;
	for (int i=0; i<=StatusSize; ++i)
	{
		cout<<"DfaStatus "<<i;
		if (Final[i])
			cout<<" (final)";
		cout<<" : ";
		for (int j=0; j<=CharLinkSize; ++j)
			if (Data(i,j)!=0)
				cout<<j<<" -> "<<Data(i,j)<<" | ";
		cout<<endl;
	}
}

DfaMatrix::~DfaMatrix()
{
	Data.Release();
	delete[] Final;
}