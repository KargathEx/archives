//跟这个差不多，不过后者更全 https://blog.csdn.net/huanghongxun/article/details/85065406
//顺便，lisp和haskell那一套机会忘光了，改天再来看吧，挂起。
#include<iostream>
namespace mystd {
	//编译期的常量，用来表示运算结果
	template <typename T, T v>
	struct integral_constant {
		static constexpr T value = v;
		typedef T value_type;
		constexpr operator T() const noexcept { return value; }
		constexpr T operator()() const noexcept { return value; }
	};

	//变量模板？
	template <int v>
	using integer_constant = integral_constant<int, v>;
	
	//实现列表结构 以下两个
	//只是定义了一个空结构体，有什么用？哦，以此类型作为值吗。
	//
	template <typename T, T... values>
	struct integral_sequence { typedef T value_type; };

	//又是表达式模板，看不懂... 这个是？
	//这样可以以 integer_sequence<1, 2, 3, 4, 5>  表示[1, 2, 3, 4, 5] 对应的语法结构其实就那么点，剩下的自己搓就行了。


	//这个是? 3) A non-type template parameter pack with an optional name.
	template <int ... values>
	using integer_sequence = integral_sequence<int, values...>;

	//求长
	template<typename T, typename integral_sequence>
	struct mylength {};

	template<typename T, T ...data>
	struct mylength<integral_sequence<T, data...>, T> :integral_constant<size_t, sizeof...(data)> {};

	template<typename T>
	using length = mylength<T, typename T::value_type>;

	//push_front
	template<typename T, T data, typename Q>
	struct push_front_t {};

	template<typename T, T data, T ...datas>
	struct push_front_t<T, data, integral_sequence<T, datas...>> { using type = integral_sequence<T, data, datas...>; };

	template<int v, typename T>
	using push_front = typename push_front_t<int, v, T>::type;

	//push_back
	template<typename T, T data, typename Q>
	struct push_back_t {};

	template<typename T, T data, T ...datas>
	struct push_back_t<T, data, integral_sequence<T, datas...>> { using type = integral_sequence<T, datas..., data>; };

	template<int v, typename T>
	using push_back = typename push_back_t<int, v, T>::type;


	//print
	template <typename integer_sequence>
	struct print { };

	template<int start, int ...values>
	struct print<integer_sequence<start, values...>> {
		static void print_integer() {
			std::cout << start << ' ';
			print<integer_sequence<values...>>::print_integer();
		}
	};
	template<>
	struct print<integer_sequence<>> {
		static void print_integer() { ; }
	};

	//empty
	template<typename T>
	struct empty;
	template<>
	struct empty<integer_sequence<>> :std::true_type {};

	template<int ...value>
	struct empty<integer_sequence<value...>> :std::false_type {};


	//条件

	//这个ocnditional现在已经是标准库的一部份了吧，还有必要自己定义吗？
	template<bool con, typename T, typename F>
	struct conditional {};

	template<typename T, typename F>
	struct conditional<true, T, F> { using type = T; };

	template<typename T, typename F>
	struct conditional<false, T, F> { using type = F; };


	//map  大部分只是把两者翻过来了而已
	template<template<int>typename Mapper, typename integer_sequence>
	struct map_t;

	template<template<int>typename Mapper>
	struct map_t<Mapper, integer_sequence<>> {
		using type = integer_sequence<>;
	};

	template<template<int>typename Mapper, int head, int ...tails>
	struct map_t<Mapper, integer_sequence<head, tails...>> {
		using type = push_front<Mapper<head>::value, typename map_t<Mapper, integer_sequence<tails...>>::type>;
	};

	template<template<int>typename Mapper, typename T>
	using map = typename map_t<Mapper, T>::type;

	template<int i>
	struct increment { static constexpr int value = i + 1; };


	//filter

	//filter f[] = []
	//filter f(x: [] ) = if (f x)[x] else[]
	//filter f(x:xs) = if (f x)[x] ++(filter f xs) else (filter f xs)
	template<template<int, int>typename Mapper, int compare, typename integer_sequence>
	struct filter;

	template<template<int, int>typename Mapper, int compare>
	struct filter<Mapper, compare, integer_sequence<>> { using type = integer_sequence<>; };


	template<template<int, int>typename Mapper, int compare, int head, int ...tails>
	struct filter<Mapper, compare, integer_sequence<head, tails...>> {
		using type = typename conditional < Mapper<head, compare>::value,
			push_front<head, typename filter<Mapper, compare, integer_sequence<tails...>>::type>,
			typename filter<Mapper, compare, integer_sequence<tails...>>::type>::type;
	};

	template<int i, int j>
	struct big {
		static constexpr bool value = (i > j);
	};
	template<int i, int j>
	struct low {
		static constexpr bool value = (i <= j);
	};


	//CONCAT
	template<typename T, typename Q>
	struct concat;

	template<typename T>
	struct concat<T, integer_sequence<>> {
		using type = T;

	};
	template<typename T, int head, int ...tail>
	struct concat<T, integer_sequence<head, tail...>> {
		using type = typename concat<push_back<head, T>, integer_sequence<tail...>>::type;

	};



	template<typename integer_sequence>
	struct quick_sort {};
	template<>
	struct quick_sort<integer_sequence<>> {
		using type = integer_sequence<>
			;
	};

	template<int head, int ...tails>
	struct quick_sort<integer_sequence<head, tails...> > {
		using type = typename concat<typename quick_sort<typename filter<big, head, integer_sequence<tails...>>::type>::type,
			typename concat<integer_sequence<head>,
			typename quick_sort<typename filter<low, head, integer_sequence<tails...>>::type>::type>::type>::type;
	};
}

using namespace std;
int main() {
	//print<filter<big,4,integer_sequence<1,2,3,4,5,6,7,8>>::type>::print_integer();
	//print<quick_sort<integer_sequence<3, 434, 45, 3, 5, 465645, 3, 245, 534, 9>>::type>::print_integer();

	std::cout << std::endl;
	vector<vector<char>> res(9, vector<char>(9, '.'));
	std::cout << res << std::endl;
}

//来源 : https://blog.csdn.net/weixin_39057744/article/details/89046257

//有了，constexpr 函数，模板元编程没意义了。
//template <typename T>
//constexpr T add(T a, T b) { return a + b; }