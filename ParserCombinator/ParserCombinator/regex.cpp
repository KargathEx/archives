#include <iostream>
#include "ParserCombinator.h"

int main()
{
	using namespace RiRi;

	ParserCombinator<ParserCombinator<bool>> unit, repeat, connection, branch, regex;

	unit = Token([](char ch) { return ch != '(' && ch != ')' && ch != '|' && ch != '+'; }) >>
		[](char ch)
	{
		ParserCombinator<bool> match;
		match = (Token(ch) >> Return(true));
		return match;
	} | '\\'_T + Token(AnyChar{}) >> [](Placeholder, char ch)
	{
		ParserCombinator<bool> match;
		match = (Token(ch) >> Return(true));
		return match;
	} | '('_T + regex + ')'_T >> [](Placeholder, const ParserCombinator<bool>& unit, Placeholder)
	{
		return unit;
	};

	repeat = unit + '*'_T >> [](const ParserCombinator<bool>& unit, Placeholder)
	{
		ParserCombinator<bool> match;
		match = match + unit >> Return(true) | Epsilon(true);
		return match;
	} | unit;

	connection = connection + repeat >>
		[](const ParserCombinator<bool>& part_1, const ParserCombinator<bool>& part_2)
	{
		ParserCombinator<bool> match;
		match = part_1 + part_2 >> Return(true);
		return match;
	} | repeat;

	branch = branch + '|'_T + connection >>
		[](const ParserCombinator<bool>& part_1, Placeholder, const ParserCombinator<bool>& part_2)
	{
		ParserCombinator<bool> match;
		match = part_1 | part_2;
		return match;
	} | connection;

	regex = branch >> [](const ParserCombinator<bool>& match)
	{
		ParserCombinator<bool> regex(match);
		regex.ReturnDefaultWhenFail(true);	//return false
		return regex;
	};

	std::string pattern = "(a|b)*abb";
	std::string match = "babaababb";

	std::cout << regex(pattern)(match) << std::endl;

	return 0;
}