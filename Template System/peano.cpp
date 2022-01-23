
//from https://blog.csdn.net/cyendra/article/details/45586461
#include <iostream>

namespace raven {

	/*************************************************
						Peano
	*************************************************/
	// Undefined
	struct Undefined { using Pred = Undefined; };

	// Zero
	struct Zero { using Pred = Zero; };

	// Succ
	template <class N> struct Succ;

	template <>struct Succ < Zero > { using Pred = Zero; };

	template <>struct Succ < Undefined > { using Pred = Undefined; };
	template <class N> struct Succ < Succ<N> > { using Pred = Succ<N>; };
	// Pred
	template <class N>struct Pred;

	template<>struct Pred < Zero > { using Value = Zero; };

	template<>struct Pred < Undefined > { using Value = Undefined; };

	template<class N>struct Pred < Succ<N> > { using Value = N; };

	/*************************************************
						Boolean Logic
	*************************************************/

	// True False
	struct True {
		const static bool ToBool = true;
	};
	struct False {
		const static bool ToBool = false;
	};

	// And
	template <class B1, class B2>
	struct And {
		using Value = False;
	};

	template <>
	struct And < True, True > {
		using Value = True;
	};

	// Or
	template <class B1, class B2>
	struct Or {
		using Value = True;
	};

	template <>
	struct Or < False, False > {
		using Value = False;
	};

	// Not
	template <class B>
	struct Not {
		using Value = False;
	};

	template <>
	struct Not < False > {
		using Value = True;
	};

	/*************************************************
	Operator
	*************************************************/

	// Equal
	template <class T1, class T2>
	struct Equal {
		using Value = False;
	};

	template <class T>
	struct Equal < T, T > {
		using Value = True;
	};

	// Less Than
	template <class N1, class N2>
	struct LessThan {
		using Value = typename LessThan<typename N1::Pred, typename N2::Pred>::Value;
	};

	template <class N>
	struct LessThan < Zero, N > {
		using Value = True;
	};

	template <class N>
	struct LessThan < N, Zero > {
		using Value = False;
	};

	template <>
	struct LessThan < Zero, Zero > {
		using Value = False;
	};

	// Less Equal
	template <class N1, class N2>
	struct LessEqual {
		using Value = typename Or<typename Equal<N1, N2>::Value, typename LessThan<N1, N2>::Value>::Value;
	};

	// Greater Than
	template <class N1, class N2>
	struct GreaterThan {
		using Value = typename GreaterThan<typename N1::Pred, typename N2::Pred>::Value;
	};

	template <class N>
	struct GreaterThan < Zero, N > {
		using Value = False;
	};

	template <class N>
	struct GreaterThan < N, Zero > {
		using Value = True;
	};

	template <>
	struct GreaterThan < Zero, Zero > {
		using Value = False;
	};

	// Greater Equal
	template <class N1, class N2>
	struct GreaterEqual {
		using Value = typename Or<typename Equal<N1, N2>::Value, typename GreaterThan<N1, N2>::Value>::Value;
	};

	/*************************************************
	Statement
	*************************************************/

	// If
	template <class B, class S1, class S2>
	struct If;

	template <class S1, class S2>
	struct If < True, S1, S2 > {
		using Value = S1;
	};

	template <class S1, class S2>
	struct If < False, S1, S2 > {
		using Value = S2;
	};

	/*************************************************
						Calculate
	*************************************************/

	// Add
	template <class N1, class N2>
	struct Add;

	template <class N>
	struct Add < Zero, N > {
		using Value = N;
	};

	template <class N1, class N2>
	struct Add < Succ<N1>, N2 > {
		using Value = typename Add<N1, Succ<N2>>::Value;
	};

	// Mul
	template <class N1, class N2>
	struct Mul;

	template <class N>
	struct Mul < Zero, N > {
		using Value = Zero;
	};

	template <class N1, class N2>
	struct Mul < Succ<N1>, N2 > {
		using Value = typename Add<N2, typename Mul<N1, N2>::Value>::Value;
	};

	// Sub
	template <class N1, class N2>
	struct Sub {
		using Value = typename Sub<typename N1::Pred, typename N2::Pred>::Value;
	};

	template <class N>
	struct Sub < N, Zero > {
		using Value = N;
	};

	template <class N>
	struct Sub < Zero, N > {
		using Value = Zero;
	};

