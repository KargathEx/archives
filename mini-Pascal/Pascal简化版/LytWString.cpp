#include "LytWString.h"

LytWString::LytWString()
{
	Length=0;
	String=new wchar_t[Length+1];
	wcscpy(String,L"");
}

LytWString::LytWString(const wchar_t& Temp)
{
	Length=1;
	String=new wchar_t[Length+1];
	String[0]=Temp;
	String[1]=0;
}

LytWString::LytWString(const wchar_t* Temp)
{
	Length=(int)wcslen(Temp);
	String=new wchar_t[Length+1];
	wcscpy(String, Temp);
}

LytWString::LytWString(const LytWString& Temp)
{
	Length=Temp.Length;
	String=new wchar_t[Length+1];
	wcscpy(String, Temp.String);
}

LytWString LytWString::operator=(const wchar_t* Temp)
{
	Length=(int)wcslen(Temp);
	delete[] String;
	String=new wchar_t[Length+1];
	wcscpy(String, Temp);
	return *this;
}

LytWString LytWString::operator=(const LytWString& Temp)
{
	Length=Temp.Length;
	delete[] String;
	String=new wchar_t[Length+1];
	wcscpy(String, Temp.String);
	return *this;
}

bool LytWString::operator==(const LytWString& Temp)const
{
	if (wcscmp(String, Temp.String)==0)
		return true;
	else return false;
}

bool LytWString::operator!=(const LytWString& Temp)const
{
	if (wcscmp(String, Temp.String)!=0)
		return true;
	else return false;
}

bool LytWString::operator>(const LytWString& Temp)const
{
	if (wcscmp(String, Temp.String)>0)
		return true;
	else return false;
}

bool LytWString::operator<(const LytWString& Temp)const
{
	if (wcscmp(String, Temp.String)<0)
		return true;
	else return false;
}

bool LytWString::operator>=(const LytWString& Temp)const
{
	if (wcscmp(String, Temp.String)>=0)
		return true;
	else return false;
}

bool LytWString::operator<=(const LytWString& Temp)const
{
	if (wcscmp(String, Temp.String)<=0)
		return true;
	else return false;
}

LytWString LytWString::operator+(const LytWString& Temp)const
{
	wchar_t* s=new wchar_t[Length+Temp.Length+1];
    wcscpy(s,String);
	wcscat(s,Temp.String);
	s[Length+Temp.Length]=0;
    LytWString Result=s;
    delete[] s;
    return Result;
}

LytWString LytWString::operator+(const wchar_t* Temp)const
{
    LytWString TempLeft=String;
    LytWString TempRight=Temp;
    return TempLeft+TempRight;
}

LytWString operator+(const wchar_t* TempLeft, const LytWString& TempRight)///////////为什么不可以加const
{
    LytWString Temp=TempLeft;
    return Temp+TempRight;
}

LytWString LytWString::operator++()///////////////////////////////////
{
	String++;
	Length--;
	return *this;
}

LytWString LytWString::operator++(int)
{
	LytWString Temp=*this;
	++(*this);
	return Temp;
}

LytWString LytWString::operator--()
{
	String--;
	Length++;
	return *this;
}

LytWString LytWString::operator--(int)
{
	LytWString Temp=*this;
	--(*this);
	return Temp;
}

wostream& operator<<(wostream& Output, const LytWString& Temp)
{
	Output<<Temp.String;
	return Output;
}

wistream& operator>>(wistream& Input, LytWString& Temp)
{
	wchar_t Buffer[1000];
	Input>>Buffer;
	Temp=Buffer;
	return Input;
} 

LytWString Wchar_tToLytWString(const wchar_t& Temp)
{
	wchar_t Input[2];
	Input[0]=Temp;
	Input[1]=L'\0';
	LytWString Result=Input;
	return Result;
}

LytWString LytWString::Sub(const int Index, const int Count)const
{
	if (Index<0 || Index>Length-1 || Count<1 || Count>Length-Index)
    {
		LytWString Temp=L"";
        return Temp;
    }
    else
    {
        wchar_t* Result=new wchar_t[Count+1];
        int Position=Index;
        for(int i=0; i<=Count-1; i++)
        {
            Result[i]=String[Position];
            ++Position;
        }
        Result[Count]=0;    //末尾的到底是0还是'\0'啊? 搜NTCTS 搜到了其他东西..
		LytWString Temp=Result;
		delete[] Result;
		return Temp;
    }
}

void LytWString::Insert(const int Index, const LytWString Temp)
{
	if (Index>=0 && Index<=Length-1 && wcscmp(Temp.String,L"")!=0)
    {
        LytWString S1=String;
        LytWString S2=S1.Sub(0,Index);
        LytWString S3=S1.Sub(Index,Length-Index);
        S1=S2+Temp+S3;
        delete[] String;
		Length=Length+Temp.Length;
        String=new wchar_t[Length+1];
		wcscpy(String,S1.String);
    }
}

void LytWString::Delete(int Index, int Count)
{
    if (Index>=0 && Index<=Length-1 && Count>0 && Count<=Length-Index)
    {
        LytWString S1=String;
        LytWString S2=S1.Sub(0,Index);
        LytWString S3=S1.Sub(Index+Count,Length-Count-Index);
        S1=S2+S3;
        delete[] String;
        Length=Length-Count;
        String=new wchar_t[Length+1];
		wcscpy(String,S1.String);
    }
}

