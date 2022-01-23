#include "StatusEdge.h"

typedef StatusBase<NfaStatusNumber, EdgeMatchContent> NfaStatusBase;
typedef NfaStatusBase::EdgeBase NfaEdgeBase;
typedef Status<NfaStatusBase, NfaEdgeBase> NfaStatus;
typedef NfaStatus::Edge NfaEdge;

Link<NfaStatusBase*> NfaStatusBase::AllStatus;
Link<NfaEdgeBase*> NfaEdgeBase::AllEdge;

typedef StatusBase<DfaStatusNumber, EdgeMatchContent> DfaStatusBase;
typedef DfaStatusBase::EdgeBase DfaEdgeBase;
typedef Status<DfaStatusBase, DfaEdgeBase> DfaStatus;
typedef DfaStatus::Edge DfaEdge;

Link<DfaStatusBase*> DfaStatusBase::AllStatus;
Link<DfaEdgeBase*> DfaEdgeBase::AllEdge;

//class EdgeMatchContent----------------------------

EdgeMatchContent::EdgeMatchContent()
{
}

void EdgeMatchContent::Add(const int MatchContent)
{
	Data.Add(MatchContent);
}

void EdgeMatchContent::Add(Set<int> &MatchContent)
{
	Data.Add(MatchContent);
}

//class NfaStatusNumber------------------------------

NfaStatusNumber::NfaStatusNumber()
{
	StatusNumber=0;
}

//class DfaStatusNumber------------------------------

DfaStatusNumber::DfaStatusNumber()
{
	StatusNumber=0;
}

void DfaStatusNumber::Add(const int Temp_StatusNumber)
{
	NfaStatusNumberSet.Add(Temp_StatusNumber);
}



