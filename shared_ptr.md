巧了，12.9 也让自己实现一个

所以在上一篇文章中，我介绍了一个基本的唯一指针，其中智能指针保留了指针的唯一所有权。 我们遇到的另一个常见的智能指针是共享指针（SP）。 在这种情况下，指针的所有权在多个 SP 实例之间共享，并且只有在所有 SP 实例都被销毁时才释放（删除）指针。

因此，我们不仅必须存储指针，而且还需要一种机制来跟踪共享指针所有权的所有 SP 实例。 当最后一个 SP 实例被销毁时，它也会删除指针（最后一个所有者清理。与最后一个离开房间的原理类似，将灯熄灭）。

共享指针上下文析构函数:
```cpp
namespace ThorsAnvil
{
    template<typename T>
    class SP
    {
        T*  data;
        public:
            ~SP()
            {
                if (amITheLastOwner())
                {
                    delete data;
                }
            }
    };
}
```

跟踪指针的共享所有者有两种主要技术：

记数：

当计数为 1 时，您是最后一个所有者。
这是一种非常简单且合乎逻辑的技术。您有一个共享计数器，它随着 SP 实例获取/释放指针的所有权而递增/递减。缺点是您需要动态分配必须管理的内存，并且在线程环境中您需要序列化对计数器的访问。

使用所有者的链表：

当您是列表中的唯一成员时，您就是最后一个所有者。
当一个 SP 实例取得/释放指针的所有权时，它们被添加到/从链表中删除。这稍微复杂一些，因为您需要维护一个循环链表（对于 O(1)）。优点是您不需要为计数管理任何单独的内存（一个 SP 实例只是指向链中的下一个 SP 实例）并且在线程环境中添加/删除共享指针不需要总是被序列化（尽管您仍然需要锁定您的邻居以确保完整性）。

共享计数

两者中更容易正确实施的是列表版本。没有真正的陷阱（我见过）。尽管人们确实很难从循环列表中插入和删除链接。我计划在某个时候写另一篇文章，所以我会在那时介绍它。

共享计数基本上是 std::shared_ptr 使用的技术（尽管它们存储的数量略多于计数以尝试提高效率，请参阅 std::make_shared ）。

我从初学者那里看到的主要错误是没有使用动态分配的计数器（即他们将计数器保留在 SP 对象中）。您必须为计数器动态分配内存，以便它可以被所有 SP 实例共享（您无法确定会有多少或它们将被删除的顺序）。

您还必须序列化对此计数器的访问，以确保在线程环境中正确维护计数。在第一个版本中，为简单起见，我将只考虑单线程环境，因此不需要同步。

第一次尝试：

```cpp
namespace ThorsAnvil
{
    template<typename T>
    class SP
    {
        T*      data;
        int*    count;
        public:
            // Remember from ThorsAnvil::UP that the constructor
            // needs to be explicit to prevent the compiler creating
            // temporary objects on the fly.
            explicit SP(T* data)
                : data(data)
                , count(new int(1))
            {}
            ~SP()
            {
                --(*count);
                if (*count == 0)
                {
                    delete data;
                }
            }
            // Remember from ThorsAnvil::UP that we need to make sure we
            // obey the rule of three. So we will implement the copy
            // constructor and assignment operator.
            SP(SP const& copy)
                : data(copy.data)
                , count(copy.count)
            {
                ++(*count);
            }
            SP& operator=(SP const& rhs)
            {
                // Keep a copy of the old data
                T*   oldData  = data;
                int* oldCount = count;

                // now we do an exception safe transfer;
                data  = rhs.data;
                count = rhs.count;

                // Update the counters
                ++(*count);
                --(*oldCount);

                // Finally delete the old pointer if required.
                if (*oldCount == 0)
                {
                    delete oldData;
                }
            }
            // Const correct access owned object
            T* operator->() const {return data;}
            T& operator*()  const {return *data;}

            // Access to smart pointer state
            T* get()                 const {return data;}
            explicit operator bool() const {return data;}
    };
}
```

问题 1：潜在的构造函数失败
当开发人员（尝试）创建 SP 时，他们将指针的所有权移交给 SP 实例。一旦构造函数启动，开发人员就会期望不需要进一步的检查。但是写的代码有问题。

