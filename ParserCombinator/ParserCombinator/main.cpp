#include <iostream>
#include "ParserCombinator.h"
//那个站已经废了，代码也没处下载，以后说不定连文章都没了。
//赶紧存一下丢到“看过的代码”里。
//就当编程经验了。
//https://zhuanlan.zhihu.com/p/25411428
// https://zhuanlan.zhihu.com/p/25411428#:~:text=AdditionTypes%E7%9A%84%E5%AE%9E%E7%8E%B0%E5%A4%A7%E8%87%B4%E5%A6%82%E4%B8%8B%3A
//评论区链接很好。
int main()
{
	using namespace RiRi;

	ParserCombinator<int> Additive, Multitive, Primary, Number, Decimal;

	//Decimal := '0' | ... | '9'
	Decimal = ('0'_T | '1'_T | '2'_T | '3'_T | '4'_T | '5'_T | '6'_T | '7'_T | '8'_T | '9'_T) >>
		[](char ch)
	{
		return ch - '0';
	};

	//Number := Number Decimal | Decimal
	Number = (Number + Decimal >>
		[](int Number, int Decimal)
		{
			return Number * 10 + Decimal;
		}) | Decimal;

	//Primary := '(' Additive ')' | Number
	Primary = ('('_T + Additive + ')'_T >>
		[](Placeholder, int Additive, Placeholder)
		{
			return Additive;
		}) | Number;

	//Multitive := Multitive '*' Primary | Multitive '/' Primary | Primary
	Multitive = (Multitive + '*'_T + Primary >>
		[](int Multitive, Placeholder, int Primary)
		{
			return Multitive * Primary;
		}) | (Multitive + '/'_T + Primary >>
			[](int Multitive, Placeholder, int Primary)
			{
				return Multitive / Primary;
			}) | Primary;

		//Additive := Additive '+' Multitive | Additive '-' Multitive | Multitive
		Additive = (Additive + '+'_T + Multitive >>
			[](int Additive, Placeholder, int Multitive)
			{
				return Additive + Multitive;
			}) | (Additive + '-'_T + Multitive >>
				[](int Additive, Placeholder, int Multitive)
				{
					return Additive - Multitive;
				}) | Multitive;

			std::cout << Additive("(1+2)*3") << std::endl;

			return 0;
}