	template <>
	struct Sub < Zero, Zero > {
		using Value = Zero;
	};

	// Div
	template <class N1, class N2>
	struct Div {
		using Value = typename If<typename GreaterEqual<N1, N2>::Value, Succ<typename Div<typename Sub<N1, N2>::Value, N2>::Value>, Zero>::Value;
		using Rem = typename If<typename GreaterEqual<N1, N2>::Value, typename Div<typename Sub<N1, N2>::Value, N2>::Rem, N1>::Value;
	};

	template <class N>
	struct Div < Zero, N > {
		using Value = Zero;
		using Rem = Zero;
	};

	template <class N>
	struct Div < N, Zero > {
		using Value = Undefined;
		using Rem = Undefined;
	};

	// Number
	using i0 = Zero;
	using i1 = Succ < Zero >;
	using i2 = Succ < i1 >;
	using i3 = Succ < i2 >;
	using i4 = Succ < i3 >;
	using i5 = Succ < i4 >;
	using i6 = Succ < i5 >;
	using i7 = Succ < i6 >;
	using i8 = Succ < i7 >;
	using i9 = Succ < i8 >;
	using i10 = Succ < i9 >;

	struct Nil {
		using Length = Zero;
	};

	template<class First, class Rest>
	struct Pair {
		using Head = First;
		using Tail = Rest;
		using Length = Succ < typename Rest::Length >;
	};

	// Pair Head
	template <class Pair>
	struct Head;

	template <class H, class T>
	struct Head < Pair<H, T> > {
		using Value = H;
	};

	// Pair Tail
	template <class Pair>
	struct Tail;

	template <class H, class T>
	struct Tail < Pair<H, T> > {
		using Value = T;
	};

	// List
	template <class Head, class... Tail>
	struct List {
		using Value = Pair < Head, typename List<Tail...>::Value >;
		using Length = typename Value::Length;
	};

	template<class Head>
	struct List < Head > {
		using Value = Pair < Head, Nil >;
		using Length = i1;
	};

	// ListToDecimal
	template <class N, class List>
	struct ListToDecimal_X;

	template <class N, class H, class T>
	struct ListToDecimal_X < N, Pair<H, T> > {
		using Value = typename ListToDecimal_X<typename Add<typename Mul<i10, N>::Value, H>::Value, T>::Value;
	};

	template <class N>
	struct ListToDecimal_X < N, Nil > {
		using Value = N;
	};

	template <class List>
	struct ListToDecimal {
		using Value = typename ListToDecimal_X<i0, List>::Value;
	};

	template <class List>
	struct Length;

	template <class H, class T>
	struct Length < Pair<H, T> > {
		using Value = Succ < typename Length<T>::Value >;
	};

	template <>
	struct Length < Nil > {
		using Value = Zero;
	};

	// List Take
	template <class N, class List>
	struct Take {
		using Value = Pair < typename List::Head, typename Take<typename N::Pred, typename List::Tail>::Value >;
		using Length = typename Value::Length;
	};

	template <class N>
	struct Take < N, Nil > {
		using Value = Nil;
		using Length = Zero;
	};

	template <class List>
	struct Take < Zero, List > {
		using Value = Nil;
		using Length = Zero;
	};

	// List Nth
	template <class N, class List>
	struct Nth;

	template <class H, class T, class N>
	struct Nth < Succ<N>, Pair<H, T> > {
		using Value = typename Nth<N, T>::Value;
	};

	template <class N>
	struct Nth < N, Nil > {
		using Value = Nil;
	};

	template <class H, class T>
	struct Nth < Zero, Pair<H, T> > {
		using Value = H;
	};

	// List Drop
	template <class N, class List>
	struct Drop;

	template <class N, class H, class T>
	struct Drop < Succ<N>, Pair<H, T> > {
		using Value = typename Drop<N, T>::Value;
	};

	template <class List>
	struct Drop < Zero, List > {
		using Value = List;
	};

	template <class N>
	struct Drop < N, Nil > {
		using Value = Nil;
	};

	// List Concat
	template <class L1, class L2>
	struct Concat;

	template <class H, class T, class List>
	struct Concat < Pair<H, T>, List > {
		using Value = Pair < H, typename Concat<T, List>::Value >;
	};

	template <class List>
	struct Concat < Nil, List > {
		using Value = List;
	};

