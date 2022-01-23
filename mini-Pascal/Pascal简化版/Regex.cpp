#include "Regex.h"

Regex::MatchResult::MatchResult()
{
	Start=0;
	Length=0;
}

Regex::MatchResult::MatchResult(const Regex::MatchResult& Object)
{
	Start=Object.Start;
	Length=Object.Length;
}

Regex::MatchResult::~MatchResult()
{
}

Regex::Regex()
{
}

void Regex::ReleaseGraph()
{
	//内存管理: 释放图NFA
	NfaStatusBase::ReleaseStatus();
	NfaEdgeBase::ReleaseEdge();
	NfaStatusBase::AllStatus.Clear();
	NfaEdgeBase::AllEdge.Clear();

	//内存管理: 释放图DFA
	DfaStatusBase::ReleaseStatus();
	DfaEdgeBase::ReleaseEdge();
	DfaStatusBase::AllStatus.Clear();
	DfaEdgeBase::AllEdge.Clear();
}

Regex::Regex(const LytWString Expression)
{
	ReleaseGraph();

	//正则表达式语法树
	GrammarTree ExpressionGrammarTree(Expression);
	ExpressionGrammarTree.GetGrammarTree();
	SetErrorMessage(ExpressionGrammarTree.GetErrorMessage());

	if (IsError()==false)
	{
		//字符集
		CharLink.SetCharNumber(ExpressionGrammarTree);
	
		//ε-NFA
		EpsilonNfa EpsilonNfaGraph;
		EpsilonNfaGraph=EpsilonNfaGraph.GetEpsilonNfa(ExpressionGrammarTree, CharLink);
	
		//NFA
		Nfa NfaGraph(EpsilonNfaGraph);
		NfaGraph.GetNfa(EpsilonNfaGraph);
	
		//DFA
		Dfa DfaGraph;
		DfaGraph.GetDfa(NfaGraph);

		//DFA矩阵
		DfaGraphMatrix.New(DfaGraph.StatusSize, CharLink.Size);
		DfaGraphMatrix.GetDfaMatrix();

		ReleaseGraph();
	}
}

bool Regex::IsError()const
{
	if (ErrorMessage==L"")
		return false;
	else return true;
}

void Regex::SetErrorMessage(const LytWString ErrorInformation)
{
	ErrorMessage=ErrorInformation;
}

LytWString Regex::GetErrorMessage()const
{
	return ErrorMessage;
}

int Regex::RunDfa(const wchar_t* Input)
{
	int LastFinalLength=-1;
	if (IsError()==0)
	{
		int Status=1;
		const wchar_t* Initializtion=Input;
		while (Status!=0)
		{
			if (DfaGraphMatrix.Final[Status])
				LastFinalLength=Input-Initializtion;

			if (Input[0])
			{
				GrammarTreeNodeData Temp;
				if (Input[0]==L'\\')
				{
					if (Input[1]!=L'\\')
						Temp.Type=Transferred;
					else Temp.Type=Char;
					++Input;
				}
				else Temp.Type=Char;
				Temp.Data=*Input++;
	
				int CharNumber=CharLink.GetCharNumber(Temp);
				if (CharNumber<=0)
					break;
				Status=DfaGraphMatrix.Data(Status, CharNumber);
			}
			else break;
		}
		return LastFinalLength;
	}
	else return LastFinalLength;
}

Regex::MatchResult Regex::Match(const LytWString& Object)
{
	SetErrorMessage(L"");
	const wchar_t* Input=Object.Buffer();
	MatchResult Result;
	if (IsError()==false)
	{
		const wchar_t* InputBuffer=Input;
		int TempLength=RunDfa(InputBuffer);
		if (TempLength!=-1)
		{
			Result.Start=InputBuffer;
			Result.Length=TempLength;
			return Result;
		}
		else SetErrorMessage(L"匹配失败");
	}
	else return Result;
}