#include "LytString.h"

LytString::LytString()
{
    length=1;
    s=new char[length]; //确实 ，空的还要一个'\0'呢.
    strcpy(s,"");
}

LytString::LytString(const char* ss)
{
    length=(int)strlen(ss);
    s=new char[length+1];
    strcpy(s,ss);   // <-
}

LytString::LytString(const LytString& o)
{
    length=o.length;
    s=new char[length+1];
    strcpy(s,o.s);
}

LytString LytString::operator=(const char* ss)
{
    length=(int)strlen(ss);
    delete[] s;
    s=new char[length+1];
    strcpy(s,ss);
    return *this;
}

LytString LytString::operator=(const LytString& o)
{
    length=o.length;
    delete[] s;
    s=new char[length+1];
    strcpy(s,o.s);
    return *this;
}

ostream& operator<<(ostream& p,const LytString& o)
{
    p<<o.s;
    return p;
}

istream& operator>>(istream& p,LytString& o)
{
    char x[1000];
    p>>x;
    o=x;
    return p;
}

bool LytString::operator==(const LytString& o)const
{
    if (strcmp(s,o.s)==0)
      {return true;}
    else {return false;}
}

bool LytString::operator!=(const LytString& o)const
{
    if (strcmp(s,o.s)!=0)
      {return true;}
    else {return false;}
}

bool LytString::operator>(const LytString& o)const
{
    if (strcmp(s,o.s)>0)
      {return true;}
    else {return false;}
}

bool LytString::operator<(const LytString& o)const
{
    if (strcmp(s,o.s)<0)
      {return true;}
    else {return false;}
}

bool LytString::operator>=(const LytString& o)const
{
    if (strcmp(s,o.s)>=0)
      {return true;}
    else {return false;}
}

bool LytString::operator<=(const LytString& o)const
{
    if (strcmp(s,o.s)<=0)
      {return true;}
    else {return false;}
}

LytString LytString::operator+(const LytString& o)const
{
    char* s1=new char[length+o.length+1];
    strcpy(s1,s);
    strcat(s1,o.s);
    s1[length+o.length]=0;
    LytString SS=s1;
    delete[] s1;
    return SS;
}

LytString LytString::operator+(const char* ss)const
{
    LytString S1=s;
    LytString S2=ss;
    return S1+S2;
}

LytString operator+(const char*ss , const LytString& o)////////为什么不可以加const 
{
    LytString S1=ss;
    return S1+o;
}

LytString LytString::sub(const int Index,const int Count)const
{
	if (Index<0 || Index>length-1 || Count<1 || Count>length-Index)
    {
		LytString SS="";
        return SS;
    }
    else
    {
        char* Result=new char[Count+1];
        int wei=Index;
        for(int i=0; i<=Count-1; i++)
        {
            Result[i]=s[wei];
            wei=wei+1;
        }
        Result[Count]=0;
		LytString SS=Result;
		delete[] Result;
		return SS;
    }
}

void LytString::insert(const int index,const LytString o)
{
    if (index>=0 && index<=length-1 && strcmp(o.s,"")!=0) //有问题，可以在末位插入
    {
        LytString S1=s;
        LytString S2=S1.sub(0,index);
        LytString S3=S1.sub(index,length-index);
        S1=S2+o+S3;
        delete[] s;
        length=length+o.length;
        s=new char[length+1];
        strcpy(s,S1.s);
    }
}

void LytString::Delete(int index,int count)
{
    if (index>=0 && index<=length-1 && count>0 && count<=length-index)
    {
        LytString S1=s;
        LytString S2=S1.sub(0,index);
        LytString S3=S1.sub(index+count,length-count-index);
        S1=S2+S3;
        delete[] s;
        length=length-count;
        s=new char[length+1];
        strcpy(s,S1.s);
    }
}

LytString LytString::toupper()const
{
    char* s1=new char[length+1];
    strcpy(s1,s);
    for (int i=0; i<=length-1; i++)
    {
        if (s1[i]>='a' && s1[i]<='z')
          {s1[i]=s1[i]-32;}
    }
    s1[length]=0;
    LytString SS=s1;
    delete[] s1;
    return SS;
}

LytString LytString::tolower()const
{
    char* s1=new char[length+1];
    strcpy(s1,s);
    for(int i=0; i<=length-1; i++)
    {
        if (s1[i]>='A' && s1[i]<='Z')
          {s1[i]=s1[i]+32;}
    }
    s1[length]=0;
    LytString SS=s1;
    delete[] s1;
    return SS;
}

LytString LytString::left(const int count)const
{
    if (count<=0 || count>length)
    {
        LytString SS="";
        return SS;
    }
    else 
    {
        LytString S1=s;
        return S1.sub(0,count);
    }
}

LytString LytString::right(const int count)const
{
    if (count<=0 && count>length)
    {
        LytString SS="";
        return SS;
    }
    else 
    {
        LytString S1=s;
        return S1.sub(length-count,count);
    }
}