	/*************************************************
						Lambda
	*************************************************/

	template <template <class A> class F, class L>
	struct Map;

	template <template <class A> class F, class H, class T>
	struct Map < F, Pair<H, T> > {
		using Value = Pair < typename F<H>::Value, typename Map<F, T>::Value >;
	};

	template <template<class A> class F>
	struct Map < F, Nil > {
		using Value = Nil;
	};

	template <template <class A, class B> class F, class I, class L>
	struct FoldLeft {
		using Value = typename FoldLeft<F, typename F<I, typename L::Head>::Value, typename L::Tail>::Value;
	};

	template <template <class A, class B> class F, class I>
	struct FoldLeft < F, I, Nil > {
		using Value = I;
	};

	template <template <class A> class F, class L>
	struct Filter {
		using Value = typename If<typename F<typename L::Head>::Value,
			Pair<typename L::Head, typename Filter<F, typename L::Tail>::Value>,
			typename Filter<F, typename L::Tail>::Value>::Value;
	};

	template <template <class A> class F>
	struct Filter < F, Nil > {
		using Value = Nil;
	};

	/*************************************************
						Function
	*************************************************/

	// Max
	template <class N1, class N2>
	struct Max {
		using Value = typename If<typename GreaterThan<N1, N2>::Value, N1, N2>::Value;
	};

	// Min
	template <class N1, class N2>
	struct Min {
		using Value = typename If<typename LessThan<N1, N2>::Value, N1, N2>::Value;
	};

	// Is_Odd
	template <class N>
	struct Is_Odd {
		using Value = typename Equal<typename Div<N, i2>::Rem, i1>::Value;
	};

	// Is_Even
	template <class N>
	struct Is_Even {
		using Value = typename Equal<typename Div<N, i2>::Rem, i0>::Value;
	};

	// Is_Integer
	template <class N>
	struct Is_Integer {
		using Value = False;
	};

	template <class N>
	struct Is_Integer < Succ<N> > {
		using Value = True;
	};

	template <>
	struct Is_Integer < Zero > {
		using Value = True;
	};

	// Is_Boolean
	template <class B>
	struct Is_Boolean {
		using Value = False;
	};

	template <>
	struct Is_Boolean < True > {
		using Value = True;
	};

	template <>
	struct Is_Boolean < False > {
		using Value = True;
	};

	// Is_Nil
	template <class X>
	struct Is_Nil {
		using Value = False;
	};

	template <>
	struct Is_Nil < Nil > {
		using Value = True;
	};

	// Is_Pair
	template <class P>
	struct Is_Pair {
		using Value = False;
	};
	template <class X, class Y>
	struct Is_Pair < Pair<X, Y> > {
		using Value = True;
	};

	/*************************************************
						Test Tool
	*************************************************/

	// Assert
	template <class E>
	struct Assert {
		inline static void True() { throw 0; }
	};

	template <>
	struct Assert < True > {
		inline static void True() {}
	};

	// Integer
	template <class N>
	struct Integer {
		const static int ToInt = -65536;
	};

	template <>
	struct Integer < Zero > {
		const static int ToInt = 0;
	};

	template <class N>
	struct Integer < Succ<N> > {
		const static int ToInt = Integer<N>::ToInt + 1;
	};

	// Boolean
	template <class B>
	struct Boolean {
		const static bool ToBool = false;
	};

	template <>
	struct Boolean < True > {
		const static bool ToBool = true;
	};

	template <>
	struct Boolean < False > {
		const static bool ToBool = false;
	};

	// Display Integer
	template <class N>
	struct DisplayInteger {
		inline static void Display() {
			std::cout << Integer<N>::ToInt << std::endl;
		}
	};

	// Display Boolean
	template <class B>
	struct DisplayBoolean {
		inline static void Display() {
			if (Boolean<B>::ToBool) std::cout << "true" << std::endl;
			else std::cout << "false" << std::endl;
		}
	};

	// Display List
	template <class P>
	struct DisplayList;

	template <class H, class T>
	struct DisplayList < Pair<H, T> > {
		inline static void Display() {
			if (Is_Integer<H>::Value::ToBool) {
				std::cout << Integer<H>::ToInt << " ";
			}
			else if (Is_Boolean<H>::Value::ToBool) {
				if (Boolean<H>::ToBool) std::cout << "true ";
				else std::cout << "false ";
			}
			DisplayList<T>::Display();
		}
	};

