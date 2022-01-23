#ifndef DFA_H
#define DFA_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "CharClass.h"
#include "StatusEdge.h"
#include "Nfa.h"

using namespace std;

typedef StatusBase<DfaStatusNumber, EdgeMatchContent> DfaStatusBase;
typedef DfaStatusBase::EdgeBase DfaEdgeBase;
typedef Status<DfaStatusBase, DfaEdgeBase> DfaStatus;
typedef DfaStatus::Edge DfaEdge;

class Dfa
{
public:
	DfaStatus Start;
	int StatusSize; // 状态数量

	Dfa();
	void GetDfa(Nfa& NfaGraph); // 生成DFA
	void Print();

private:
	Set<DfaStatusBase*> UnsettledStatus; // 未处理的状态
	Set<DfaStatusBase*> ExisentStatus; // DFA存在的状态

	void SetStatusNumber(); //设置状态编号
	DfaStatusBase* NfaStatusBaseToDfa(NfaStatusBase* Object); // 将NFA状态转化为DFA状态
	void FindCorrespondingNfaStatus(Set<int>& StatusNumber, Nfa& NfaGraph, Link<NfaStatusBase*>& Result); // 提供状态编号，找到某个DFA状态所对应的NFA状态
	DfaStatusBase* IsExisentStatus(Set<int>& StatusNumber, bool Final); // 某个状态是否存在于DFA
};

#endif