#ifndef LYTSTRING_H
#define LYTSTRING_H

#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <string.h>
#include <math.h>
#include <iomanip> 
#include "LytWString.h"

using namespace std;

class LytString
{
protected:
    int length;
    char* s;
public:
    LytString();
    /*功能：定义LytString(无参数)
      返回值：类型为LytString，其中字符串s为空，长度length为0*/

    LytString(const char* ss);
    /*功能：定义LytString
      参数：类型为不可修改的字符串指针
      返回值：类型为LytString，所带字符串指针s内容同参数ss*/
      
    LytString(const LytString& o);
    /*功能：定义LytString 
      参数：类型为不可修改的LytString
      返回值：内容同参数o，类型为LytString*/
      
    LytString operator=(const char* ss);
    /*功能：定义操作符=
            （LytSting）=(字符串指针ss) 
      参数：记录=右端参数，类型为不可修改的字符串指针
      返回值：类型为LytString，所带字符串指针s内容同于参数ss*/
      
    LytString operator=(const LytString & o);
    /*功能：定义操作符=
            (LytString)=(LytString)
      参数：记录=右端参数，类型为不可修改的LytString
      返回值：类型为LytString，内容同于参数o*/
      
    friend ostream& operator<<(ostream& p,const LytString& o);
    /*功能：定义操作符<<，输出类型为LytString的参数o 
            （ostream）<<(LytString)
      参数：p记录<<左边类型为ostream的函数，o为记录<<右边类型为LytString的参数
      返回值：类型为ostream*/
      
    friend istream& operator>>(istream& p,LytString& o);
    /*功能：定义操作符<<，输出类型为LytString的参数o 
            （ostream）<<(LytString)
      参数：p记录<<左边类型为ostream的函数，o为记录<<右边类型为LytString的参数
      返回值：类型为ostream*/
      
    bool operator==(const LytString& o)const;
    /*功能：定义操作符==
            （LytString）==(LytString) 
      参数：o记录==右边的LytString
      返回值：类型不可修改的为布尔值，若两者字符串s相同，返回真，反之则返回假假*/
      
    bool LytString::operator!=(const LytString& o)const;
    /*功能：定义操作符!=
            （LytString）!=(LytString) 
      参数：o记录!=右边的LytString
      返回值：类型不可修改的为布尔值，若两者字符串s不同，返回真，反之则返回假假*/
      
    bool LytString::operator>(const LytString& o)const;
    /*功能：定义操作符> 
            （LytString）>(LytString) 
      参数：o记录>右边的LytString
      返回值：类型不可修改的为布尔值，若前者大于后者，返回真，反之则返回假假*/
      
    bool LytString::operator<(const LytString& o)const;
    /*功能：定义操作符< 
            （LytString）<(LytString) 
      参数：o记录>右边的LytString
      返回值：类型不可修改的为布尔值，若前者小于后者，返回真，反之则返回假假*/
      
    bool LytString::operator>=(const LytString& o)const;
    /*功能：定义操作符>= 
            （LytString）>=(LytString) 
      参数：o记录>=右边的LytString
      返回值：类型不可修改的为布尔值，若前者大于等于后者，返回真，反之则返回假假*/
      
    bool LytString::operator<=(const LytString& o)const;
    /*功能：定义操作符<=
            （LytString）<=(LytString) 
      参数：o记录<=右边的LytString
      返回值：类型不可修改的为布尔值，若前者小于等于后者，返回真，反之则返回假假*/
      
    LytString operator+(const LytString& o)const;
    /*功能：定义操作符+，将+两端连接的LytString所带字符串s连接起来 
            （LytString）+(LytString)
      参数：记录+右端参数，类型为不可修改的LytString
      返回值：类型为LytString，所带字符串指针s内容为以上俩字符串连接后的结果*/
      
    LytString operator+(const char* ss)const;
    /*功能：定义操作符+，将+两端连接的LyuString所带字符串s与字符串ss连接起来
            （LytString）+(char*)
      参数：记录+右边参数，类型为不可修改的字符串指针
      返回值：类型为LytString，所带字符串指针s内容为以上俩字符串连接后的结果*/
      
    friend LytString operator+(const char*ss , const LytString& o);
    /*功能：定义操作符+将+两端连接的字符串ss与LyuString所带字符串s连接起来
            （char*）+(LytString)
      参数：ss记录+左边字符串指针，o为记录+右边类型为LytString参数
      返回值：类型为LytString，所带字符串指针s内容为以上俩字符串连接后的结果*/ 
      