LytWString LytWString::ToUpper()const
{
    wchar_t* Result=new wchar_t[Length+1];
    wcscpy(Result, String);
    for (int i=0; i<=Length-1; i++)
    {
		if (Result[i]>='a' && Result[i]<='z')
			Result[i]=Result[i]-32;
    }
    Result[Length]=0;
    LytWString Temp=Result;
    delete[] Result;
    return Temp;
}

LytWString LytWString::ToLower()const
{
    wchar_t* Result=new wchar_t[Length+1];
    wcscpy(Result, String);
    for(int i=0; i<=Length-1; i++)
    {
        if (Result[i]>='A' && Result[i]<='Z')
			Result[i]=Result[i]+32;
    }
    Result[Length]=0;
    LytWString Temp=Result;
    delete[] Result;
    return Temp;
}

LytWString LytWString::Left(const int Count)const
{
    if (Count<=0 || Count>Length)
    {
        LytWString Temp=L"";
        return Temp;
    }
    else 
    {
        LytWString Temp=String;
		return Temp.Sub(0,Count);
    }
}

LytWString LytWString::Right(const int Count)const
{
    if (Count<=0 && Count>Length)
    {
        LytWString Temp=L"";
        return Temp;
    }
    else 
    {
        LytWString Temp=String;
        return Temp.Sub(Length-Count,Count);
    }
}

LytWString LytWString::TrimLeft()const
{
    if (String[0]==L' ')
    {
        int Position=0;
        bool Judge=true;
        while (Judge==true && Position<=Length-1)
        {
			if (String[Position]!=L' ')
				Judge=false;
            else Position++;
        }  
        if (Judge==false)
        {
            LytWString Temp=String;
			return Temp.Sub(Position, Length-Position);
        }
        else 
        {
            LytWString Temp=L"";
            return Temp;
        }
    }
    else
    {
        LytWString Temp=String;
        return Temp;
    }
}

LytWString LytWString::TrimRight()const
{
    if(String[Length-1]==L' ')
    {
        int Position=Length-1;
        bool Judge=true;
        LytWString Temp=String;
        while (Judge==true && Position>=0)
        {
            if (String[Position]!=L' ')
				Judge=false;
            else Position--;
        }
        if (Judge==false)
        {
            LytWString Temp=String;
			return Temp.Sub(0, Position+1);
        }
        else
        {
            LytWString Temp=L"";
            return Temp;
        }
    }
    else
    {
        LytWString Temp=String;
        return Temp;
    }
}

LytWString LytWString::Trim()const
{
    return TrimLeft().TrimRight();
}

int LytWString::Pos(const LytWString& Temp)const
{
	if (wcscmp(String, Temp.String)==0)
		return 0;
	else if(wcscmp(Temp.String, L"")==0)
		return -1;
    else
    {
        int i=-1,wei1=0,wei2=0;
		while (wei1<=Length-1 && wei2<=Temp.Length-1)
        {
			if (String[wei1]==Temp.String[wei2])
            {
                int j=wei1;
                bool pd=true;
                wei1=wei1+1;
                wei2=wei2+1;
				while (pd==true && wei1<=Length-1 && wei2<=Temp.Length-1)
                {
					if (String[wei1]!=Temp.String[wei2])
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

int LytWString::Replace(const LytWString& Find , const LytWString& Result)
{
    LytWString S1=String;
	int k=S1.Pos(Find);
	if (wcscmp(Find.String, L"")==0 || k==-1)
		return 0;
	else if (wcscmp(Find.String, Result.String)==0)
		return 1;
	else if (wcscmp(String, Find.String)==0)
    {
        delete[] String;
		Length=Result.Length;
        String=new wchar_t[Length+1];
		wcscpy(String, Result.String);
        return 1;
    }
    else
    {
        LytWString S2;
        int i=0;
        while(k!=-1)
        {
            S1.Delete(k,Find.Length);
            S1.Insert(k,Result);
            i=i+1;
            S2=S2+S1.Sub(0,k+Result.Length+1);
            S1=S1.Sub(k+Result.Length+1,S1.Length-k-Result.Length-1);
            k=S1.Pos(Find);
        }
        delete[] String;
        Length=S2.Length;
        String=new wchar_t[Length+1];
		wcscpy(String, S2.String);
        return i;
    }
}

int LytWString::Size()const
{
	return Length;
}

wchar_t& LytWString::operator[](int Index)
{
	return String[Index];
}

const wchar_t* LytWString::Buffer()const
{
	return String;
}

int LytWString::ToInt()const
{
	return _wtoi(String);
}

double LytWString::ToDouble()const
{
	return _wtof(String);
}

LytWString IntToWString(const int Value, const int Length)
{
	wchar_t* Temp=new wchar_t[Length+1];
	_itow_s(Value, Temp, Length, 10);
	Temp[Length]=0;
	LytWString Result=Temp;
	delete[] Temp;
	return Result;
}

LytWString DoubleToWString(const double Value, const int Length)
{
	char* Temp=new char[2*Length+1];
	_gcvt(Value, Length, Temp);
	Temp[2*Length]=0;
	wchar_t* Buffer=new wchar_t[Length+1];
	mbstowcs(Buffer, Temp, Length);
	Buffer[Length]=0;
	LytWString Result=Buffer;
	delete[] Buffer;
	delete[] Temp;
	return Result;
}

LytWString::~LytWString()
{
	delete[] String;
}
