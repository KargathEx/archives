#ifndef __PARSER_COMBINATOR__

#define __PARSER_COMBINATOR__

#include <type_traits>
#include <string>
#include <vector>
#include <tuple>
#include <set>
#include <unordered_map>
#include <initializer_list>
#include <functional>
#include <memory>
#include <stdexcept>

#define NAMESPACE_BEGIN namespace RiRi{
#define NAMESPACE_END   }	//为啥要有个这,大概是代码太长吧hhh

NAMESPACE_BEGIN

//这获得的是啥？ 函数类型？
template <typename T>
struct function_traits
	: function_traits<decltype(&T::operator())>	//这个好像叫CRTP,不是，那个涉及到基类和派生类，这个应该就是榨取用的。
{
};

template <typename ClassType,
	typename ReturnType,
	typename... Args>
	struct function_traits<ReturnType(ClassType::*)(Args...) const>//传入一个函数指针，对应的位置都会被匹配出来。
{
	typedef ReturnType result_type;	//获知返回值类型。
	typedef std::tuple<Args...> args_type;	//参数列表类型是一个tuple.
	template<size_t index>	//返回参数个数的第index个元素
	using arg = typename std::tuple_element_t<index, args_type>;

	static constexpr size_t arity = sizeof...(Args);	//元素个数.
};

template<class T>
class Reverse_helper	//!为什么不直接rbegin呢？
{
public:
	Reverse_helper(T const& _container) noexcept
		:container(_container)
	{}

	auto begin() const noexcept { return std::make_reverse_iterator(std::end(container)); }
	auto end()   const noexcept { return std::make_reverse_iterator(std::begin(container)); }
private:
	T const& container;
};

template<class T>
inline
Reverse_helper<T> Reverse(T const& _container) noexcept
{
	return Reverse_helper<T>(_container);
}
//古人云，实现标准库不过是大作业难度..

class Any	//与std::any的区别？
{
public:
	template<class T>
	using StorageType = typename std::decay_t<T>;

	template<typename U,
		class = typename std::enable_if_t<
		!std::is_same_v<StorageType<U>, Any>, void	//如果被存的不是any则存放一下，防止无限递归？
		>>
		Any(U&& _value)
		: ptr(new Container<StorageType<U>>(std::forward<U>(_value)))
	{
	}

	Any()
		: ptr(nullptr)
	{
	}

	Any(const Any& _that)
		: ptr(_that.clone())
	{
	}

	Any(Any&& _that)
		: ptr(_that.ptr)
	{
		_that.ptr = nullptr;
	}

	//这里的is是用来 https://www.reddit.com/r/cpp_questions/comments/mxo16h/as_keyword_in_c/
	//而23的is则是 http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2392r0.pdf ,看起来要大得多。
	template<class U>	
	inline
		bool is() const	//如果能转成功，就属于同一类型。
	{
		typedef StorageType<U> T;

		if (!ptr)
			return false;

		auto derived = dynamic_cast<Container<T>*> (ptr); 
		//转型失败且 新类型 是指针类型，那么它会返回该类型的空指针。
		return !(derived == nullptr);
	}

	template<class U>
	inline
		StorageType<U>& as()&	//这个返回的就是值了。
	{
		typedef StorageType<U> T;

		if (!ptr)
			throw std::bad_cast();

		auto container = dynamic_cast<Container<T>*> (ptr);
		//如果转型失败且 新类型 是引用类型，那么它会抛出与类型 std::bad_cast 的处理块匹配的异常。
		if (!container)
			throw std::bad_cast();

		return container->value;
	}

	template<class U>
	inline
		const StorageType<U>& as() const&	//const版本。
	{
		typedef StorageType<U> T;

		if (!ptr)
			throw std::bad_cast();

		auto container = dynamic_cast<Container<T>*> (ptr);

		if (!container)
			throw std::bad_cast();

		return container->value;
	}

	template<class U>
	inline
		StorageType<U>&& as()&&		//右值引用版本。
	{
		typedef StorageType<U> T;

		if (!ptr)
			throw std::bad_cast();

		auto container = dynamic_cast<Container<T>*> (ptr);

		if (!container)
			throw std::bad_cast();

		return std::move(container->value);
	}

	template<class U>
	inline
		operator U() const	//类型转换操作符直接使用它。
	{
		return as<StorageType<U>>();
	}

	template<class U>
	inline
		Any& reset(U&& _value)	//重置一下给别的jb插。等同于 https://zh.cppreference.com/w/cpp/utility/any/reset 吧？
	{
		if (ptr)
			delete ptr;
		ptr = new Container<StorageType<U>>(std::forward<U>(_value));
		return *this;
	}

	Any& operator=(const Any& a)
	{
		if (ptr == a.ptr)
			return *this;
		auto old_ptr = ptr;
		ptr = a.clone();
		if (old_ptr)
			delete old_ptr;
		return *this;
	}

	Any& operator=(Any&& a)
	{
		if (ptr == a.ptr)
			return *this;
		std::swap(ptr, a.ptr);
		return *this;
	}

	~Any()
	{
		if (ptr)
			delete ptr;
		ptr = nullptr;
	}

private:
	class ContainerBase
	{
	public:
		virtual ~ContainerBase() = default;
		virtual ContainerBase* clone() const = 0;
	};

	template<typename T>
	class Container
		:public ContainerBase
	{
	public:

		template<typename U>
		Container(U&& value)
			:value(std::forward<U>(value))
		{
		}

		inline
			ContainerBase* clone() const
		{
			return new Container<T>(value);
		}

		T value;
	};

	inline
		ContainerBase* clone() const
	{
		if (ptr)
			return ptr->clone();
		else
			return nullptr;
	}

	ContainerBase* ptr;
};

//https://en.wikipedia.org/wiki/Product_type
//https://manishearth.github.io/blog/2017/03/04/what-are-sum-product-and-pi-types/
//http://nlab-pages.s3.us-east-2.amazonaws.com/nlab/show/product+type 
//介绍很清楚，就是... struct一样的东西？ 那为啥还要单独搞一个?为了可以接受任意数量的来构造我们自己的。
template<class _Type,
	class _Type0,
	class... _Types>
	struct ProductTypes
{
	typedef std::tuple<_Type, _Type0, _Types...> Tuple_type;

	ProductTypes() = default;
	
	template<class... _Tys,
		class = typename std::enable_if_t<std::is_constructible_v<Tuple_type, const std::tuple<_Tys...>>, void>>
		ProductTypes(_Tys&&... __Args)	//如果是可构造的的话就把后面部分作为参数构造出来，否则为void,是这么理解吗？ 第二遍再来熟悉语法.
		//如果可以从<,_>构造出来<_,>的话就存在，这是内层，外层则是如果第一个表达式为true则，存在第二个类型void,否则此函数整个都不存在。
		:_Data(std::tuple<_Tys...>(std::forward<_Tys>(__Args)...))
	{
	}
	//上面是对通用类型的转发，这里是基本类型。
	template<class... _Tys,
		class = typename std::enable_if_t<std::is_constructible_v<Tuple_type, const std::tuple<_Tys...>>, void>>
		ProductTypes(const std::tuple<_Tys...>& __Other)
		:_Data(__Other)
	{
	}
	//这里是右值
	template<class... _Tys,
		class = typename std::enable_if_t<std::is_constructible_v<Tuple_type, std::tuple<_Tys...>>, void>>
		ProductTypes(std::tuple<_Tys...>&& __Other)
		: _Data(std::move(__Other))
	{
	}
	//这里大概是用其他ProductTypes作为此ProductTypes的一部分时候的构造函数了。
	template<class _Ty,
		class _Ty0,
		class... _Tys,
		class Tuple = typename ProductTypes<_Ty, _Ty0, _Tys...>::Tuple_type,
		class = typename std::enable_if_t<std::is_constructible_v<Tuple_type, Tuple>, void>>
		ProductTypes(const ProductTypes<_Ty, _Ty0, _Tys...>& __Other)
		:_Data(__Other._Data)
	{
	}
	//右值版本
	template<class _Ty,
		class _Ty0,
		class... _Tys,
		class Tuple = typename ProductTypes<_Ty, _Ty0, _Tys...>::Tuple_type,
		class = typename std::enable_if_t<std::is_constructible_v<Tuple_type, Tuple>, void>>
		ProductTypes(ProductTypes<_Ty, _Ty0, _Tys...>&& __Other)
		:_Data(std::move(__Other._Data))
	{
	}
	//正经事，获取tuple里的第i个元素的值。
	template<size_t _Index>
	typename std::enable_if_t<
		(_Index < std::tuple_size_v<Tuple_type>),
		typename std::tuple_element<_Index, Tuple_type>	//https://zh.cppreference.com/w/cpp/utility/tuple/tuple_element
		>::type& get()
	{
		return std::get<_Index>(_Data);
	}
	//const版本.
	template<size_t _Index>
	typename std::enable_if_t<
		(_Index < std::tuple_size_v<Tuple_type>),
		const typename std::tuple_element<_Index, Tuple_type>::type
		>& get() const
	{
		return std::get<_Index>(_Data);
	}

private:
	//获取内部类型的友元函数。
	template<class... _Tys>
	friend std::tuple<_Tys...>& ProductTypes_Get_tuple(ProductTypes<_Tys...>& Product);

