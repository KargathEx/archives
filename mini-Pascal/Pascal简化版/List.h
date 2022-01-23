#ifndef LIST_H
#define LIST_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <locale.h>

using namespace std;

template<typename _Type>
class List
{
protected:
	_Type* Data;
	int Length; //数组的长度
	int Capacity; //数组的容量，容量大于长度的时候才需要重新New
	static const int CAPACITY_ADD; //每次增大容量的大小

private:
	bool SetCapacity() //若需更新容量，返回真
	{
		if (Capacity<Length)
		{
			while (Capacity<=Length)
				Capacity=Capacity+CAPACITY_ADD;
			return true;
		}
		return false;
	}

public:
	List()
	{
		Data=0;
		Length=0;
		Capacity=0;
	}

	List(const int Count)
	{
		Data=0;
		Length=Count;
		Capacity=0;
		New();
	}

	List(const List<_Type>& Object)
	{
		Data=0;
		Length=Object.Length;
		Capacity=0;
		New();
		for (int i=0; i<=Length-1; i++)
			Data[i]=Object[i];
	}

	~List()
	{
		Release();
		Length=0;
		Capacity=0;
	}

	void Print()const
	{
		for (int i=0; i<=Length-1; i++)
			cout<<Data[i]<<" ";
		cout<<endl;
	}

	int Size()const
	{
		return Length;
	}

	const _Type* Buffer()const
	{
		return Data;
	}

	void New()
	{
		if (SetCapacity())
		{
			Release();
			Data=new _Type[Capacity];
		}
	}

	void Release()
	{
		if (Data) delete[] Data;
		Data=0;
	}

	void Insert(const int Index, const _Type& Temp)
	{
		Length++;
		if (SetCapacity())
		{
			_Type* Result=new _Type[Capacity];
			for (int i=0; i<=Length-1; i++)
			{
				if (i<Index)
					Result[i]=Data[i];
				else if (i==Index)
					Result[i]=Temp;
				else Result[i]=Data[i-1];
			}
			Release();
			Data=Result;
		}
		else
		{
			int Position=Length-1;
			for (int i=Length-1; i>=Index+1; i--)
				Data[i]=Data[--Position];
			Data[Index]=Temp;
		}
	}

	void Insert(const int Index, const List<_Type>& Object)
	{
		if (Index>=0 && Index<=Length)
		{
			int OldLength=Length;
			Length=Length+Object.Length;
			if (SetCapacity())
			{
				_Type* Result=new _Type[Capacity];
				for (int i=0; i<=Length-1; i++)
				{
					if (i<Index)
						Result[i]=Data[i];
					else if (i>=Index && i<Index+Object.Length)
						Result[i]=Object[i-Index];
					else Result[i]=Data[i-Object.Length];
				}
				Release();
				Data=Result;
			}
			else
			{
				int Position=OldLength;
				for (int i=Length-1; i>=Index+Object.Length; i--)
					Data[i]=Data[--Position];
				for (int i=Index; i<Index+Object.Length; i++)
					Data[i]=Object[i-Index];
			}
		}
	}

	void Add(const _Type& Temp)
	{
		Insert(Length, Temp);
	}

	void Add(const List<_Type>& Object)
	{
		Insert(Length, Object);
	}

	void Delete(const int Index, const int Count)
	{
		if (Index>=0 && Index<=Length-1 && Count>0 && Count<=Length-Index)
		{
			for (int i=Index+Count; i<=Length-1; i++)
				Data[i-Count]=Data[i];
			Length=Length-Count;
		}
	}

	List<_Type> Sub(const int Index, const int Count)const
	{
		List<_Type> Result;
		if (Index<0 || Index>Length-1 || Count<1 || Count>Length-Index)
			return Result;
		else
		{
			Result.Length=Count;
			Result.New();
			int Position=0;
			for (int i=Index; i<Index+Count; i++)
				Result[Position++]=Data[i];
			return Result;
		}
	}

	List<_Type> Left(const int Count)const
	{
		if (Count<0 || Count>Length)
		{
			List<_Type> Result;
			return Result;
		}
		else
		{
			List<_Type> Result(Count);
			for (int i=0; i<=Count-1; i++)
				Result[i]=Data[i];
			return Result;
		}
	}

	List<_Type> Right(const int Count)const
	{
		if (Count<0 || Count>Length)
		{
			List<_Type> Result;
			return Result;
		}
		else
		{
			List<_Type> Result(Count);
			int Position=0;
			for (int i=Length-Count; i<=Length-1; i++)
				Result[Position++]=Data[i];
			return Result;
		}
	}

	List<_Type> operator=(const List<_Type>& Object)
	{
		Length=Object.Length;
		New();
		for (int i=0; i<=Length-1; i++)
			Data[i]=Object[i];
		return *this;
	}

	bool operator==(const List<_Type>& Object)const
	{
		if (Length!=Object.Length)
			return false;
		else
		{
			for (int i=0; i<=Length-1; i++)
				if (Data[i]!=Object[i])
					return false;
		}
		return true;
	}

	bool operator!=(const List<_Type>& Object)const
	{
		if (Length!=Object.Length)
			return true;
		else
		{
			for (int i=0; i<=Length-1; i++)
				if (Data[i]!=Object[i])
					return true;
		}
		return false;
	}

	_Type& operator[](const int Index)
	{
		if (Index>=0 && Index<=Length-1)
			return Data[Index];
		else throw "List越界访问";
	}

	_Type operator[](const int Index)const
	{
		if (Index>=0 && Index<=Length-1)
			return Data[Index];
		else throw "List越界访问";
	}

	List<_Type> operator+(const List<_Type>& Object)const
	{
		List<_Type> Result;
		Result.Length=Length+Object.Length;
		Result.New();
		for (int i=0; i<=Result.Length-1; i++)
		{
			if (i<Length)
				Result[i]=Data[i];
			else Result[i]=Object[i-Length];
		}
		return Result;
	}
};

template<typename _Type>
const int List<_Type>::CAPACITY_ADD=20; //每次增大容量的大小

#endif