LytString LytString::trimleft()const
{
    if (s[0]==' ')
    {
        int wei=0;
        bool pd=true;
        while (pd==true && wei<=length-1)
        {
            if (s[wei]!=' ')
              {pd=false;}
            else {wei=wei+1;}
        }  
        if (pd==false)
        {
            LytString S1=s;
            return S1.sub(wei,length-wei);
        }
        else 
        {
            LytString S1="";
            return S1;
        }
    }
    else
    {
        LytString S1=s;
        return S1;
    }
}

LytString LytString::trimright()const
{
    if(s[length-1]==' ')
    {
        int wei=length-1;
        bool pd=true;
        LytString S1=s;
        while (pd==true && wei>=0)
        {
            if (s[wei]!=' ')
              {pd=false;}
            else {wei=wei-1;}
        }
        if (pd==false)
        {
            LytString S1=s;
            return S1.sub(0,wei+1);
        }
        else
        {
            LytString S1="";
            return S1;
        }
    }
    else
    {
        LytString S1=s;
        return S1;
    }
}

LytString LytString::trim()const
{
    return trimleft().trimright();
}

int LytString::pos(const LytString& o)const
{
    if (strcmp(s,o.s)==0)
    {
        return 0;
    }
    else if(strcmp(o.s,"")==0)
    {
        return -1;
    }
    else
    {
        int i=-1,wei1=0,wei2=0;
        while (wei1<=length-1 && wei2<=o.length-1)
        {
            if (s[wei1]==o.s[wei2])
            {
                int j=wei1;
                bool pd=true;
                wei1=wei1+1;
                wei2=wei2+1;
                while (pd==true && wei1<=length-1 && wei2<=o.length-1)
                {
                    if (s[wei1]!=o.s[wei2])
                      {pd=false;}
                    else 
                    {
                        wei1=wei1+1;
                        wei2=wei2+1;
                    }    
                }
                if (pd==true) 
                {
                    i=j;
                    break;
                }
                else 
                {
                    wei2=0;
                    wei1=j+1;;
                }
            }
            else {wei1=wei1+1;}
        }
        return i;
    }
}

int LytString::replace(const LytString& find , const LytString& result)
{
    LytString S1=s;
    int k=S1.pos(find);
    if (strcmp(find.s,"")==0 || k==-1)
      {return 0;}
    else if (strcmp(find.s,result.s)==0)
      {return 1;}
    else if (strcmp(s,find.s)==0)
    {
        delete[] s;
        length=result.length;
        s=new char[length+1];
        strcpy(s,result.s);
        return 1;
    }
    else
    {
        LytString S2;
        int i=0;
        while(k!=-1)
        {
            S1.Delete(k,find.length);
            S1.insert(k,result);
            i=i+1;
            S2=S2+S1.sub(0,k+result.length+1);
            S1=S1.sub(k+result.length+1,S1.length-k-result.length-1);
            k=S1.pos(find);
        }
        delete[] s;
        length=S2.length;
        s=new char[length+1];
        strcpy(s,S2.s);
        return i;
    }
}

LytWString LytString::ToWString()const
{
	wchar_t* Buffer=new wchar_t[length+1];
	mbstowcs(Buffer, s, length);
	Buffer[length]=0;
	LytWString Result=Buffer;
	delete[] Buffer;
	return Result;
}

int StrToInt(const LytString& o)
{
    int result=0;
	result=atoi(o.s);
    return result;
}

LytString IntToStr(const int i)
{
    char* s1=new char[100];
    int d1=0,t=i;
    while (t>=1)
    {
        d1=d1+1;
        t=t/10;
    }   
    t=i; 
    for (int j=d1-1; j>=0; j--)
    {
        s1[j]=t%10+48;
        t=t/10;
    }
	s1[d1]=0;
    LytString SS=s1;
    delete[] s1;
    return SS;
}

LytString DoubleToStr(const double Number)
{
	if (Number>=1)
	{
		int Integer=(int)Number;
		int Double=(int)((Number)*1000)%1000;
		int Size=3;
		if (Double>=1)
			Size=Size-1;
		if (Double>=10)
			Size=Size-1;
		if (Double>=100)
			Size=Size-1;
		LytString Temp=IntToStr(Integer)+".";
		for (int i=1; i<=Size; i++)
			Temp=Temp+"0";
		Temp=Temp+IntToStr(Double);
		return Temp;
	}
	else
	{
		int Double=(int)((Number)*1000)%1000;
		int Size=3;
		if (Double>=1)
			Size=Size-1;
		if (Double>=10)
			Size=Size-1;
		if (Double>=100)
			Size=Size-1;
		LytString Temp="0.";
		for (int i=1; i<=Size; i++)
			Temp=Temp+"0";
		Temp=Temp+IntToStr(Double);
		return Temp;
	}
}

double StrToDouble(const LytString& o)
{
    double result;
    result=atof(o.s);
    return result;
}

int LytString::Size()const
{
	return length;
}

char& LytString::operator [](int Index)
{
	return s[Index];
}

const char* LytString::Buffer()const
{
	return s;
}

LytString::~LytString()
{
    delete[] s;
}
