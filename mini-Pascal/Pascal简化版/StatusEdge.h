#ifndef STATUSEDGE_H
#define STATUSEDGE_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "Set.h"

using namespace std;

class EdgeMatchContent // 边的匹配内容
{
public:
	Set<int> Data;

	EdgeMatchContent();
	void Add(const int MatchContent);
	void Add(Set<int>& MatchContent);
};

class NfaStatusNumber // Nfa编号
{
public:
	int StatusNumber;

	NfaStatusNumber();
};

class DfaStatusNumber // Dfa编号
{
public:
	Set<int> NfaStatusNumberSet; // 对应的Nfa状态编号
	int StatusNumber;

	DfaStatusNumber();
	void Add(const int Temp_StatusNumber);
};

template<typename StatusNumber, typename EdgeContent>
class StatusBase
{
public:
	class EdgeBase
	{
	public:
		EdgeContent Data;
		static Link<EdgeBase*> AllEdge; // 所有的边
		StatusBase* From;
		StatusBase* To;

		EdgeBase()
		{
			From=0;
			To=0;
		}

		static void ReleaseEdge() // 内存管理: 释放边
		{
			Node<EdgeBase*>* Temp=AllEdge.Head;
			while (Temp)
			{
				delete Temp->Data;
				Temp=Temp->Next;
			}
		}
	};

	Link<EdgeBase*> InEdges;
	Link<EdgeBase*> OutEdges;
	static Link<StatusBase*> AllStatus; // 所有的状态
	bool FinalStatus;
	StatusNumber Data;

	StatusBase()
	{
		FinalStatus=0;
	}

	static void ReleaseStatus() // 内存管理: 释放状态
	{
		Node<StatusBase*>* Temp=AllStatus.Head;
		while (Temp)
		{
			delete Temp->Data;
			Temp=Temp->Next;
		}
	}
};

template<typename StatusData, typename EdgeData>
class Status
{
public:
	class Edge
	{
	public:
		EdgeData* Data;

		Edge()
		{
			Data=0;
		}

		Edge(EdgeData* Object)
		{
			Data=Object;
		}

		void New()
		{
			Data=new EdgeData;
			EdgeData::AllEdge.AddLast()->Data=Data;
		}

		void Connect(Status& FromStatus, Status& ToStatus) // 添加边链接状态FromStatus和ToStatus
		{
			New();
			FromStatus.Data->OutEdges.AddLast()->Data=Data;
			ToStatus.Data->InEdges.AddLast()->Data=Data;
			Data->From=FromStatus.Data;
			Data->To=ToStatus.Data;
		}

		void Connect(StatusData* FromStatusBase, StatusData* ToStatusBase) // 添加边链接状态FromStatus和ToStatus
		{
			New();
			FromStatusBase->OutEdges.AddLast()->Data=Data;
			ToStatusBase->InEdges.AddLast()->Data=Data;
			Data->From=FromStatusBase;
			Data->To=ToStatusBase;
		}

		void CutOff() // 删除边
		{
			Node<EdgeData*>* Temp=Data->From->OutEdges.Find(Data);
			Temp->Data=0;
			Data->From->OutEdges.Delete(Temp);

			Temp=Data->To->InEdges.Find(Data);
			Temp->Data=0;
			Data->To->InEdges.Delete(Temp);
		}

	};

	StatusData* Data;

	Status()
	{
		Data=0;
	}

	Status(StatusData* Object)
	{
		Data=Object;
	}

	void New()
	{
		Data=new StatusData;
		StatusData::AllStatus.AddLast()->Data=Data;
	}
};

#endif