	template<class... _Tys>
	friend const std::tuple<_Tys...>& ProductTypes_Get_tuple(const ProductTypes<_Tys...>& Product);
	
	//这可太好玩了。
	template<class Product1, class Product2, class _Return_type>
	friend _Return_type concat_Product(Product1&& _Arg1, Product2&& _Arg2);
	
	//操作上的区别只有先1还是先2，实际return type不相同.
	template<class Product1, class Product2, class _Return_type>
	friend _Return_type push_Product(Product1&& _Arg1, Product2&& _Arg2);

	template<class Product1, class Product2, class _Return_type>
	friend _Return_type push_front_Product(Product1&& _Arg1, Product2&& _Arg2);

	Tuple_type& _Get_tuple()&
	{
		return _Data;
	}

	const Tuple_type& _Get_tuple() const&
	{
		return _Data;
	}

	Tuple_type&& _Get_tuple()&&		//拍平了出一个tuple<...>
	{
		return std::move(_Data);
	}

	Tuple_type _Data;
};

//根据类型参数存在与否来分真假，只要能匹配上就合格
template<class _Ty>
struct is_ProductTypes
	:std::false_type
{
};

template<class _Ty,
	class _Ty0,
	class... _Tys>
	struct is_ProductTypes<ProductTypes<_Ty, _Ty0, _Tys...>>
	:std::true_type
{
};

//!会用在哪些地方？一千多行.. 先扔着，总之知道是返回tuple<...>就好了。
template<class _Ty>
struct ProductTypesTuple
{
	typedef std::tuple<_Ty> type;
};

template<class _Ty,
	class _Ty0,
	class... _Tys>
	struct ProductTypesTuple<ProductTypes<_Ty, _Ty0, _Tys...>>
{
	typedef std::tuple<_Ty, _Ty0, _Tys...> type;
};

//大概是从tuple里拆出来东西取消掉&之后再组装起来？查不到别的。
template<class _Ty>
struct ProductTypesConstructor_impl;

template<class _Ty>
struct ProductTypesConstructor_impl<std::tuple<_Ty>>
{
	typedef std::decay_t<_Ty> type;
};

template<class _Ty,
	class _Ty0,
	class... _Tys>
	struct ProductTypesConstructor_impl<std::tuple<_Ty, _Ty0, _Tys...>>
{
	typedef ProductTypes<std::decay_t<_Ty>, std::decay_t<_Ty0>, std::decay_t<_Tys>...> type;
};

template<class... _Types>
using ProductTypesConstructor = typename ProductTypesConstructor_impl<typename std::tuple<_Types...>>::type;

template<class _Ty>
struct getProductTypesTuple
{
	typedef std::tuple<std::decay_t<_Ty>> type;
};

template<class _Ty,
	class _Ty0,
	class... _Tys>
	struct getProductTypesTuple<ProductTypes<_Ty, _Ty0, _Tys...>>
{
	typedef typename ProductTypes<_Ty, _Ty0, _Tys...>::Tuple_type type;
};
//↑这些全都在decay,场景是什么？


//前面ProductTypes里用过的那些类型
template<class _Type, class _Ty0, class... _Types>
struct PushProductTypes;

template<class _Ty,
	class _Ty0,
	class... _Types>
	struct PushProductTypes
{
	typedef ProductTypes<std::decay_t<_Ty>, std::decay_t<_Ty0>, std::decay_t<_Types>...> type;
};

template<class _Ty,
	class _Ty0,
	class... _Tys,
	class _Type,
	class... _Types>
	struct PushProductTypes<ProductTypes<_Ty, _Ty0, _Tys...>, _Type, _Types...>
{
	typedef ProductTypes<_Ty, _Ty0, _Tys..., std::decay_t<_Type>, std::decay_t<_Types>...> type;
};

template<class _Type, class _Ty0, class... _Types>
struct PushFrontProductTypes;

template<class _Ty,
	class _Ty0,
	class... _Types>
	struct PushFrontProductTypes
{
	typedef ProductTypes<std::decay_t<_Ty0>, std::decay_t<_Types>..., std::decay_t<_Ty>> type;
};

template<class _Ty,
	class _Ty0,
	class... _Tys,
	class _Type,
	class... _Types>
	struct PushFrontProductTypes<ProductTypes<_Ty, _Ty0, _Tys...>, _Type, _Types...>
{
	typedef ProductTypes<std::decay_t<_Type>, std::decay_t<_Types>..., _Ty, _Ty0, _Tys...> type;
};

template<class _Type, class... _Types>
struct ConcatProductTypes;

template<class _Type,
	class _Ty,
	class _Ty0,
	class... _Tys>
	struct ConcatProductTypes<_Type, ProductTypes<_Ty, _Ty0, _Tys...>>
{
	typedef typename PushProductTypes<_Type, _Ty, _Ty0, _Tys...>::type type;
};
//↑至此为止

//获取里面储存的一个内部tuple.
template<class... _Tys>
inline
std::tuple<_Tys...>& ProductTypes_Get_tuple(ProductTypes<_Tys...>& Product)
{
	return Product._Get_tuple();
}

template<class... _Tys>
inline
const std::tuple<_Tys...>& ProductTypes_Get_tuple(const ProductTypes<_Tys...>& Product)
{
	return Product._Get_tuple();
}

//前面ProductTypes里用过的那些函数
template<class Product1,
	class Product2,
	class _Return_type = typename ConcatProductTypes<std::decay_t<Product1>, std::decay_t<Product2>>::type>
	inline
	_Return_type concat_Product(Product1&& _Arg1, Product2&& _Arg2)
{
	return _Return_type(std::tuple_cat(_Arg1._Get_tuple(), _Arg2._Get_tuple()));
}
template<class Product1,
	class Product2,
	class _Return_type = typename PushProductTypes<std::decay_t<Product1>, std::decay_t<Product2>>::type>
	inline
	_Return_type push_Product(Product1&& _Arg1, Product2&& _Arg2)
{
	return _Return_type(std::tuple_cat(_Arg1._Get_tuple(),
		std::tuple<std::decay_t<Product2>>(std::forward<Product2>(_Arg2))));
}

template<class Product1,
	class Product2,
	class _Return_type = typename PushFrontProductTypes<std::decay_t<Product1>, std::decay_t<Product2>>::type>
	inline
	_Return_type push_front_Product(Product1&& _Arg1, Product2&& _Arg2)
{
	return _Return_type(std::tuple_cat(std::tuple<std::decay_t<Product2>>(std::forward<Product2>(_Arg2)),
		_Arg1._Get_tuple()));
}

//加法类型?终于到正事了。
template<class _Type, class _Type0, class... _Types>
struct AdditionTypes;

template<class _Type>
struct AdditionTypesBase
{
};

struct AdditionTypesData
{
protected:
	typedef Any _Data_type;

	AdditionTypesData() = default;

	template<class _Ty>
	AdditionTypesData(_Ty&& __Data)
		:_Data(std::forward<_Ty>(__Data))
	{
	}

	_Data_type& _Get_data()&
	{
		return _Data;
	}

	const _Data_type& _Get_data() const&
	{
		return _Data;
	}

	_Data_type&& _Get_data()&&
	{
		return std::move(_Data);
	}

private:
	Any _Data;

	template<class _Type, class _Type0, class... _Types>
	friend struct AdditionTypes;
};

template<class _From, class _To>
struct AdditionTypesConvertible;

template<class _From, class _To>
struct AdditionTypesStrictConvertible;

template<class _Ty, class _Arg>
struct AdditionTypesConstructible;

template<class _Ty>
struct is_AdditionTypes
	:std::false_type
{
};

template<class _Ty,
	class _Ty0,
	class... _Tys>
	struct is_AdditionTypes<AdditionTypes<_Ty, _Ty0, _Tys...>>
	:std::true_type
{
};