    LytString sub(const int index,const int count)const;
    /*函数名:sub
      功能:从第index位开始，取出此LytString所带字符串指针s中count个字符
      参数:index记录从第几位开始，范围为0<=index<=length-1，类型为不可修改的整数 
           count记录所取出字符个数，范围为1<=count<=length-index，类型为不可修改的整数 
      返回值：类型为不可修改的LytString，所带字符串指针s内容为所取出的字符串
      错误数据处理方法：若index<0或index>length-1或count<1或count>length-index，返回值所带字符串指针s为空*/
      
    void insert(const int index,const LytString o);
    /*函数名：insert
      功能：从第index位开始，插入类型为LytString的字符串o
      参数：index记录从第几位开始，范围为0<=index<=length-1，类型为不可修改的整数
            o记录插入的字符串，类型为不可修改的LytString，其中o所带字符串指针内容不可为空 
      错误数据处理方法：若index<0或index>length-1或参数o所带字符串指针内容为空，字符串不做任何改变*/
      
    void Delete(int index,int count);
    /*函数名：Delete
      功能：从第index位开始，删除count个字符
      参数：index记录从第几位开始，范围为0<=index<=length-1，类型为不可修改的整数
            count记录删除字符的个数，范围为0<=count<=length-index，类型为不可修改的整数
      错误数据处理方法：若index<0或index>length-1或count<0或count>length-index，字符串不做任何修改*/
                        
    LytString toupper()const;
    /*函数名：toupper
      功能：返回一个LytString，其中字符串指针s的内容为将其中小写字符改为大写字符的结果
      返回值：类型为不可修改的LytString*/
    
    LytString tolower()const;
    /*函数名：tolower
      功能：返回一个LytString，其中字符串指针s的内容为将其中大写字符改为小写字符的结果
      返回值：类型为不可修改的LytString*/
      
    LytString left(const int count)const;
    /*函数名：left
      功能：从左边开始，取count个字符，成为一个类型为LytString的字符串 
      参数：count记录所得LytString的长度，范围为0<=count<=length类型为不可修改的整数
      返回值：类型为不可修改的LytString
      错误数据处理：count<0或count>length，返回字符串内容为空*/
      
    LytString right(const int count)const;
    /*函数名：right
      功能：从右边开始，取count个字符，成为一个类型为LytString的字符串 
      参数：count记录所得LytString的长度，范围为1<=count<=length类型为不可修改的整数
      返回值：类型为不可修改的LytString
      错误数据处理：若count<0或count>length，返回字符串内容为空*/
      
    LytString trimleft()const;
    /*函数名：trimleft
      功能：返回一个LytString，其中字符串指针内容s为将其左端空格去掉后的结果
      返回值：类型为不可修改的LytString*/
    
    LytString trimright()const;
    /*函数名：trimright
      功能：返回一个LytString，其中字符串指针内容s为将其右端空格去掉后的结果
      返回值：类型为不可修改的LytString*/
    
    LytString trim()const;
    /*函数名：trim
      功能：返回一个LytString，其中字符串指针内容s为将其左端右端空格去掉后的结果
      返回值：类型为不可修改的LytString*/
      
    int pos(const LytString& o)const;
    /*函数名：pos
      功能：给出LytString类型的参数o在字符串s中第一次出现的位置
      参数：o记录要找的字符串，至少在字符串s中出现一次(非空)，类型为不可修改的LytString
      返回值：返回o在字符串s中第一次出现的位置，类型为不可修改的整数
      错误数据处理方法：若在字符串s中找不到o或者o为空字符串，返回-1*/
      
    int replace(const LytString& find , const LytString& result);
    /*函数名：replace
      功能：在字符串s中找到find，再将其替换为result
      参数：find记录所要替换的字符串，且find不可为空，类型为不可修改的LytString
            result记录替换的结果，类型为不可修改的LytString
      返回值：返回修改的次数，类型为不可修改整数
      错误数据处理方法：若find为空或在s中找不到find，返回0，字符串本身不做任何修改*/

	LytWString ToWString()const;
    
    friend int StrToInt(const LytString& o);
    friend LytString IntToStr(const int i);
    friend double StrToDouble(const LytString& o);
    friend LytString DoubleToStr(const double d); 

	int Size()const;
	char& operator[](int Index);
	const char* Buffer()const;

    
    ~LytString();
    /*析构函数*/
};

#endif
