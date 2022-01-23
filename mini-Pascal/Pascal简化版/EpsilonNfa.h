#ifndef EPSILONNFA_H
#define EPSILONNFA_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "Tree.h"
#include "CharClass.h"
#include "StatusEdge.h"

using namespace std;

typedef StatusBase<NfaStatusNumber, EdgeMatchContent> NfaStatusBase;
typedef NfaStatusBase::EdgeBase NfaEdgeBase;
typedef Status<NfaStatusBase, NfaEdgeBase> NfaStatus;
typedef NfaStatus::Edge NfaEdge;

class EpsilonNfa
{
public:
	NfaStatus Start;
	NfaStatus End;
	int StatusSize; // 状态数量

	EpsilonNfa();
	EpsilonNfa GetEpsilonNfa(GrammarTree& ExpressionGrammarTree, CharClassLink& CharLink); //生成EpsilonNfa
	void Print();

private:
	void SetStausNumber(); // 设置状态编号
	EpsilonNfa BuildEpsilonNfa(TreeNode<GrammarTreeNodeData>* Temp_TreeNode, CharClassLink& CharLink);
	void Reverse(TreeNode<GrammarTreeNodeData>* Temp_TreeNode, CharClassLink& CharLink, Set<int>& CharNumber); //处理 [] 中的 ^
	void RemoveUnnessaryData(); // 删除多余数据
	void SetFinalStatus(); // 设置结束状态
};

#endif