在 C++ 中，通过 new 分配内存不会失败（与 C 不同，malloc() 在失败时可以返回 Null）。在 C++ 中，通过标准 new 分配内存失败会产生 std::bad_alloc 异常。
此外，如果我们从构造函数中抛出异常，则在实例的生命周期结束时，将永远不会调用析构函数（因为仅在完全形成的对象上才会调用析构函数）。

因此，如果在构造过程中抛出异常（因为上述原因所以不会调用析构函数），我们必须负责确保在异常逃脱出构造函数之前删除指针，否则将导致指针泄漏。

构造函数负责指针:
```cpp

namespace ThorsAnvil
{
    //.....
        explicit SP(T* data)
        : data(data)
        , count(new (std::nothorw) int(1)) // use the no throw version of new.
    {
        // Check if the pointer correctly allocated
        if (count == nullptr)
        {
            // If we failed then delete the pointer
            // and manually throw the exception.
            delete data;
            throw std::bad_alloc();
        }
    }
    // or
    .....
        explicit SP(T* data)
        // The rarely used try/catch for exceptions in argument lists.
        try
        : data(data)
        , count(new int(1))
    {}
    catch (...)
    {
        // If we failed because of an exception
        // delete the pointer and rethrow the exception.
        delete data;
        throw;
    }
}
```

问题2，DRY原则与赋值运算发
目前赋值运算符是异常安全的，并且符合强异常保证，所以这里没有真正的问题。 但是类中似乎有很多重复的代码。
再看一下赋值运算符：
```cpp
namespace ThorsAnvil
{
     .....
            SP& operator=(SP const& rhs)
            {
                T*   oldData  = data;
                int* oldCount = count;

                data  = rhs.data;
                count = rhs.count;
                ++(*count);

                --(*oldCount);
                if (*oldCount == 0)
                {
                    delete oldData;
                }
            }
}
```
这些地方看起来已经写过了
```cpp
// This looks like the SP copy constructor.
                data  = rhs.data;
                count = rhs.count;
                ++(*count);

// This looks like the SP destructor.
                --(*oldCount);
                if (*oldCount == 0)
                {
                    delete oldData;
                }
```
所以我们可以用复制和交换原则来重写它：
```cpp
SP& operator=(SP const& rhs)
{
    // constructor of tmp handles increment.
    SP tmp(rhs);

    std::swap(data,  tmp.data);
    std::swap(count, tmp.count);
    return *this;
}   // the destructor of tmp is executed here.
    // this handles the decrement and release of the pointer

// This is usually simplified further into
SP& operator=(SP rhs) // Note implicit copy because of pass by value.
{
    rhs.swap(*this);  // swaps moved to swap method.
    return *this;
}
```

于是最终实现长这样；

```cpp
namespace ThorsAnvil
{
    template<typename T>
    class SP
    {
        T*      data;
        int*    count;
        public:
            // Explicit constructor
            explicit SP(T* data)
            try
                : data(data)
                , count(new int(1))
            {}
            catch(...)
            {
                // If we failed because of an exception
                // delete the pointer and rethrow the exception.
                delete data;
                throw;
            }
            ~SP()
            {
                --(*count);
                if (*count == 0)
                {
                    delete data;
                }
            }
            SP(SP const& copy)
                : data(copy.data)
                , count(copy.count)
            {
                ++(*count);
            }
            // Use the copy and swap idiom
            // It works perfectly for this situation.
            SP& operator=(SP rhs)
            {
                rhs.swap(*this);
                return *this;
            }
            SP& operator=(T* newData)
            {
                SP tmp(newData);
                tmp.swap(*this);
                return *this;
            }
            // Always good to have a swap function
            // Make sure it is noexcept
            void swap(SP& other) noexcept
            {
                std::swap(data,  other.data);
                std::swap(count, other.count);
            }
            // Const correct access owned object
            T* operator->() const {return data;}
            T& operator*()  const {return *data;}

            // Access to smart pointer state
            T* get()                 const {return data;}
            explicit operator bool() const {return data;}
        };
}
```