template<class _Type,
	class _Type0,
	class... _Types>
	struct AdditionTypes
	:AdditionTypesData,
	AdditionTypesBase<_Type>, AdditionTypesBase<_Type0>, AdditionTypesBase<_Types>...
{
	template<class _Ty>
	using type_included = typename std::is_base_of<AdditionTypesBase<std::decay_t<_Ty>>, AdditionTypes>;

	template<class _Ty>
	using type_constructible = AdditionTypesConstructible<AdditionTypes, std::decay_t<_Ty>>;

	AdditionTypes() = default;

	template<class _Ty,
		class = typename std::enable_if_t<
		type_included<_Ty>::value ||
		(!is_AdditionTypes<std::decay_t<_Ty>>::value && type_constructible<_Ty>::value),
		void
		>>
		AdditionTypes(_Ty&& __Data)
		:AdditionTypesData(
			typename std::conditional<
			type_included<_Ty>::value,
			_Ty,
			typename type_constructible<_Ty>::type
			>::type(std::forward<_Ty>(__Data))
		)
	{
	}

	template<class _Ty,
		class... _Tys,
		class = typename std::enable_if_t<AdditionTypesStrictConvertible<AdditionTypes<_Ty, _Tys...>, AdditionTypes>::value, void>>
		AdditionTypes(const AdditionTypes<_Ty, _Tys...>& __Other)
		:AdditionTypesData(__Other._Data)
	{
	}

	template<class _Ty,
		class... _Tys,
		class = typename std::enable_if_t<AdditionTypesStrictConvertible<AdditionTypes<_Ty, _Tys...>, AdditionTypes>::value, void>>
		AdditionTypes(AdditionTypes<_Ty, _Tys...>&& __Other)
		:AdditionTypesData(std::move(__Other._Data))
	{
	}

	template<class _Ty,
		class... _Tys,
		class = typename std::enable_if_t<
		!AdditionTypesStrictConvertible<AdditionTypes<_Ty, _Tys...>, AdditionTypes>::value&&
		AdditionTypesConvertible<AdditionTypes<_Ty, _Tys...>, AdditionTypes>::value,
		void
		>>
		AdditionTypes(const AdditionTypes<_Ty, _Tys...>& __Other, size_t = 0)
	{
		construct_helper<_Ty, _Tys...>::construct(this, __Other);
	}

	template<class _Ty,
		class... _Tys,
		class = typename std::enable_if_t<
		!AdditionTypesStrictConvertible<AdditionTypes<_Ty, _Tys...>, AdditionTypes>::value&&
		AdditionTypesConvertible<AdditionTypes<_Ty, _Tys...>, AdditionTypes>::value,
		void
		>>
		AdditionTypes(AdditionTypes<_Ty, _Tys...>&& __Other, size_t = 0)
	{
		construct_helper<_Ty, _Tys...>::construct(this, std::move(__Other));
	}

	template<class _Ty,
		class = typename std::enable_if_t<
		type_included<_Ty>::value || type_constructible<_Ty>::value,
		void
		>>
		bool contain() const
	{
		return this->_Data.template is<_Ty>();
	}

	template<class _Ty,
		class = typename std::enable_if_t<
		type_included<_Ty>::value,
		void
		>>
		_Ty & get()
	{
		return this->_Data.template as<_Ty>();
	}

	template<class _Ty,
		class = typename std::enable_if_t<
		type_included<_Ty>::value,
		void
		>>
		const _Ty& get() const
	{
		return this->_Data.template as<_Ty>();
	}

	template<class _Ty,
		class = typename std::enable_if_t<
		!AdditionTypesConvertible<AdditionTypes, std::decay_t<_Ty>>::value&&
		std::conjunction<std::is_constructible<_Ty, _Type>,
		std::is_constructible<_Ty, _Type0>,
		std::is_constructible<_Ty, _Types>...
		>::value,
		void
		>>
		operator _Ty() const
	{
		return convert_helper<_Type, _Type0, _Types...>::template get<std::decay_t<_Ty>>(this);
	}

private:
	template<class... _Types_>
	struct construct_helper;

	template<class _Type_>
	struct construct_helper<_Type_>
	{
		template<class _Container, class _Other>
		static
			void construct(_Container container, _Other&& other)
		{
			typedef typename std::conditional<
				type_included<_Type_>::value,
				_Type_,
				typename type_constructible<_Type_>::type
			>::type _To_type;

			if (other._Get_data().template is<_Type_>())
				container->_Data.reset(_To_type(std::forward<_Other>(other)._Get_data().template as<_Type_>()));
			else
				throw std::bad_cast();
		}
	};

	template<class _Type_, class _Type0_, class... _Types_>
	struct construct_helper<_Type_, _Type0_, _Types_...>
	{
		template<class _Container, class _Other>
		static
			void construct(_Container container, _Other&& other)
		{
			typedef typename std::conditional<
				type_included<_Type_>::value,
				_Type_,
				typename type_constructible<_Type_>::type
			>::type _To_type;

			if (other._Get_data().template is<_Type_>())
				container->_Data.reset(_To_type(std::forward<_Other>(other)._Get_data().template as<_Type_>()));
			else
				construct_helper<_Type0_, _Types_...>::construct(container, std::forward<_Other>(other));
		}
	};

	template<class... _Types_>
	struct convert_helper
	{
		template<class _Ty,
			class _Container>
			static
			_Ty get(_Container)
		{
			return _Ty();
		}
	};

	template<class _Type_>
	struct convert_helper<_Type_>
	{
		template<class _Ty,
			class _Container>
			static
			typename std::enable_if_t<!std::is_constructible_v<_Ty, _Type_>, _Ty>
			get(_Container)
		{
			return _Ty();
		}

		template<class _Ty,
			class _Container>
			static
			typename std::enable_if_t<std::is_constructible_v<_Ty, _Type_>, _Ty>
			get(_Container container)
		{
			if (container->template contain<_Type_>())
				return container->template get<_Type_>();
			throw std::bad_cast();
		}
	};

	template<class _Type_, class _Type0_, class... _Types_>
	struct convert_helper<_Type_, _Type0_, _Types_...>
	{
		template<class _Ty,
			class _Container>
			static
			typename std::enable_if_t<!std::is_constructible_v<_Ty, _Type_>, _Ty>
			get(_Container container)
		{
			return convert_helper<_Ty, _Type0_, _Types_...>::template get<_Ty>(container);
		}

		template<class _Ty,
			class _Container>
			static
			typename std::enable_if_t<std::is_constructible_v<_Ty, _Type_>, _Ty>
			get(_Container container)
		{
			return container->template contain<_Type_>() ?
				container->template get<_Type_>() :
				convert_helper<_Type0_, _Types_...>::template get<_Ty>(container);
		}
	};

};

template<class _Type>
struct AdditionTypesConstructor_impl;

template<class _Type>
struct AdditionTypesConstructor_impl<std::tuple<_Type>>
{
	typedef std::decay_t<_Type> type;
};

template<class _Type, class _Type0>
struct AdditionTypesConstructor_impl<std::tuple<_Type, _Type0>>
{
	typedef std::decay_t<_Type> _dType;
	typedef std::decay_t<_Type0> _dType0;
	typedef typename std::conditional<std::is_same_v<_dType, _dType0>,
		_dType,
		AdditionTypes<_dType, _dType0>
	>::type type;
};

template<class _Type, class _Type0, class _Type1, class... _Types>
struct AdditionTypesConstructor_impl<std::tuple<_Type, _Type0, _Type1, _Types...>>
{
	template<class _Ty, class _SubType>
	struct helper
	{
		typedef typename AdditionTypesConstructor_impl<std::tuple<_SubType, _Ty>>::type type;
	};

	template<class _Ty, class... _Tys, class _SubType>
	struct helper<AdditionTypes<_Ty, _Tys...>, _SubType>
	{
		typedef AdditionTypes<_SubType, _Ty, _Tys...> type;
	};

	typedef typename AdditionTypesConstructor_impl<typename std::tuple<_Type0, _Type1, _Types...>>::type _SubAdditionTypes;
	typedef typename std::conditional<std::is_base_of_v<AdditionTypesBase<_Type>, _SubAdditionTypes>,
		_SubAdditionTypes,
		typename helper<_SubAdditionTypes, _Type>::type
	>::type type;
};

template<class... _Types>
using AdditionTypesConstructor = typename AdditionTypesConstructor_impl<std::tuple<_Types...>>::type;

template<class _Type, class _SubType>
struct PushAdditionTypes;

template<class _Type, class _SubType>
struct PushAdditionTypes
{
	typedef AdditionTypesConstructor<_Type, _SubType> type;
};

template<class _Ty,
	class... _Tys,
	class _SubType>
	struct PushAdditionTypes<AdditionTypes<_Ty, _Tys...>, _SubType>
{
	typedef std::decay_t<_SubType> _dSubType;
	typedef AdditionTypes<_Ty, _Tys...> _Type;
	typedef typename std::conditional<std::is_base_of_v<AdditionTypesBase<_dSubType>, _Type>,
		_Type,
		AdditionTypes<_Ty, _Tys..., _dSubType>
	>::type type;
};

template<template<class, class...> class _Cond,
	class _SubTypes, class _To>
	struct AdditionTypesConvertible_impl;

template<template<class, class...> class _Cond,
	class _To>
	struct AdditionTypesConvertible_impl<_Cond, std::tuple<>, _To>
	:std::true_type
{
};

template<template<class, class...> class _Cond,
	class _SubType,
	class... _SubTypes,
	class _To>
	struct AdditionTypesConvertible_impl<_Cond, std::tuple<_SubType, _SubTypes...>, _To>
	:std::conditional<_Cond<_To, _SubType>::value,
	AdditionTypesConvertible_impl<_Cond, std::tuple<_SubTypes...>, _To>,
	std::false_type
	>::type
{
};

template<class _From, class _To>
struct AdditionTypesConvertible
	:std::false_type
{
};

template<class _From,
	class _Ty1,
	class... _Tys1>
	struct AdditionTypesConvertible<_From, AdditionTypes<_Ty1, _Tys1...>>
	:AdditionTypesConvertible_impl<std::is_constructible, std::tuple<_From>, AdditionTypes<_Ty1, _Tys1...>>
{
};

template<class _Ty1,
	class... _Tys1,
	class _Ty2,
	class... _Tys2>
	struct AdditionTypesConvertible<AdditionTypes<_Ty1, _Tys1...>, AdditionTypes<_Ty2, _Tys2...>>
	:AdditionTypesConvertible_impl<std::is_constructible, std::tuple<_Ty1, _Tys1...>, AdditionTypes<_Ty2, _Tys2...>>
{
};

