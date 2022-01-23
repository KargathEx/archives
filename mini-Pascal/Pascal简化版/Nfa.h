#ifndef NFA_H
#define NFA_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "StatusEdge.h"
#include "EpsilonNfa.h"

using namespace std;

typedef StatusBase<NfaStatusNumber, EdgeMatchContent> NfaStatusBase;
typedef NfaStatusBase::EdgeBase NfaEdgeBase;
typedef Status<NfaStatusBase, NfaEdgeBase> NfaStatus;
typedef NfaStatus::Edge NfaEdge;

class Nfa
{
public:
	NfaStatus Start;
	Link<NfaStatusBase*> AvailableStatus; // 有效状态
	int StatusSize; // 状态数量: 有效状态个数

	Nfa();
	Nfa(EpsilonNfa& EpsilonNfaGraph);
	void Initialize(EpsilonNfa& EpsilonNfaGraph);
	void GetNfa(EpsilonNfa& EpsilonNfaGraph); // 生成NFA
	void Print();

private:
	void GetAvailableStatus(EpsilonNfa& EpsilonNfaGraph); // 找到所有有效状态
	bool IsEpsilonEdge(NfaEdgeBase* Object); // 判断是否为Epsilon边，即边无任何匹配内容
	void GetEpsilonClosure(NfaStatusBase* Temp_AvailableStatus, Set<NfaStatusBase*>& EpsilonClosure); // 找到某个状态的Epsilon闭包
	void InsertEdge(); // 添加必要的边
	void RemoveUnnessaryData(); // 删除多余数据
};

#endif