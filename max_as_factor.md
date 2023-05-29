函数不能当factor,所以需要包一层: [来源](https://github.com/codereport/useful_function_objects/blob/main/ufo.hpp)
```cpp
#include <algorithm>
// UFO = Useful Function Objects
// ufo::min{}
// ufo::max{}
// ufo::abs_diff{}
namespace ufo {

   // STRUCT TEMPLATE min
  template<class _Ty = void> struct min {
    constexpr _Ty operator()(const _Ty& _Left, const _Ty& _Right) const {
      return (std::min(_Left, _Right));
    }
  };

  // STRUCT TEMPLATE SPECIALIZATION min
  template<>
  struct min<void> { 
    // transparent functor for operator*
      typedef int is_transparent;
      template<class _Ty1, class _Ty2>
      constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const 
        -> decltype(std::min(static_cast<_Ty1&&>(_Left), static_cast<_Ty2&&>(_Right))) {
          return (std::min(static_cast<_Ty1&&>(_Left), static_cast<_Ty2&&>(_Right)));
      }
  };

  // STRUCT TEMPLATE max
  template<class _Ty = void> struct max {
    constexpr _Ty operator()(const _Ty& _Left, const _Ty& _Right) const {
      return (std::max(_Left, _Right));
    }
  };

  // STRUCT TEMPLATE SPECIALIZATION max
  template<>
  struct max<void> { 
    // transparent functor for operator*
      typedef int is_transparent;
      template<class _Ty1, class _Ty2>
      constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const 
        -> decltype(std::max(static_cast<_Ty1&&>(_Left), static_cast<_Ty2&&>(_Right))) {
          return (std::max(static_cast<_Ty1&&>(_Left), static_cast<_Ty2&&>(_Right)));
      }
  };

   // STRUCT TEMPLATE abs_diff
  template<class _Ty = void> struct abs_diff {
    constexpr _Ty operator()(const _Ty& _Left, const _Ty& _Right) const {
      return (std::abs(_Left - _Right));
    }
  };

  // STRUCT TEMPLATE SPECIALIZATION abs_diff
  template<>
  struct abs_diff<void> { 
    // transparent functor for operator*
      typedef int is_transparent;
      template<class _Ty1, class _Ty2>
      constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const 
        -> decltype(std::abs(static_cast<_Ty1&&>(_Left) - static_cast<_Ty2&&>(_Right))) {
          return (std::abs(static_cast<_Ty1&&>(_Left) - static_cast<_Ty2&&>(_Right)));
      }
  };
}
```
等价的写法是 [来源](https://github.com/codereport/blackbird/blob/main/combinators.hpp#LL55C1-L55C61)
```cpp
auto _max_  = [](auto a, auto b) { return std::max(a, b); };
```
使用场景[来源](https://www.youtube.com/watch?v=qNKBdqifxpU&t=407s)  
原来旧时代的人要写这么多东西.   
学新东西，方便的还是用户自己。

