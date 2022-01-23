#include "Dfa.h"

Dfa::Dfa()
{
	StatusSize=0;
}

void Dfa::SetStatusNumber()
{
	StatusSize=0;
	Node<DfaStatusBase*>* Temp=DfaStatusBase::AllStatus.GetHead();
	while (Temp)
	{
		Temp->Data->Data.StatusNumber=++StatusSize;
		Temp=Temp->Next;
	}
}

DfaStatusBase* Dfa::NfaStatusBaseToDfa(NfaStatusBase* Object)
{
	DfaStatus NewDfaStatus;
	NewDfaStatus.New();
	NewDfaStatus.Data->Data.Add(Object->Data.StatusNumber);
	if (Object->FinalStatus)
		NewDfaStatus.Data->FinalStatus=true;
	return NewDfaStatus.Data;
}

void Dfa::FindCorrespondingNfaStatus(Set<int>& StatusNumber, Nfa& NfaGraph, Link<NfaStatusBase*>& Result)
{
	Node<int>* Temp_StatusNumber=StatusNumber.GetHead();
	while (Temp_StatusNumber)
	{
		Node<NfaStatusBase*>* Temp_AvailableStatus=NfaGraph.AvailableStatus.GetHead();
		while (Temp_AvailableStatus)
		{
			if (Temp_AvailableStatus->Data->Data.StatusNumber==Temp_StatusNumber->Data)
			{
				Result.AddLast()->Data=Temp_AvailableStatus->Data;
				break;
			}
			Temp_AvailableStatus=Temp_AvailableStatus->Next;
		}
		Temp_StatusNumber=Temp_StatusNumber->Next;
	}
}

DfaStatusBase* Dfa::IsExisentStatus(Set<int>& StatusNumber, bool Final) // 如果状态不存在就加进UnsettledStatus、ExisentStatus
{
	Node<DfaStatusBase*>* Temp_ExisentStatus=ExisentStatus.GetHead();
	while (Temp_ExisentStatus)
	{
		if (Temp_ExisentStatus->Data->Data.NfaStatusNumberSet==StatusNumber)
		{
			return Temp_ExisentStatus->Data;
		}
		Temp_ExisentStatus=Temp_ExisentStatus->Next;
	}
	
	DfaStatus NewDfaStatus;
	NewDfaStatus.New();
	NewDfaStatus.Data->Data.NfaStatusNumberSet.Copy(StatusNumber);
	NewDfaStatus.Data->FinalStatus=Final;
	UnsettledStatus.Add(NewDfaStatus.Data);
	ExisentStatus.Add(NewDfaStatus.Data);
	return NewDfaStatus.Data;
}

void Dfa::GetDfa(Nfa& NfaGraph)
{
	DfaStatusBase* Temp_DfaStatusBase;
	Temp_DfaStatusBase=NfaStatusBaseToDfa(NfaGraph.Start.Data);
	Start.Data=Temp_DfaStatusBase;

	UnsettledStatus.Add(Temp_DfaStatusBase);
	ExisentStatus.Add(Temp_DfaStatusBase);

	Node<DfaStatusBase*>* Temp_UnsettledStatus=UnsettledStatus.GetHead();
	while (Temp_UnsettledStatus)
	{
		//求此状态出发的边上所有匹配的字符集CharSet
		Link<NfaStatusBase*> CorrespondingNfaStatus;
		FindCorrespondingNfaStatus(Temp_UnsettledStatus->Data->Data.NfaStatusNumberSet, NfaGraph, CorrespondingNfaStatus);
		Node<NfaStatusBase*>* Temp_CorrespondingNfaStatus=CorrespondingNfaStatus.GetHead();
		Set<int> CharSet;
		while (Temp_CorrespondingNfaStatus)
		{
			Node<NfaEdgeBase*>* TempEdge=Temp_CorrespondingNfaStatus->Data->OutEdges.GetHead();
			while (TempEdge)
			{
				CharSet.Add(TempEdge->Data->Data.Data);
				TempEdge=TempEdge->Next;
			}
			Temp_CorrespondingNfaStatus=Temp_CorrespondingNfaStatus->Next;
		}

		// 遍历CharSet，求得每个CharSet所到达的状态编码EndStatusNumber
		Node<int>* Temp_CharSet=CharSet.GetHead();
		while (Temp_CharSet)
		{
			Temp_CorrespondingNfaStatus=CorrespondingNfaStatus.GetHead();
			Set<int> EndStatusNumber;
			bool TempFinal=false;
			while (Temp_CorrespondingNfaStatus)
			{
				Node<NfaEdgeBase*>* TempEdge=Temp_CorrespondingNfaStatus->Data->OutEdges.GetHead();
				while (TempEdge)
				{
					if (TempEdge->Data->Data.Data.Exists(Temp_CharSet->Data))
					{
						EndStatusNumber.Add(TempEdge->Data->To->Data.StatusNumber);
						if (TempEdge->Data->To->FinalStatus)
							TempFinal=true;
					}
					TempEdge=TempEdge->Next;
				}
				Temp_CorrespondingNfaStatus=Temp_CorrespondingNfaStatus->Next;
			}

			// 判断状态是否存在于Dfa中，并添加必要的边
			Temp_DfaStatusBase=IsExisentStatus(EndStatusNumber, TempFinal);
			DfaEdge NewDfaEdge;
			NewDfaEdge.Connect(Temp_UnsettledStatus->Data, Temp_DfaStatusBase);
			NewDfaEdge.Data->Data.Add(Temp_CharSet->Data);
			Temp_CharSet=Temp_CharSet->Next;
		}
		CorrespondingNfaStatus.Zero();
		Temp_UnsettledStatus=Temp_UnsettledStatus->Next;
	}
	SetStatusNumber();
}

void Dfa::Print()
{
	cout<<"DfaStatus is started from "<<Start.Data->Data.StatusNumber<<endl;
	Node<DfaStatusBase*>* Temp_AllStatus=DfaStatusBase::AllStatus.GetHead();
	while (Temp_AllStatus)
	{
		cout<<"DfaStatus "<<Temp_AllStatus->Data->Data.StatusNumber;
		if (Temp_AllStatus->Data->FinalStatus)
			cout<<" (final)";
		cout<<" : ";

		Node<DfaEdgeBase*>* TempEdge=Temp_AllStatus->Data->OutEdges.GetHead();
		while (TempEdge)
		{
			cout<<TempEdge->Data->Data.Data.GetHead()->Data<<" -> "<<TempEdge->Data->To->Data.StatusNumber<<" | ";
			TempEdge=TempEdge->Next;
		}
		cout<<endl;
		Temp_AllStatus=Temp_AllStatus->Next;
	}
}