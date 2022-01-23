#ifndef SET_H
#define SET_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "Link.h"

using namespace std;

template<typename _Type>
class Set
{
public:
	Link<_Type> Data;
	int Size;

	Set():Size(0){}
	~Set(){}

	Node<_Type>* GetHead()
	{
		return Data.GetHead();
	}

	void Copy(Set<_Type>& Object)
	{
		Data.Copy(Object.Data);
		Size=Object.Size;
	}

	void Sub(Set<_Type>& Object)	//¥”÷–ºı»•£¨‡≈.
	{
		Node<_Type>* Temp_Object=Object.GetHead();
		while (Temp_Object)
		{
			Node<_Type>* Temp=Data.GetHead();
			while (Temp)
			{
				if (Temp_Object->Data==Temp->Data)
				{
					Node<_Type>* TempDelete=Temp;
					Temp=Temp->Next;
					Data.Delete(TempDelete);
					--Size;
				}
				else Temp=Temp->Next;
			}
			Temp_Object=Temp_Object->Next;
		}
	}

	bool Add(const _Type& Object)
	{
		Node<_Type>* Temp=Data.GetHead();
		if (Temp)
		{
			while (Temp)
			{
				if (Object==Temp->Data)
					return false;
				Temp=Temp->Next;
			}
		}
		Data.AddLast()->Data=Object;
		++Size;
		return true;
	}

	void Add(const Set<_Type>& Object)
	{
		Node<_Type>* Temp=Object.Data.Head;
		while (Temp)
		{
			Add(Temp->Data);
			++Size;
			Temp=Temp->Next;
		}
	}

	void Delete(Node<_Type>* Object)
	{
		Data.Delete(Object);
		--Size;
	}

	Node<_Type>* Find(const _Type& Object)
	{
		return Data.Find(Object);
	}

	bool Exists(const _Type& Object)
	{
		return Data.Find(Object)!=0;
	}

	bool operator==(Set<_Type>& Object)const
	{
		if (Size==Object.Size)
		{
			Node<_Type>* Temp=Data.Head;
			while(Temp)
			{
				bool Judge=false;
				Node<_Type>* Temp_Object=Object.GetHead();
				while (Temp_Object)
				{
					if (Temp->Data==Temp_Object->Data)
					{
						Judge=true;
						break;
					}
					Temp_Object=Temp_Object->Next;
				}
				if (!Judge) return false;
				Temp=Temp->Next;
			}
			return true;
		}
		else return false;
	}

	void Zero()
	{
		Data.Zero();
	}

};

#endif