template<class _From, class _To>
struct AdditionTypesStrictConvertible
	:std::false_type
{
};

template<class _Ty,
	class _Ty_,
	class _Ty2,
	class... _Tys2>
	struct AdditionTypesStrictConvertible<AdditionTypes<_Ty, _Ty_>, AdditionTypes<_Ty2, _Tys2...>>
	:std::conditional<
	std::is_base_of<AdditionTypesBase<_Ty>, AdditionTypes<_Ty2, _Tys2...>>::value&&
	std::is_base_of<AdditionTypesBase<_Ty_>, AdditionTypes<_Ty2, _Tys2...>>::value,
	std::true_type, std::false_type
	>::type
{
};

template<class _Ty1,
	class _Ty1_,
	class... _Tys1,
	class _Ty2,
	class... _Tys2>
	struct AdditionTypesStrictConvertible<AdditionTypes<_Ty1, _Ty1_, _Tys1...>, AdditionTypes<_Ty2, _Tys2...>>
	:std::conditional<
	std::is_base_of<AdditionTypesBase<_Ty1>, AdditionTypes<_Ty2, _Tys2...>>::value&&
	AdditionTypesStrictConvertible<AdditionTypes<_Ty1_, _Tys1...>, AdditionTypes<_Ty2, _Tys2...>>::value,
	std::true_type, std::false_type
	>::type
{
};

template<class _Ty,
	class _is_Constructible = std::false_type>
	struct Constructible_type
	: _is_Constructible
{
	typedef _Ty type;
};

template<class _SubTypes, class _Arg>
struct AdditionTypesConstructible_impl;

template<class _Arg>
struct AdditionTypesConstructible_impl<std::tuple<>, _Arg>
	:Constructible_type<void>
{
};

template<class _SubType,
	class... _SubTypes,
	class _Arg>
	struct AdditionTypesConstructible_impl<std::tuple<_SubType, _SubTypes...>, _Arg>
	:std::conditional<std::is_constructible_v<_SubType, _Arg>,
	Constructible_type<_SubType, std::true_type>,
	AdditionTypesConstructible_impl<std::tuple<_SubTypes...>, _Arg>
	>::type
{
};

template<class _Ty,
	class _Arg>
	struct AdditionTypesConstructible
	: std::false_type
{
};

template<class _Ty,
	class... _Tys,
	class _Arg>
	struct AdditionTypesConstructible<AdditionTypes<_Ty, _Tys...>, _Arg>
	: AdditionTypesConstructible_impl<std::tuple<_Ty, _Tys...>, _Arg>
{
};

template<class _Type, class _Arg>
struct is_strict_constructible
{
	static const bool value =
		(!is_AdditionTypes<_Type>::value && !is_AdditionTypes<_Arg>::value &&
			std::is_constructible_v<_Type, _Arg>) ||
		(is_AdditionTypes<_Type>::value && is_AdditionTypes<_Arg>::value &&
			AdditionTypesStrictConvertible<_Arg, _Type>::value);
};

struct tuple_helper
{
	template<class _Ty>
	struct decay
	{
		typedef std::tuple<> type;
	};

	template<class... _Tys>
	struct decay<std::tuple<_Tys...>>
	{
		typedef std::tuple<std::decay_t<_Tys>...> type;
	};

	template <class F, class Tuple, std::size_t... I>
	static inline
		constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
	{
		return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
	}

	template <class F, class Tuple>
	static inline
		constexpr decltype(auto) apply(F&& f, Tuple&& t)
	{
		return apply_impl(
			std::forward<F>(f), std::forward<Tuple>(t),
			std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
	}
};

struct AnyChar {};

template<class _Ty>
struct ReturnValue
{
	ReturnValue() = default;
	ReturnValue(_Ty&& _value)
		:value(std::forward<_Ty>(_value))
	{
	}

	const _Ty& operator()() const
	{
		return value;
	}

	const _Ty& get() const
	{
		return value;
	}

private:
	const _Ty value;
};

template<class _Ty>
struct is_ReturnValue
	:std::false_type
{
};

template<class _Ty>
struct is_ReturnValue<ReturnValue<_Ty>>
	:std::true_type
{
};

template<class _Return_type>
class ParserCombinator;

template<class _Return_type>
class ParserCombinatorComponent;
class ParserCombinatorComponentBase;

typedef Any Parsed;
struct EpsilonParsed {};
struct NoParse {};

template<class _Return_type>
using RecoveryParse = std::pair<size_t, _Return_type>;

struct Placeholder
{
	Placeholder() = default;

	template<class _Ty>
	Placeholder(_Ty&&) {};
};

class ParserCombinatorComponentBase
{
public:
	virtual ~ParserCombinatorComponentBase() = default;

protected:

	template<class _Return_type_1>
	friend class ParserCombinator;

	template<class _Return_type_1>
	friend class ParserCombinatorComponent;

	class __impl;
	typedef std::shared_ptr<__impl> __pImpl;

	__pImpl impl;

	struct LeftRecursion;

	struct Result
	{
		std::vector<std::unordered_map<const void*, LeftRecursion>>* memorization;
		const std::string* const str;
		size_t index;
		Parsed value;
		const void* _P_this;

		bool operator < (const Result& other) const
		{
			return index < other.index;
		}
	};

	struct LeftRecursion
	{
		struct Info
		{
			bool completed;
			size_t count;
			std::set<Result> result;
		};
		std::unordered_map<const void*, Info> info;
	};

	typedef Parsed ParsedResult;
	typedef std::function<bool(const ParsedResult&)> Continuation;

	class __impl
	{
	public:
		typedef std::function<bool(const Result&, const Continuation&)> __funcType;
		__funcType func;

		void setFunc(const __funcType& _func)
		{
			func = _func;
		}

		void setFunc(__funcType&& _func)
		{
			func = std::move(_func);
		}

		bool exec(const Result& in, const Continuation& cont) const
		{
			return func(in, cont);
		}

	};

	ParserCombinatorComponentBase(const __pImpl& _ptr)
		:impl(_ptr)
	{
	}

	ParserCombinatorComponentBase(__pImpl&& _ptr)
		:impl(std::move(_ptr))
	{
	}

	template<class _Return_type>
	ParserCombinatorComponentBase(const ParserCombinator<_Return_type>& ref, bool copy = false)
		: impl(new __impl)
	{
		auto func = ([](const Result& in, const Continuation& cont, const ParserCombinatorComponent<_Return_type>* ptr)
			{
				auto _P_target = ptr;
				auto _in(in);
				_in._P_this = _P_target;

				auto& memorization((*(in.memorization))[in.index]);
				if (memorization.find(_P_target) != memorization.end())
				{
					auto& L(memorization[_P_target]);
					if (L.info.find(in._P_this) != L.info.end())
					{
						auto& L_Rec(L.info[in._P_this]);
						auto& result(L_Rec.result);
						if (L_Rec.completed)
						{
							if (result.empty())
								return cont(NoParse{});
							for (auto& i : Reverse(result))
							{
								if (cont(i))
								{
									return true;
								}
							}
							return false;
						}
						else
						{
							if (L_Rec.count > (in.str->length() - in.index) + 1)
							{
								return cont(NoParse{});
							}
							L_Rec.count++;
							return ptr->impl->exec(_in, [&cont, &result](auto ret)
								{
									if (ret.template is<Result>())
									{
										auto& ret_struct(ret.template as<Result>());
										if (result.find(ret_struct) == result.end())
										{
											result.insert(ret_struct);
										}
									}
									return cont(ret);
								});
						}
					}
					else
					{
						L.info[in._P_this] = LeftRecursion::Info{ false, 1, {} };
						goto __Recursive;
					}
				}
				else
				{
					memorization[_P_target] = LeftRecursion{
						{{
							_P_target,
							{ false, 1, {} },
						}}
					};
				__Recursive:;

					return ptr->impl->exec(_in, [&cont, &memorization, &_P_target](auto ret)
						{
							auto& L_Rec(memorization[_P_target].info[_P_target]);
							auto& result(L_Rec.result);

							if (ret.template is<Result>())
							{
								auto& ret_struct(ret.template as<Result>());
								result.insert(ret_struct);
							}

							if (!L_Rec.completed)
							{
								L_Rec.completed = true;
								if (!result.empty())
								{
									for (auto& i : Reverse(result))
									{
										if (cont(i))
										{
											return true;
										}
									}
									return false;
								}
								else
								{
									return cont(ret);
								}
							}
							else
							{
								return cont(ret);
							}
						});
				}
			});
		__impl::__funcType captured;
		if (!copy)
		{
			captured = ([func = std::move(func),
				_Ref = std::weak_ptr<typename decltype(ref.bind)::element_type>(ref.bind)]
				(const Result& in, const Continuation& cont)
			{
				return func(in, cont,
					std::static_pointer_cast<ParserCombinatorComponent<_Return_type>>(*_Ref.lock()).get());
			});
		}
		else
		{
			captured = ([func = std::move(func),
				_Ref = ref._Get_component_ptr()]
				(const Result& in, const Continuation& cont)
			{
				return func(in, cont, _Ref.get());
			});
		}
		impl->setFunc(std::move(captured));
	}

