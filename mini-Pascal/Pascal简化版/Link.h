#ifndef LINK_H
#define LINK_H

#include <iostream>
#include <string.h>
#include <math.h>
#include <iomanip> 

using namespace std;

template<typename _Type>
class Node
{
public:
	_Type		Data;
	Node*		Prev;
	Node*		Next;

	Node():Prev(nullptr),Next(nullptr){}
	~Node(){}
};

template <typename _Type>
class Link
{
public:
	Node<_Type>*	Head;
	Node<_Type>*	Tail;

	Link() :Head(nullptr), Tail(nullptr) {}

	~Link()
	{
		Clear();
	}

	Node<_Type>* GetHead()
	{
		return Head;
	}

	Node<_Type>* GetTail()
	{
		return Tail;
	}

	void Zero()	//全部成为0
	{
		Node<_Type>* Temp=Head;
		while (Temp)
		{
			Temp->Data=0;
			Temp=Temp->Next;
		}
	}

	void Copy(Link<_Type>& Object)	
	{
		Node<_Type>* Temp_Object=Object.Head;
		while (Temp_Object)
		{
			AddLast()->Data=Temp_Object->Data;	//我觉得这些东西我都该自己实现一遍来保证正确性,自己的练度太垃圾了.
			Temp_Object=Temp_Object->Next;
		}
	}

	void Clear()/*清除所有节点，记得delete完，不要内存泄漏*/
	{
	    while(Head!=0)
	    {
	        Node<_Type>* t=Head->Next;
	        delete Head;
	        Head=t;
        }
        Tail=0;
    }    
	Node<_Type>*    AddFirst()/*在开头增加一个新节点，返回*/
	{
		Node<_Type>* t=new Node<_Type>;
		t->Prev=0;
		if (Head==0)
		{
		    t->Next=0;
		    Head=t;
            Head->Prev=0;
            Head->Next=0;
            Tail=t;
            Tail->Prev=0;
            Tail->Next=0;
        }
        else 
        {
            Head->Prev=t;
            t->Next=Head;
		    Head=t;
        }
		return Head;
	}

	Node<_Type>*	AddLast()/*在末尾增加一个新节点，返回*/
	{
		Node<_Type>* t=new Node<_Type>;
		t->Next=0;
		if (Tail==0)
		{
		    t->Prev=0;
		    Head=t;
            Head->Prev=0;
            Head->Next=0;
            Tail=t;
            Tail->Prev=0;
            Tail->Next=0;
        }
        else
        {
            Tail->Next=t;
		    t->Prev=Tail;
		    Tail=t;
        }
		return Tail;
	}

	Node<_Type>*	AddPrev(Node<_Type>* X)/*在X前面增加一个新节点，返回*/
	{
		if (X!=0)
		{
		    Node<_Type>* t=new Node<_Type>;
		    if (X->Prev==0)
		    {
                t->Prev=0;
                X->Prev=t;
                t->Next=X;
                t->Next->Prev=t;
                Head=t;
            }    
            else
            {
                t->Prev=X->Prev;
                t->Prev->Next=t;
                X->Prev=t;
                t->Next=X;
                t->Next->Prev=t;//写多不会错，为什么要弄还没想明白  
            }
            return t; 
		}
		return 0;
	}

	Node<_Type>*	AddNext(Node<_Type>* X)/*在X后面增加一个新节点，返回*/
	{
	    if (X!=0)
	    {
	        Node<_Type>* t=new Node<_Type>;
		    if (X->Next==0)
	        {
		        t->Next=0;
                X->Next=t;
                t->Prev=X;
                t->Prev->Next=t;
                Tail=t;
            }
            else
            {
                t->Next=X->Next;
                t->Next->Prev=t;
		        X->Next=t;
	            t->Prev=X;
                t->Prev->Next=t;   
            }
		    return t;
        }
		return 0;
	}

	void Delete(Node<_Type>* X)/*删掉X*/
	{
	    if (X!=0)
	    {
	        if (X->Prev==0 && X->Next==0)
	            Clear();
	        else if (X->Prev==0)
	        {
	            Node<_Type>* t=Head->Next;
	            delete X;
	            t->Prev=0;
	            Head=t;
            }
            else if (X->Next==0)
            {
                Node<_Type>* t=Tail->Prev;
                delete X;
                t->Next=0;
                Tail=t;
            }
            else
            {
                X->Prev->Next=X->Next;
                X->Next->Prev=X->Prev;
                delete X;
            } 
        }
	}

	Node<_Type>* Find(const _Type& Data)/*找到data是Data的节点，返回，否则返回0*/
	{
		Node<_Type>* TempNode=Head;
		while (TempNode)
		{
			if (TempNode->Data==Data)
				return TempNode;
			else TempNode=TempNode->Next;
		}
		return 0;
	}

	bool Exists (const _Type& Data)
	{
		return Find(Data)!=0;
	}
};

#endif