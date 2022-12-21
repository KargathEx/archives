最简单的例子：
```cpp

namespace mingy
{
	template<typename T>
	class UP
	{
		T* data;
	public:
		UP(T* data)
			: data(data)
		{}
		~UP()
		{
			delete data;
		}
		T* operator->() { return data; }
		T& operator*() { return *data; }
		T* release()
		{
			T* result = nullptr;
			std::swap(result, data);
			return result;
		}
		// So it can be used in conditional expression
		operator bool() { return data; }
	};
}
```
看起来没有遵循三原则，因为这里有dtor却没有cptor和operator=.
```cpp
int test1()
{
    ThorsAnvil::UP   sp1<int>(new int(5));
    ThorsAnvil::UP   sp2<int>(sp1);  // copy construction}
```
自动生成的复制构造函数只会复制指针，而指针指向的值是同一个，在main退出的时候会导致指针指向的资源被两次释放而出错。

赋值操作符也有类似的问题，
```cpp
int main()
{
	mingy::Unique_ptr  <int> sp1(new int(5));
	mingy::Unique_ptr   <int>sp2(new int(6));

	sp2 = sp1;
}
```
它不仅会充分释放，还会丢失sp1指向的数据。

下一个问题是由 C++ 倾向于急切地将一种类型转换为另一种类型（如果有一半的机会）引起的。 如果您的类包含一个接受单个参数的构造函数，那么编译器将使用它作为将一种类型转换为另一种类型的方法。
例子：
```cpp
void takeOwner1(ThorsAnvil::UP<int> x)
{}
void takeOwner2(ThorsAnvil::UP<int> const& x)
{}
void takeOwner3(ThorsAnvil::UP<int>&& x)
{}

int main()
{
	int* data = new int(7);

	takeOwner1(data);
	takeOwner2(data);
	takeOwner3(data);
}
```

虽然这里没有函数接受int*作为参数，但是编译器发现它可以通过单参数构造函数将 int* 转换为 ThorsAnvil::UP<int> 类型的对象，并构建临时对象以方便函数的调用。

在智能指针的情况下，它获得在构造函数中传递的对象的所有权，这可能是一个问题，因为临时对象的生命周期是包含语句（除了一些例外，我们将在另一篇文章中介绍）。 作为一个简单的经验法则，您可以考虑以“;”结尾的临时结尾的寿命。

//意思是int会在被初始化成临时值，临时值失效后被析构吗？
//不是，是在该函数退出的时候失效，所以上面的函数调用如果有两个，就会出错。
所以在写单参构造函数的时候要小心。

很明显operator*在解引用空指针的时候会遇到问题：
```cpp
T& operator*()  {return *data;}
```
但不是很明显的是，operator-> 也将导致解引用指针
```cpp
T* operator->() {return data;}
```

这个问题有几个解决方案。 如果数据是空指针，您可以检查数据并抛出异常，或者您可以将其作为使用智能指针的前提条件（即，用户有责任了解或检查数据的状态） 使用这些方法之前的智能指针）。

标准里选择了一个先决条件（一种非常常见的 C++ 实践：不要对所有用户强加开销（为初学者避免问题），而是提供一种机制来检查那些需要检查状态的人 这样做;
因此他们可以选择在需要时而不是每次都支付开销）。 我们可以在这里做同样的事情，但我们没有为用户提供任何机制来检查智能指针的状态。


问题 4：常量正确性
当通过智能指针访问拥有的对象时，我们不会影响智能指针的状态，因此任何基本上返回对象（不更改智能指针的状态）的成员都应标记为 const。
所以应该写成这样
```cpp
T* operator->() const {return data;}
T& operator*()  const {return *data;}
```
问题5，使得bool转换容易一点
当前的operator bool()在需要bool值的地方执行转换。

但是当编译器试图强制几乎匹配的对象时，它也会使用转换运算符。

例如，您现在可以使用 operator== 测试两个 UP，即使 UP<> 类不存在实际的 operator==。 这是因为编译器可以将两个 UP<> 对象转换为 bool 并且可以比较它们。

```cpp

ThorsAnvil::UP<int>    value1(new int(8));
ThorsAnvil::UP<int>    value2(new int(9));

if (value1 == value2) {
	// unfortunately this will print "They match".
	// Because both values are converted to bool (in this case true).
	// Then the test is done.
	std::cout << "They match\n";
}
```
在 C++03 中，使用指向成员的指针有一个令人讨厌的工作。 但在 C++11 中添加了新功能，使转换运算符仅在布尔上下文中触发，否则必须显式调用。
```cpp
explicit operator bool() {return data;}
...
ThorsAnvil::UP<int>    value1(new int(8));
ThorsAnvil::UP<int>    value2(new int(9));

if (value1) { // This is expecting a boolean expression.
    std::cout << "Not nullptr\n";
}

if (static_cast<bool>(value1) == static_cast<bool>(value2)) { // Need to be explicit
    std::cout << "Both are either nullptr or not\n";
}
```
在有了上述问题之后，我们的代码长这样：

```cpp
namespace ThorsAnvil
{
	template<typename T>
	class UP
	{
		T* data;
	public:
		// Explicit constructor
		explicit UP(T* data)
			: data(data)
		{}
		~UP()
		{
			delete data;
		}
		// Remove compiler generated methods.
		UP(UP const&) = delete;
		UP& operator=(UP const&) = delete;

		// Const correct access owned object
		T* operator->() const { return data; }
		T& operator*()  const { return *data; }

		// Access to smart pointer state
		T* get()                 const { return data; }
		explicit operator bool() const { return data; }

		// Modify object state
		T* release()
		{
			T* result = nullptr;
			std::swap(result, data);
			return result;
		}
	};
}
```

第一部分完结。
https://lokiastari.com/blog/2014/12/30/c-plus-plus-by-example-smart-pointer/index.html