	ParserCombinatorComponentBase(const std::function<bool(char)>& func)
		:impl(new __impl)
	{
		impl->setFunc([func](const Result& in, const Continuation& cont)
			{
				if (in.index < in.str->length() && func(in.str->operator[](in.index)))
				{
					return cont(Result
						{
							in.memorization,
							in.str,
							in.index + 1,
							in.str->operator[](in.index),
							in._P_this,
						});
				}
				else
				{
					return cont(NoParse{});
				}
			});
	}

	ParserCombinatorComponentBase(const char token)
		:impl(new __impl)
	{
		impl->setFunc([token](const Result& in, const Continuation& cont)
			{
				if (in.index < in.str->length() && in.str->operator[](in.index) == token)
				{
					return cont(Result
						{
							in.memorization,
							in.str,
							in.index + 1,
							token,
							in._P_this,
						});
				}
				else
				{
					return cont(NoParse{});
				}
			});
	}

	ParserCombinatorComponentBase(const std::function<size_t(const std::string&)>& func)
		:impl(new __impl)
	{
		impl->setFunc([func](const Result& in, const Continuation& cont)
			{
				auto check(func(in.str->substr(in.index, in.str->length() - in.index)));
				if (in.index < in.str->length() && check > 0)
				{
					if (in.index + check > in.str->length())
						throw std::out_of_range(*in.str);
					return cont(Result
						{
							in.memorization,
							in.str,
							in.index + check,
							in.str->substr(in.index, check),
							in._P_this,
						});
				}
				else
				{
					return cont(NoParse{});
				}
			});
	}

	ParserCombinatorComponentBase(const std::string& token)
		:impl(new __impl)
	{
		impl->setFunc([token](const Result& in, const Continuation& cont)
			{
				if (in.str->compare(in.index, token.length(), token) == 0)
				{
					return cont(Result
						{
							in.memorization,
							in.str,
							in.index + token.length(),
							token,
							in._P_this,
						});
				}
				else
				{
					return cont(NoParse{});
				}
			});
	}

	template<class _Return_type_1,
		class _Return_type_2>
		static
		typename std::enable_if_t<is_ProductTypes<_Return_type_1>::value, __pImpl>
		connect(const __pImpl& lhs, const __pImpl& rhs)
	{
		__pImpl impl(new __impl);
		impl->setFunc([lhsImpl = lhs, rhsImpl = rhs]
		(const Result& in, const Continuation& cont)
			{
				return lhsImpl->exec(in, [&cont, &rhsImpl](auto first)
					{
						if (first.template is<Result>())
						{
							auto value(std::move(first.template as<Result>().value));
							first.template as<Result>().value = EpsilonParsed{};
							return rhsImpl->exec(first, [&cont, &first_value = value](auto second)
								{
									if (second.template is<Result>())
									{
										auto& ret = second;
										auto& retValue(ret.template as<Result>().value);
										if (first_value.template is<EpsilonParsed>())
											first_value.reset(_Return_type_1());
										if (retValue.template is<EpsilonParsed>())
											retValue.reset(_Return_type_2());
										retValue.reset(push_Product(first_value.template as<_Return_type_1>(),
											std::move(retValue.template as<_Return_type_2>())));
										return cont(ret);
									}
									else
									{
										return cont(second);
									}
								});
						}
						else
						{
							return cont(first);
						}
					});
			});
		return impl;
	}

	template<class _Return_type_1,
		class _Return_type_2>
		static
		typename std::enable_if_t<!is_ProductTypes<_Return_type_1>::value, __pImpl>
		connect(const __pImpl& lhs, const __pImpl& rhs)
	{
		typedef typename PushProductTypes<_Return_type_1, _Return_type_2>::type _Result_type;
		__pImpl impl(new __impl);
		impl->setFunc([lhsImpl = lhs, rhsImpl = rhs]
		(const Result& in, const Continuation& cont)
			{
				return lhsImpl->exec(in, [&cont, &rhsImpl](auto first)
					{
						if (first.template is<Result>())
						{
							auto value(std::move(first.template as<Result>().value));
							first.template as<Result>().value = EpsilonParsed{};
							return rhsImpl->exec(first, [&cont, &first_value = value](auto second)
								{
									if (second.template is<Result>())
									{
										auto& ret = second;
										auto& retValue(ret.template as<Result>().value);
										if (first_value.template is<EpsilonParsed>())
											first_value.reset(_Return_type_1());
										if (retValue.template is<EpsilonParsed>())
											retValue.reset(_Return_type_2());
										retValue.reset(_Result_type(std::make_tuple(first_value.template as<_Return_type_1>(),
											std::move(retValue.template as<_Return_type_2>()))));
										return cont(ret);
									}
									else
									{
										return cont(second);
									}
								});
						}
						else
						{
							return cont(first);
						}
					});
			});
		return impl;
	}

	template<class _Return_type_1,
		class _Return_type_2>
		static
		__pImpl alternative(const __pImpl& lhs, const __pImpl& rhs)
	{
		typedef typename PushAdditionTypes<_Return_type_1, _Return_type_2>::type _Result_type;
		__pImpl impl(new __impl);
		impl->setFunc([lhsImpl = lhs, rhsImpl = rhs]
		(const Result& in, const Continuation& cont)
			{
				auto _in(in);
				_in.value = EpsilonParsed{};
				return lhsImpl->exec(_in, [&cont, &_in, &rhsImpl]
				(auto alt1)
					{
						if (alt1.template is<Result>())
						{
							if constexpr (!std::is_same_v<_Return_type_1, _Result_type>)
							{
								auto& alt1_value(alt1.template as<Result>().value);
								if (alt1_value.template is<EpsilonParsed>())
									alt1_value.reset(_Return_type_1());
								alt1_value.reset(_Result_type(alt1_value.template as<_Return_type_1>()));
								if (cont(alt1))
									return true;
								else
									goto __Alternative;

							}
							else
							{
								if (cont(alt1))
									return true;
								else
									goto __Alternative;
							}
						}
						else
						{
						__Alternative:;
							return rhsImpl->exec(_in, [&cont](auto alt2)
								{
									if constexpr (!std::is_same_v<_Return_type_2, _Result_type>)
									{
										if (alt2.template is<Result>())
										{
											auto& alt2_value(alt2.template as<Result>().value);
											if (alt2_value.template is<EpsilonParsed>())
												alt2_value.reset(_Return_type_2());
											alt2_value.reset(_Result_type(alt2_value.template as<_Return_type_2>()));
											return cont(alt2);
										}
									}
									else
									{
										return cont(alt2);
									}
								});
						}
					});
			});
		return impl;
	}

	template<class _Return_type,
		class _Func,
		class traits = function_traits<typename std::decay_t<_Func>>>
		static
		typename std::enable_if_t<
		traits::arity == 1,
		__pImpl
		> callback(const __pImpl& lhs, _Func&& callback)
	{
		__pImpl impl(new __impl);
		impl->setFunc([prv_impl = lhs, callback = std::forward<_Func>(callback)]
		(const Result& in, const Continuation& cont)
		{
			return prv_impl->exec(in, [&cont, &callback](auto prv)
				{
					if (prv.template is<Result>())
					{
						auto& prv_value(prv.template as<Result>().value);
						if (prv_value.template is<EpsilonParsed>())
							prv_value.reset(_Return_type());
						prv_value = callback(prv_value.template as<_Return_type>());
						return cont(prv);
					}
					else
					{
						return cont(prv);
					}
				});
		});
		return impl;
	}

	template<class _Return_type,
		class _Func,
		class traits = function_traits<typename std::decay_t<_Func>>>
		static
		typename std::enable_if_t<
		(traits::arity > 1),
		__pImpl
				> callback(const __pImpl& lhs, _Func&& callback)
	{
		__pImpl impl(new __impl);
		impl->setFunc([prv_impl = lhs, callback = std::forward<_Func>(callback)]
		(const Result& in, const Continuation& cont)
		{
			return prv_impl->exec(in, [&cont, &callback](auto prv)
				{
					if (prv.template is<Result>())
					{
						auto& prv_value(prv.template as<Result>().value);
						if (is_ProductTypes<_Return_type>::value && prv_value.template is<_Return_type>())
						{
							prv_value = tuple_helper::apply(callback, ProductTypes_Get_tuple(prv_value.template as<_Return_type>()));
							return cont(prv);
						}
						else
						{
							throw std::bad_cast();
						}
					}
					else
					{
						return cont(prv);
					}
				});
		});
		return impl;
	}

	template<class _Ty>
	static
		__pImpl return_value(const __pImpl& lhs, const ReturnValue<_Ty>& _value)
	{
		__pImpl impl(new __impl);
		impl->setFunc([prv_impl = lhs, value = _value.get()]
		(const Result& in, const Continuation& cont)
		{
			return prv_impl->exec(in, [&cont, &value](auto prv)
				{
					if (prv.template is<Result>())
						prv.template as<Result>().value = value;
					return cont(prv);
				});
		});
		return impl;
	}