	template <>
	struct DisplayList < Nil > {
		inline static void Display() {
			std::cout << std::endl;
		}
	};
}

// code on codewars:

#include<type_traits>
 struct Peano {};

 struct Zero: Peano {
   static constexpr int value = 0;
 };

 template<class T>
 struct Succ: Peano {
   static constexpr int value = T::value + 1;
 };

 template<int v>
 struct peano {
     using type = Succ<typename peano<v - 1>::type>;
 };

 template<>
 struct peano<0> {
     using type = Zero;
 };

 struct Error {};
 struct NoError : Error {};
 struct Negative : Error {};
 struct Infinity : Error {};

 struct Ordreing {};
 struct EQ: Ordreing {};
 struct GT: Ordreing {};
 struct LT: Ordreing {};

 struct Bool {};
 struct True: Bool {};
 struct False: Bool {};

struct NYI {};

template<class T1, class T2>
struct Add;

template<class T1>
struct Add<T1, Zero> { using type = T1; };

template<class T1, class T2>
struct Add<T1, Succ<T2>> { using type = Succ<typename Add<T1, T2>::type>; };

template<class T1, class T2>
struct Sub;

template<class T1>
struct Sub<T1, Zero> { using type = T1; using error = NoError; };

template<class T2>
struct Sub<Zero, Succ<T2>> { using type = Zero; using error = Negative; };

template<class T1, class T2>
struct Sub<Succ<T1>, Succ<T2>> { using type = typename Sub<T1, T2>::type; using error = typename Sub<T1, T2>::error; };
template<class T1, class T2>
struct Mul;

template<class T1>
struct Mul<T1, Zero> { using type = Zero; };

template<class T1, class T2>
struct Mul<T1, Succ<T2>> { using type = typename Add<typename Mul<T1, T2>::type, T1>::type; };
//以加法的形式实现乘法，好的。

template<class T1, class T2>
struct Compare;

template<>
struct Compare<Zero, Zero> { using type = EQ; };

template<class T1>
struct Compare<T1, Zero> { using type = GT; };

template<class T2>
struct Compare<Zero, T2> { using type = LT; };

template<class T1, class T2>
struct Compare<Succ<T1>, Succ<T2>> { using type = typename Compare<T1, T2>::type; };

template<class T1, class T2>
struct Div;

template<class T1>
struct Div<T1, Zero> { using type = Zero; using error = Infinity; };

template<class T1>
struct Div<Zero, T1> { using type = Zero; using error = NoError; };

template<>
struct Div<Zero, Zero> { using type = Zero; using error = Infinity; };
//因为同样好所以才不得已而定义了一个更好的.

template<class T1, class T2>
struct Div {
    using type = typename std::conditional<std::is_same<
                                                    typename Compare<T1, T2>::type,
                                                    LT>::value
                                        , Zero
//如果左边更小，则，为零。否则选择下面这个分支:
        , typename std::conditional<std::is_same<typename Compare<T1, T2>::type, EQ>::value
        , Succ<Zero>
        //如果两个一样就返回1{Succ<Zero>},否则{又一个否则}，返回1-2剩下的，这个会递归展开，辗转相除，直到，被除数比除数小，就是1那种，然后返回？Zero.
        //如果两者可以相同则返回0了。那么，展开的过程中，每次的除法，结果放在展开链的哪里保存呢？中间每次遇到一个Succ<Zero>都会被如何处理？
        //不想了，找东西看看吧。
        //insight展开的太难受了，放弃...
        , Succ<typename Div<typename Sub<T1, T2>::type, T2>::type>
        >::type
    >::type;
};

template<class T>
struct Even;

template<>
struct Even<Zero> { using type = True; };

template<>
struct Even<Succ<Zero>> { using type = False; };

template<class T>
struct Even<Succ<Succ<T>>> { using type = typename Even<T>::type; };

template<class T>
struct Odd;

template<>
struct Odd<Zero> { using type = False; };

template<>
struct Odd<Succ<Zero>> { using type = True; };

template<class T>
struct Odd<Succ<Succ<T>>> { using type = typename Odd<T>::type; };
int main()
{
    std::cout << std::is_same<
        Div<peano<10>::type, peano<2>::type>::type,
        peano<5>::type
    >::value;
}
