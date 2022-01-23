#ifndef LYTPTR_H
#define LYTPTR_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <locale.h>

using namespace std;

template<typename _Type>
class LytPtr // 智能指针
{
private:
	int* Count; //引用计数
	_Type* Object; // 智能指针控制对象

	void Increase()		//这里关于对象的判断意思是不为空? 什么时候会为空?
	{
		if (Object && Count) (*Count)++;	
	}

	void Decrease()		//当到达0的时候删掉对象并且归零
	{
		if (Object && Count)
		{
			(*Count)--;
			if (!(*Count))
			{
				if (Object) delete Object;
				delete Count;
				Object=0;
				Count=0;
			}
		}
	}

public:
	LytPtr():Count(0),Object(nullptr){}

	LytPtr(_Type* Temp):Object(Temp)		
	{										
		Count=new int(1);					
	}
//为什么这帮人都喜欢new... class里的东西就得这样吗？
//这个应该是接收new xx 返回的指针的那个构造函数，此LytPtr构造出来之后，
//在它作用域消失的地方如果没有别的东西要它，他自己就消减自己的引用计数，析构自己，
//所以，如果有赋值的话，是“多个LytPtr共享此两个私有变量”的？
//多个 shared_ptr 对象可以共同托管一个指针 p，当所有曾经托管 p 的 shared_ptr 对象都解除了对其的托管时，就会执行delete p
	//看起来确实如此..

	LytPtr(const LytPtr<_Type>& Temp):Count(Temp.Count),Object(Temp.Object)
	{
		Increase();
	}

	template<typename T>
	LytPtr(const LytPtr<T>& Temp)
	{
		Object=dynamic_cast<_Type*>(Temp.Buffer());	//这个是用来把其它类型的赋值给当前类型的，能解决协变问题? 为什么要dynamic_cast?
		//如果 表达式 是到多态类型 Base 的指针或引用，且 新类型 是到 Derived 类型的指针或引用，则进行运行时检查.sidecast,downcast
		if (Object)				
		{
			Count=Temp.GetCount();
			Increase();
		}
		else
		{
			Object=0;
			Count=0;
		}
	}

	~LytPtr()
	{
		Decrease();
	}

	operator bool()const	//这个玩意是啥？ 调用它的地方？ bool是类型转换对吧？ 只要不等于0就转换到. 或者转换到.
	{
		return Object!=0;
	}

	LytPtr<_Type>& operator=(_Type* Temp)
	{
		Decrease();				
								
		Count=new int(1);		
		Object=Temp;			
		return *this;			
	}
	//对于赋值操作，先递减之前保管的对象，然后把新对象设置为1然后接过来指针的值。 为什么要设置为1？
	//而不是接受之前的值?哦，因为这个接受的是原始指针。 那原始指针被他接受之后，需要自己先delete自己的吗？ 你说呢，呵呵..
	//这个东西是为了当前指针对象被析构的时候依旧存在这个变量吗？ 不然为啥要动态分配?
	//或者，是为了传递给别的对象的时候，依旧能找到这块地址里存的值?  
	//别人家的智能指针也都必须new出来局部变量吗? 看起来是的，那std里的如何呢?

	LytPtr<_Type>& operator=(const LytPtr<_Type>& Temp)
	{
		Decrease();
		Object=Temp.Object;
		Count=Temp.Count;
		Increase();
		return *this;
	}

	bool operator==(_Type* Temp)const
	{
		if (Object) return Object==Temp;
		else
		{
			if (Temp) return false;
			else return true;
		}
	}

	bool operator==(const LytPtr<_Type>& Temp)const
	{
		return Object==Temp.Object;
	}

	bool operator!=(_Type* Temp)const
	{
		if (Object) return Object!=Temp;
		else
		{
			if (Temp) return true;
			else return false;
		}
	}

	_Type* operator ->()const
	{
		if (Object) return Object;
		else return 0;
	}

	_Type* Buffer()const
	{
		if (Object) return Object;
		else return 0;
	}
	
	int* GetCount()const
	{
		return Count;
	}
};

#endif