	template<class _Return_type>
	static
		__pImpl error_recovery(const __pImpl& lhs,
			const std::function<RecoveryParse<_Return_type>(const std::string&, size_t)>& _func)
	{
		__pImpl impl(new __impl);
		impl->setFunc([prv_impl = lhs, func = _func]
		(const Result& in, const Continuation& cont)
			{
				return prv_impl->exec(in, [&cont, &in, &func](auto prv)
					{
						if (!prv.template is<Result>())
						{
							auto recovery(func(*in.str, in.index));
							if (in.index + recovery.first > in.str->length())
								throw std::out_of_range(*in.str);
							return cont(Result
								{
									in.memorization,
									in.str,
									in.index + recovery.first,
									std::move(recovery.second),
									in._P_this,
								});
						}
						else
						{
							return cont(prv);
						}
					});
			});
		return impl;
	}

	const Parsed exec(const std::string& str) const
	{
		auto _P_this = this;
		if (!impl)
			throw std::bad_function_call();
		std::vector<std::unordered_map<const void*, LeftRecursion>> memorization(str.length() + 1);
		memorization[0][_P_this] = LeftRecursion{
			{{
				_P_this,
				{ false, 1, {} },
			}}
		};

		Parsed ret;
		impl->exec(Result
			{
				&memorization,
				&str,
				0,
				EpsilonParsed{},
				_P_this,
			}, [&ret](auto result)
			{
				ret = result;
				return ret.template is<Result>();
			});

		auto& L_Rec(memorization[0][_P_this].info[_P_this]);
		auto& result(L_Rec.result);
		L_Rec.completed = true;
		if (ret.template is<Result>())
		{
			auto& ret_struct(ret.template as<Result>());
			if (auto [pos, inserted] = result.insert(ret_struct); inserted)
			{
				return ret;
			}
			return *result.rbegin();
		}
		else if (!result.empty())
		{
			return *result.rbegin();
		}
		else
		{
			return ret;
		}
	}
};

template<class _Return_type = Parsed>
class ParserCombinator final
{
private:
	typedef ParserCombinator __self;
	typedef ParserCombinatorComponentBase __component_base;
	typedef ParserCombinatorComponentBase::Continuation Continuation;
	typedef ParserCombinatorComponentBase::Result Result;
	typedef ParserCombinatorComponent<_Return_type> __component;
	typedef std::shared_ptr<__component_base> __P_component_base;
	typedef std::shared_ptr<__component> __P_component;
	typedef std::shared_ptr<__P_component> __P_bind;
	typedef std::shared_ptr<__P_component_base> __P_bind_base;
	typedef std::vector<__P_bind_base> __reference;

	__P_bind_base bind;
	__reference reference;

	bool recursive;
	bool returnDefault;

	template<class _Return_type_1>
	friend class ParserCombinator;
	template<class _Return_type_1>
	friend class ParserCombinatorComponent;
	friend class ParserCombinatorComponentBase;

	template<class _Return_type_1>
	friend ParserCombinatorComponent<_Return_type_1> Val(const ParserCombinator<_Return_type_1>&);

	inline
		__component& _Get_component() const
	{

		return *_Get_component_ptr();
	}

	inline
		__P_component _Get_component_ptr() const
	{
		if (bind)
			return std::static_pointer_cast<__component>(*bind);
		else
			throw std::bad_function_call();
	}

	inline
		void construct_reference(typename __component::__reference& ref)
	{
		recursive = false;
		reference.reserve(ref.size());
		for (auto i : ref)
		{
			auto ptr(i.second.lock());
			if (ptr != bind)
				reference.push_back(ptr);
			else
				recursive = true;
		}
	}

public:

	typedef _Return_type return_type;

	ParserCombinator();
	ParserCombinator(const __self& rhs);
	ParserCombinator(__self&& rhs);
	ParserCombinator(const ParserCombinatorComponent<_Return_type>& rhs);
	ParserCombinator(ParserCombinatorComponent<_Return_type>&& rhs);

	__self& operator=(const __self&);
	__self& operator=(__self&&);

	template<class _Return_type_1,
		class = typename std::enable_if_t<
		!std::is_same_v<std::decay_t<_Return_type>, std::decay_t<_Return_type_1>>&&
		is_strict_constructible<std::decay_t<_Return_type>, std::decay_t<_Return_type_1>>::value,
		void
		>
	>
		__self& operator=(const ParserCombinator<_Return_type_1>&);

	template<class _Return_type_1,
		class = typename std::enable_if_t<
		!std::is_same_v<std::decay_t<_Return_type>, std::decay_t<_Return_type_1>>&&
		is_strict_constructible<std::decay_t<_Return_type>, std::decay_t<_Return_type_1>>::value,
		void
		>
	>
		__self& operator=(ParserCombinator<_Return_type_1>&&);

	__self& operator=(const ParserCombinatorComponent<_Return_type>&);

	template<class _Return_type_1,
		class = typename std::enable_if_t<
		is_strict_constructible<std::decay_t<_Return_type>, std::decay_t<_Return_type_1>>::value, void
		>
	>
		__self& operator=(const ParserCombinatorComponent<_Return_type_1>&);

	__self& operator=(ParserCombinatorComponent<_Return_type>&&);

	template<class _Return_type_1,
		class = typename std::enable_if_t<
		is_strict_constructible<std::decay_t<_Return_type>, std::decay_t<_Return_type_1>>::value, void
		>
	>
		__self& operator=(ParserCombinatorComponent<_Return_type_1>&&);

	const _Return_type operator()(const std::string& str) const;
	const _Return_type operator()(const std::string& str, size_t&) const;

	template<class _Return_type_1,
		class new_Return_type = ParserCombinatorComponent<typename PushProductTypes<_Return_type, _Return_type_1>::type>>
		new_Return_type operator+(const ParserCombinator<_Return_type_1>& rhs) const;

	template<class _Return_type_1,
		class new_Return_type = ParserCombinatorComponent<typename PushProductTypes<_Return_type, _Return_type_1>::type>>
		new_Return_type operator+(const ParserCombinatorComponent<_Return_type_1>& rhs) const;

	template<class _Return_type_1,
		class new_Return_type = ParserCombinatorComponent<typename PushAdditionTypes<_Return_type, _Return_type_1>::type>>
		new_Return_type operator|(const ParserCombinator<_Return_type_1>& rhs) const;

	template<class _Return_type_1,
		class new_Return_type = ParserCombinatorComponent<typename PushAdditionTypes<_Return_type, _Return_type_1>::type>>
		new_Return_type operator|(const ParserCombinatorComponent<_Return_type_1>& rhs) const;

	template<class _Ty,
		class new_Return_type = ParserCombinatorComponent<std::decay_t<_Ty>>>
		new_Return_type operator>>(const ReturnValue<_Ty>& _value) const;

	template<class _Func,
		class traits = function_traits<typename std::decay_t<_Func>>,
		class new_Return_type = ParserCombinatorComponent<std::decay_t<typename traits::result_type>>,
		class args_type = typename traits::args_type,
		class = typename std::enable_if_t<
		!is_ReturnValue<std::decay_t<_Func>>::value&&
		std::is_constructible_v<
		typename tuple_helper::decay<args_type>::type,
		typename ProductTypesTuple<_Return_type>::type
		>,
		void
		>
	>
		new_Return_type operator>>(_Func&& callback) const;


	template<class _Func,
		class new_Return_type = ParserCombinatorComponent<_Return_type>,
		class _Recovery_func = std::function<RecoveryParse<_Return_type>(const std::string&, size_t)>,
		class = typename std::enable_if_t<
		std::is_constructible_v<_Recovery_func, std::decay_t<_Func>>,
		void
		>
	>
		new_Return_type operator[](_Func&& _func) const;

