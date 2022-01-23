#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <stdlib.h>
#include <time.h>
//#include <string.h>
#include <conio.h>
#include <math.h>
#include "LytWString.h"

using namespace std;

template<typename _Type>
class TreeNode
{
public:
	_Type Data;
	TreeNode* Left;
	TreeNode* Right;
	TreeNode* Up;		//是根节点

	TreeNode()
	{
		Left=nullptr;
		Right=nullptr;
		Up=nullptr;
	}

	~TreeNode()	//这个递归过程是如何进行的?如果从根节点的话还好，如果从顶上岂不是一下子就断了？ 我猜delete这个会调用析构函数吧?是的，相互递归的过程.
	{
		if (Left) delete Left;	
		if (Right) delete Right;
	}

	LytWString ToString(LytWString Prefix , LytWString(*Conv)(_Type&))
	{
		if(this)
		{
			LytWString Result=Prefix+Conv(Data);
			if(Left||Right)
			{
				Result=Result+L"{\r\n";
				if(Left)
					Result=Result+Left->ToString(Prefix+L"  ",Tos);
				if(Right)
					Result=Result+Right->ToString(Prefix+L"  ",Tos);
				return Result+Prefix+L"}\r\n";
			}
			else return Result+L"\r\n";
		}
		else return L"";
	}
};

template <typename _Type>
class Tree
{
public:
	TreeNode<_Type>* TreeHead;

	Tree()
	{
		TreeHead=new TreeNode<_Type>;
	}

	~Tree()
	{
		if (TreeHead) delete TreeHead;
		TreeHead=0;
	}

	TreeNode<_Type>* AddUp()
	{
		TreeNode<_Type>* Temp=new TreeNode<_Type>;
		if (TreeHead!=0)
		{
			TreeHead->Up=Temp;	
			Temp->Up=0;
			Temp->Left=TreeHead;
			Temp->Right=0;
			Temp->Left->Up=Temp;
		}
		else
		{
			Temp->Up=0;
			Temp->Left=0;
			Temp->Right=0;
		}
		TreeHead=Temp;
		return Temp;
	}

	TreeNode<_Type>* AddLeft(TreeNode<_Type>* X)
	{
		if (X!=0 && X->Left==0)
		{
			TreeNode<_Type>* Temp=new TreeNode<_Type>;
			Temp->Up=X;
			X->Left=Temp;
			Temp->Left=0;
			Temp->Right=0;
			Temp->Up->Left=Temp;
			return Temp;
		}
		else return 0;
	}

	TreeNode<_Type>* AddRight(TreeNode<_Type>* X)
	{
		if (X!=0 && X->Right==0)
		{
			TreeNode<_Type>* Temp=new TreeNode<_Type>;
			Temp->Up=X;
			X->Right=Temp;
			Temp->Left=0;
			Temp->Right=0;
			Temp->Up->Right=Temp;
			return Temp;
		}
		else return 0;
	}

	TreeNode<_Type>* MoveDown(TreeNode<_Type>* X)	//把当前节点往下挪，给他找个爹，然后自己变成左孩子,Temp->Left=X; 传入的参数确实会因为多出来一个参数操了自己的爹而变成孙子，嗯。 返回的是作为根节点的Op,所以这个赋值是赋给X的爹了，难怪line 734会看Up. 如果up!=null,则可以比较.
	{
		if (X!=0)
		{
			TreeNode<_Type>* Temp=new TreeNode<_Type>;	//在非空的前提下，创建新的. 如果X是树根的话，就把此空的复制为根.
			if (X==TreeHead)
				TreeHead=Temp;
			if (X->Up!=0)	//如果X的父节点不为空，代表他是某个节点的子节点，则把temp的上一个设置为x的上一个,如果X是右子节点，则设置temp为它父节点的右子节点 否则设置为子节点。 然后把X设置为temp的孩子。		所以这个真的是，new出来一个新节点，替换掉X的位置，然后把X设置为它的子节点。
			{
				Temp->Up=X->Up;
				if (X->Up->Right==X) 
					Temp->Up->Right=Temp;
				else Temp->Up->Left=Temp;
				Temp->Left=X;
				Temp->Right=0;
				X->Up=Temp;
			}
			else
			{
				Temp->Up=0;		//如果x是根节点，则把x设置为根节点的左孩子.
				Temp->Left=X;
				Temp->Right=0;
				X->Up=Temp;
			}
			return Temp;
		}
		else return 0;
	}

	TreeNode<_Type>* MoveLeft(TreeNode<_Type>* X)
	{
		if (X!=0)
		{
			TreeNode<_Type>* Temp=new TreeNode<_Type>;
			if (X==TreeHead)
				TreeHead=Temp;
			if (X->Up!=0)
			{
				Temp->Up=X->Up;
				if (X->Up->Right==X) Temp->Up->Right=Temp;
				else Temp->Up->Left=Temp;
				Temp->Left=X;
				Temp->Right=0;
				X->Up=Temp;
			}
			else
			{
				Temp->Up=0;
				Temp->Left=X;
				Temp->Right=0;
				X->Up=Temp;
			}
			return Temp;
		}
		else return 0;
	}

	TreeNode<_Type>* MoveRight(TreeNode<_Type>* X)
	{
		if (X!=0)
		{
			TreeNode<_Type>* Temp=new TreeNode<_Type>;
			if (X==TreeHead)
				TreeHead=Temp;
			if (X->Up!=0)
			{
				Temp->Up=X->Up;
				Temp->Up->Left=Temp;
				Temp->Left=X;
				Temp->Right=0;
				X->Up=Temp;
			}
			else
			{
				Temp->Up=0;
				Temp->Left=0;
				Temp->Right=X;
				X->Up=Temp;
			}
			return Temp;
		}
		else return 0;
	}

	TreeNode<_Type>* KeepLeft(TreeNode<_Type>* X)
	{
		if (X->Left)
		{
			TreeNode<_Type>* Keep=X->Left;
			if (X->Right)
				delete X->Right;
			X->Right=0;
			if (X==TreeHead)
			{
				TreeHead=Keep;
				Keep->Up=0;
			}
			else
			{
				X->Left->Up=X->Up;
				if (X->Up->Left==X)
					X->Up->Left=X->Left;
				else X->Up->Right=X->Left;
			}
			delete X;
			X=0;
			return Keep;
		}
		return 0;
	}

	TreeNode<_Type>* KeepRight(TreeNode<_Type>* X)
	{
		if (X->Right)
		{
			TreeNode<_Type>* Keep=X->Right;
			if (X->Left)
				delete X->Left;

			if (X==TreeHead)
			{
				TreeHead=Keep;
				Keep->Up=0;
			}
			else
			{
				X->Right->Up=X->Up;
				if (X->Up->Left==X)
					X->Up->Left=X->Right;
				else X->Up->Right=X->Right;
			}
			delete X;
			X=0;
			return Keep;
		}
		return 0;
	}
};

#endif