#ifndef ORDEREDLINK_H
#define ORDEREDLINK_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "Link.h"

using namespace std;

template <typename _Type,bool AllowMultiply>
class OrderedLink
{
public:
	Link<_Type> Data;

	OrderedLink()
	{
	}

	~OrderedLink()
	{
	}

	Node<_Type>* GetHead()
	{
		return Data.Head;
	}

	void Copy(OrderedLink<_Type, AllowMultiply>& Object)
	{
		Data.Copy(Object.Data);
	}

	bool Add(const _Type& Object)
	{
		Node<_Type>* Temp=Data.Head;
		if (Temp)
		{
			while (Temp)
			{
				if (Object==Temp->Data)
				{
					if (AllowMultiply)
					{
						Data.AddPrev(Temp)->Data=Object;
						return true;
					}
					else return false;
				}
				else if (Object<Temp->Data)
				{
					Data.AddPrev(Temp)->Data=Object;
					return true;
				}
				Temp=Temp->Next;
			}
		}
		Data.AddLast()->Data=Object;
		return true;
	}

	Node<_Type>* Find(const _Type& Object)
	{
		return Data.Find(Object);
	};

	bool Exists(const _Type& Object)
	{
		return Data.Find(Object)!=0;
	}

	void Delete(Node<_Type>* Object)
	{
		if (AllowMultiply)
		{
			Node<_Type>* Temp=Data.Head;
			while (Temp)
			{
				if (Temp->Data==Object->Data)
				{
					Node<_Type>* Temp_Delete=Temp;
					Temp=Temp->Next;
					Data.Delete(Temp_Delete);
				}
				else Temp=Temp->Next;
			}
		}
		else
		{
			Node<_Type>* Temp=Data.Find(Object->Data);
			if (Temp)
				Data.Delete(Temp);
		}
	}

	bool operator==(const OrderedLink<_Type, AllowMultiply>& Object)
	{
		Node<_Type>* Temp=Data.Head;
		Node<_Type>* Temp_Object=Object.Data.Head;
		while (true)
		{
			if (Temp && Temp_Object)
			{
				if (Temp->Data!=Temp_Object->Data)
					return false;
				Temp=Temp->Next;
				Temp_Object=Temp_Object->Next;
			}
			else if (!Temp && !Temp_Object)
				return true;
			else return false;
		}
	}

	void Zero()
	{
		Data.Zero();
	}
};

#endif