	void ReturnDefaultWhenFail(bool flag = true)
	{
		returnDefault = flag;
	}
};

template<class _Return_type = Parsed>
class ParserCombinatorComponent final
	:public ParserCombinatorComponentBase
{
private:
	typedef ParserCombinatorComponentBase __base;
	typedef ParserCombinatorComponent __self;

	template<class _Ty>
	using __self_t = ParserCombinatorComponent<_Ty>;

	typedef ParserCombinatorComponentBase::Result Result;

	typedef std::shared_ptr<__base> __P_base;
	typedef std::weak_ptr<__P_base> __P_bind_base_weak;
	typedef std::unordered_map<void*, __P_bind_base_weak> __reference;
	__reference reference;

	ParserCombinatorComponent()
		:ParserCombinatorComponentBase(__pImpl(new __impl))
	{
		impl->setFunc([](const Result& in, const Continuation& cont)
			{
				return cont(in);
			});
	}

	ParserCombinatorComponent(const __self&) = default;
	ParserCombinatorComponent(__self&&) = default;

	ParserCombinatorComponent(const __pImpl& ptr)
		:ParserCombinatorComponentBase(ptr)
	{
	}

	ParserCombinatorComponent(__pImpl&& ptr)
		:ParserCombinatorComponentBase(std::move(ptr))
	{
	}

	explicit
		ParserCombinatorComponent(const ParserCombinator<_Return_type>& ref, bool copy = false)
		:ParserCombinatorComponentBase(ref, copy)
	{
		set_reference(ref, copy);
	}

	void set_reference(const ParserCombinator<_Return_type>& ref, bool copy)
	{
		for (auto i : ref.reference)
			reference.insert({ i.get(), i });
		if (!copy || ref.recursive)
			reference.insert({ ref.bind.get(), ref.bind });
	}

	friend class ParserCombinatorComponentBase;
	template<class _Return_type_1>
	friend class ParserCombinatorComponent;
	template<class _Return_type_1>
	friend class ParserCombinator;

	friend ParserCombinatorComponent<char> operator"" _T(char);
	friend ParserCombinatorComponent<std::string> operator"" _T(const char*, size_t);
	friend ParserCombinatorComponent<char> Token(AnyChar);
	friend ParserCombinatorComponent<char> Token(char ch);
	friend ParserCombinatorComponent<char> Token(const std::function<bool(char)>& func);
	friend ParserCombinatorComponent<std::string> Token(const char* ch);
	friend ParserCombinatorComponent<std::string> Token(const std::string& str);
	friend ParserCombinatorComponent<std::string> Token(const std::function<size_t(const std::string&)>& func);

	template<class _Return_type_1>
	friend ParserCombinatorComponent<_Return_type_1> Val(const ParserCombinator<_Return_type_1>&);

	template<class _Type>
	friend ParserCombinatorComponent<std::decay_t<_Type>> Epsilon(_Type&&);

	explicit
		ParserCombinatorComponent(const char token)
		:ParserCombinatorComponentBase(token)
	{
	}

	explicit
		ParserCombinatorComponent(const std::string& token)
		:ParserCombinatorComponentBase(token)
	{
	}

	explicit
		ParserCombinatorComponent(const std::function<bool(char)>& func)
		:ParserCombinatorComponentBase(func)
	{
	}

	explicit
		ParserCombinatorComponent(const std::function<size_t(const std::string&)>& func, Placeholder)
		:ParserCombinatorComponentBase(func)
	{
	}

	__self operator=(const __self&) = delete;
	__self operator=(__self&&) = delete;

	const _Return_type operator()(const std::string& str, bool returnDefault) const
	{
		auto ret(exec(str));
		if (!ret.template is<NoParse>())
			if (!std::is_same_v<std::decay_t<_Return_type>, EpsilonParsed> &&
				ret.template as<Result>().value.template is<EpsilonParsed>())
				return _Return_type();
			else
				return ret.template as<Result>().value.template as<_Return_type>();
		else if (returnDefault)
			return _Return_type();
		else
			throw NoParse{};
	}

	const _Return_type operator()(const std::string& str, size_t& index, bool returnDefault) const
	{
		auto ret(exec(str));
		if (!ret.template is<NoParse>())
		{
			if (!std::is_same_v<std::decay_t<_Return_type>, EpsilonParsed> &&
				ret.template as<Result>().value.template is<EpsilonParsed>())
			{
				return _Return_type();
			}
			else
			{
				auto& result(ret.template as<Result>());
				index = result.index;
				return result.value.template as<_Return_type>();
			}
		}
		else if (returnDefault)
		{
			index = 0;
			return _Return_type();
		}
		else
		{
			index = 0;
			throw NoParse{};
		}
	}

public:
	typedef _Return_type return_type;

	template<class _Return_type_1,
		class new_Return_type = __self_t<typename PushAdditionTypes<_Return_type, _Return_type_1>::type>>
		new_Return_type operator|(const __self_t<_Return_type_1>& rhs) const
	{
		auto ret(new_Return_type(__base::alternative<_Return_type, _Return_type_1>(this->impl, rhs.impl)));
		ret.reference = reference;
		ret.reference.insert(rhs.reference.begin(), rhs.reference.end());
		return ret;
	}

	template<class _Return_type_1,
		class new_Return_type = __self_t<typename PushAdditionTypes<_Return_type, _Return_type_1>::type>>
		new_Return_type operator|(const ParserCombinator<_Return_type_1>& rhs) const
	{
		__self_t<_Return_type_1> rhs_c(rhs);
		auto ret(new_Return_type(__base::alternative<_Return_type, _Return_type_1>(this->impl, rhs_c.impl)));
		ret.reference = reference;
		ret.reference.insert(rhs_c.reference.begin(), rhs_c.reference.end());
		return ret;
	}

	template<class _Return_type_1,
		class new_Return_type = __self_t<typename PushProductTypes<_Return_type, _Return_type_1>::type>>
		new_Return_type operator+(const __self_t<_Return_type_1>& rhs) const
	{
		auto ret(new_Return_type(__base::connect<_Return_type, _Return_type_1>(this->impl, rhs.impl)));
		ret.reference = reference;
		ret.reference.insert(rhs.reference.begin(), rhs.reference.end());
		return ret;
	}

	template<class _Return_type_1,
		class new_Return_type = __self_t<typename PushProductTypes<_Return_type, _Return_type_1>::type>>
		new_Return_type operator+(const ParserCombinator<_Return_type_1>& rhs) const
	{
		__self_t<_Return_type_1> rhs_c(rhs);
		auto ret(new_Return_type(__base::connect<_Return_type, _Return_type_1>(this->impl, rhs_c.impl)));
		ret.reference = reference;
		ret.reference.insert(rhs_c.reference.begin(), rhs_c.reference.end());
		return ret;
	}

	template<class _Ty,
		class new_Return_type = __self_t<std::decay_t<_Ty>>>
		new_Return_type operator>>(const ReturnValue<_Ty>& _value) const
	{
		auto ret(new_Return_type(__base::return_value(this->impl, _value)));
		ret.reference = reference;
		return ret;
	}

	template<class _Func,
		class traits = function_traits<typename std::decay_t<_Func>>,
		class new_Return_type = __self_t<std::decay_t<typename traits::result_type>>,
		class args_type = typename traits::args_type,
		class = typename std::enable_if_t<
		!is_ReturnValue<std::decay_t<_Func>>::value&&
		std::is_constructible_v<
		typename tuple_helper::decay<args_type>::type,
		typename ProductTypesTuple<_Return_type>::type
		>,
		void
		>
	>
		new_Return_type operator>>(_Func&& _Callback) const
	{
		auto ret(new_Return_type(__base::callback<_Return_type>(this->impl, std::forward<_Func>(_Callback))));
		ret.reference = reference;
		return ret;
	}

	template<class _Func,
		class _Recovery_func = std::function<RecoveryParse<_Return_type>(const std::string&, size_t)>,
		class = typename std::enable_if_t<
		std::is_constructible<_Recovery_func, std::decay_t<_Func>>::value,
		void
		>
	>
		__self operator[](_Func&& _func) const
	{
		auto ret(__self(__base::error_recovery(this->impl, _Recovery_func(std::forward<_Func>(_func)))));
		ret.reference = reference;
		return ret;
	}
};

#define __PTR_TO_BASE(ptr) static_cast<ParserCombinatorComponentBase*>(ptr)
#define __NEW_BIND(ptr) std::make_shared<__P_component_base>(__PTR_TO_BASE(ptr))

template<class _Return_type>
inline
ParserCombinator<_Return_type>::ParserCombinator()
	:bind(__NEW_BIND(new ParserCombinatorComponent<_Return_type>())),
	recursive(false),
	returnDefault(false)
{
}

template<class _Return_type>
inline
ParserCombinator<_Return_type>::ParserCombinator(const __self& rhs)
	:bind(std::make_shared<__P_component_base>(*rhs.bind)),
	reference(rhs.reference),
	recursive(rhs.recursive),
	returnDefault(rhs.returnDefault)
{
	if (recursive)
		reference.push_back(rhs.bind);
}

template<class _Return_type>
inline
ParserCombinator<_Return_type>::ParserCombinator(__self&& rhs)
	:bind(std::move(rhs.bind)),
	reference(std::move(rhs.reference)),
	recursive(std::move(rhs.recursive)),
	returnDefault(std::move(rhs.returnDefault))
{
}

template<class _Return_type>
inline
ParserCombinator<_Return_type>::ParserCombinator(const ParserCombinatorComponent<_Return_type>& rhs)
{
	construct_reference(rhs.reference);
	bind = __NEW_BIND(new ParserCombinatorComponent<_Return_type>(rhs));
}

template<class _Return_type>
inline
ParserCombinator<_Return_type>::ParserCombinator(ParserCombinatorComponent<_Return_type>&& rhs)
{
	construct_reference(rhs.reference);
	bind = __NEW_BIND(new ParserCombinatorComponent<_Return_type>(std::move(rhs)));
}

template<class _Return_type>
inline
ParserCombinator<_Return_type>& ParserCombinator<_Return_type>::operator=(const __self& other)
{
	reference = other.reference;
	*bind = *other.bind;
	recursive = other.recursive;
	returnDefault = other.returnDefault;
	if (recursive)
		reference.push_back(other.bind);
	return *this;
}

template<class _Return_type>
inline
ParserCombinator<_Return_type>& ParserCombinator<_Return_type>::operator=(__self&& other)
{
	reference = std::move(other.reference);
	bind = std::move(other.bind);
	recursive = std::move(other.recursive);
	returnDefault = std::move(other.returnDefault);
	return *this;
}

template<class _Return_type>
template<class _Return_type_1, class>
inline
ParserCombinator<_Return_type>& ParserCombinator<_Return_type>::operator=(const ParserCombinator<_Return_type_1>& other)
{
	reference = other.reference;
	bind->reset(__PTR_TO_BASE(new ParserCombinatorComponent<_Return_type>()));
	recursive = other.recursive;
	returnDefault = other.returnDefault;
	if (recursive)
		reference.push_back(other.bind);
	_Get_component_ptr()->impl->setFunc([impl = std::move(ParserCombinatorComponent<_Return_type_1>(other).impl)]
	(const Result& in, const Continuation& cont)
	{
		return impl->exec(in, [&cont](auto ret)
			{
				if (ret.template is<Result>())
				{
					auto& ret_value(ret.template as<Result>().value);
					ret_value = static_cast<std::decay_t<_Return_type>>(ret_value.template as<_Return_type_1>());
				}
				return cont(ret);
			});
	});
	return *this;
}

template<class _Return_type>
template<class _Return_type_1, class>
inline
ParserCombinator<_Return_type>& ParserCombinator<_Return_type>::operator=(ParserCombinator<_Return_type_1>&& other)
{
	reference = std::move(other.reference);
	bind->reset(__PTR_TO_BASE(new ParserCombinatorComponent<_Return_type>()));
	recursive = std::move(other.recursive);
	returnDefault = std::move(other.returnDefault);
	if (recursive)
		reference.push_back(other.bind);
	_Get_component_ptr()->impl->setFunc([impl = std::move(ParserCombinatorComponent<_Return_type_1>(std::move(other)).impl),
		ref = std::move(other.bind)]
		(const Result& in, const Continuation& cont)
	{
		return impl->exec(in, [&cont](auto ret)
			{
				if (ret.template is<Result>())
				{
					auto& ret_value(ret.template as<Result>().value);
					ret_value = static_cast<std::decay_t<_Return_type>>(ret_value.template as<_Return_type_1>());
				}
				return cont(ret);
			});
	});
	return *this;
}

template<class _Return_type>
inline
ParserCombinator<_Return_type>& ParserCombinator<_Return_type>::operator=(const ParserCombinatorComponent<_Return_type>& other)
{
	decltype(reference) tmp;
	std::swap(reference, tmp);
	construct_reference(other.reference);
	bind->reset(__PTR_TO_BASE(new ParserCombinatorComponent<_Return_type>(other)));
	return *this;
}

template<class _Return_type>
template<class _Return_type_1, class>
inline
ParserCombinator<_Return_type>& ParserCombinator<_Return_type>::operator=(const ParserCombinatorComponent<_Return_type_1>& other)
{
	decltype(reference) tmp;
	std::swap(reference, tmp);
	construct_reference(other.reference);
	bind->reset(__PTR_TO_BASE(new ParserCombinatorComponent<_Return_type>()));
	_Get_component_ptr()->impl->setFunc([impl = other.impl]
	(const Result& in, const Continuation& cont)
	{
		return impl->exec(in, [&cont](auto ret)
			{
				if (ret.template is<Result>())
				{
					auto& ret_value(ret.template as<Result>().value);
					ret_value = static_cast<std::decay_t<_Return_type>>(ret_value.template as<_Return_type_1>());
				}
				return cont(ret);
			});
	});
	return *this;
}

template<class _Return_type>
inline
ParserCombinator<_Return_type>& ParserCombinator<_Return_type>::operator=(ParserCombinatorComponent<_Return_type>&& other)
{
	decltype(reference) tmp;
	std::swap(reference, tmp);
	construct_reference(other.reference);
	bind->reset(__PTR_TO_BASE(new ParserCombinatorComponent<_Return_type>(std::move(other))));
	return *this;
}

template<class _Return_type>
template<class _Return_type_1, class>
inline
ParserCombinator<_Return_type>& ParserCombinator<_Return_type>::operator=(ParserCombinatorComponent<_Return_type_1>&& other)
{
	decltype(reference) tmp;
	std::swap(reference, tmp);
	construct_reference(other.reference);
	bind->reset(__PTR_TO_BASE(new ParserCombinatorComponent<_Return_type>()));
	_Get_component_ptr()->impl->setFunc([impl = std::move(other.impl)]
	(const Result& in, const Continuation& cont)
	{
		return impl->exec(in, [&cont](auto ret)
			{
				if (ret.template is<Result>())
				{
					auto& ret_value(ret.template as<Result>().value);
					ret_value = static_cast<std::decay_t<_Return_type>>(ret_value.template as<_Return_type_1>());
				}
				return cont(ret);
			});
	});
	return *this;
}

#undef __PTR_TO_BASE

template<class _Return_type>
inline
const _Return_type ParserCombinator<_Return_type>::operator()(const std::string& str) const
{
	return _Get_component()(str, returnDefault);
}

template<class _Return_type>
inline
const _Return_type ParserCombinator<_Return_type>::operator()(const std::string& str, size_t& index) const
{
	return _Get_component()(str, index, returnDefault);
}

template<class _Return_type>
template<class _Return_type_1, class new_Return_type>
inline
new_Return_type ParserCombinator<_Return_type>::operator|(const ParserCombinatorComponent<_Return_type_1>& rhs) const
{
	ParserCombinatorComponent<_Return_type> lhs_c(*this);
	return lhs_c | rhs;
}

template<class _Return_type>
template<class _Return_type_1, class new_Return_type>
inline
new_Return_type ParserCombinator<_Return_type>::operator|(const ParserCombinator<_Return_type_1>& rhs) const
{
	ParserCombinatorComponent<_Return_type> lhs_c(*this);
	ParserCombinatorComponent<_Return_type_1> rhs_c(rhs);
	return lhs_c | rhs_c;
}

template<class _Return_type>
template<class _Return_type_1, class new_Return_type>
inline
new_Return_type ParserCombinator<_Return_type>::operator+(const ParserCombinatorComponent<_Return_type_1>& rhs) const
{
	ParserCombinatorComponent<_Return_type> lhs_c(*this);
	return lhs_c + rhs;
}

template<class _Return_type>
template<class _Return_type_1, class new_Return_type>
inline
new_Return_type ParserCombinator<_Return_type>::operator+(const ParserCombinator<_Return_type_1>& rhs) const
{
	ParserCombinatorComponent<_Return_type> lhs_c(*this);
	ParserCombinatorComponent<_Return_type_1> rhs_c(rhs);
	return lhs_c + rhs_c;
}

template<class _Return_type>
template<class _Ty,
	class new_Return_type>
	inline
	new_Return_type ParserCombinator<_Return_type>::operator>>(const ReturnValue<_Ty>& _value) const
{
	return ParserCombinatorComponent<_Return_type>(*this) >> _value;
}

template<class _Return_type>
template<class _Func,
	class traits,
	class new_Return_type,
	class args_type,
	class>
	inline
	new_Return_type ParserCombinator<_Return_type>::operator>>(_Func&& callback) const
{
	return ParserCombinatorComponent<_Return_type>(*this) >> callback;
}

template<class _Return_type>
template<class _Func,
	class new_Return_type,
	class _Recovery_func,
	class>
	inline
	new_Return_type ParserCombinator<_Return_type>::operator[](_Func&& _func) const
{
	return ParserCombinatorComponent<_Return_type>(*this)[std::forward<_Func>(_func)];
}

inline
ParserCombinatorComponent<char> Token(AnyChar)
{
	return ParserCombinatorComponent<char>([](char) { return true; });
}

inline
ParserCombinatorComponent<char> Token(char ch)
{
	return ParserCombinatorComponent<char>(ch);
}

inline
ParserCombinatorComponent<char> Token(const std::function<bool(char)>& func)
{
	return ParserCombinatorComponent<char>(func);
}

inline
ParserCombinatorComponent<std::string> Token(const char* ch)
{
	return ParserCombinatorComponent<std::string>(std::string(ch));
}

inline
ParserCombinatorComponent<std::string> Token(const std::string& str)
{
	return ParserCombinatorComponent<std::string>(str);
}

inline
ParserCombinatorComponent<std::string> Token(const std::function<size_t(const std::string&)>& func)
{
	return ParserCombinatorComponent<std::string>(func, Placeholder{});
}

inline
ParserCombinatorComponent<char> operator"" _T(char ch)
{
	return Token(ch);
}

inline
ParserCombinatorComponent<std::string> operator"" _T(const char* ch, size_t)
{
	return Token(std::string(ch));
}

template<class _Return_type>
inline
ParserCombinatorComponent<_Return_type> Val(const ParserCombinator<_Return_type>& value)
{
	return (ParserCombinatorComponent<_Return_type>(value, true));
}

template<class _Type>
inline
ParserCombinatorComponent<std::decay_t<_Type>> Epsilon(_Type&& value)
{
	return (ParserCombinatorComponent<int>() >>
		[value = std::forward<_Type>(value)](int)->std::decay_t<_Type>
	{
		return value;
	});
}

template<class _Type,
	class _Return_type = std::decay_t<_Type>>
	inline
	ReturnValue<_Return_type> Return(_Type&& value)
{
	return ReturnValue<_Return_type>(std::forward<_Type>(value));
}

template<class _Type>
inline
RecoveryParse<_Type> Recovery(size_t skip, _Type&& value)
{
	return std::make_pair(skip, std::forward<_Type>(value));
}

NAMESPACE_END

#ifdef __ENABLE_MEMORIZATION__
#undef __ENABLE_MEMORIZATION__
#endif

#endif