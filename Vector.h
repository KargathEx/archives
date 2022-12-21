//三百行的vector实现. from: https://lokiastari.com/blog/2016/03/20/vector-the-other-stuff/index.html
template<typename T>
class Vector
{
public:
       using value_type = T;
       using reference = T&;
       using const_reference = T const&;
       using pointer = T*;
       using const_pointer = T const*;
       using iterator = T*;
       using const_iterator = T const*;
       using riterator = std::reverse_iterator<iterator>;
       using const_riterator = std::reverse_iterator<const_iterator>;
       using difference_type = std::ptrdiff_t;
       using size_type = std::size_t;
private:
       size_type       capacity;
       size_type       length;
       T* buffer;
       struct Deleter
       {
              void operator()(T* buffer) const
              {
                      ::operator delete(buffer);
              }
       };
public:
       Vector(int capacity = 10)
              : capacity(capacity)
              , length(0)
              , buffer(static_cast<T*>(::operator new(sizeof(T)* capacity)))
       {}
       template<typename I>
       Vector(I begin, I end)
              : capacity(std::distance(begin, end))
              , length(0)
              , buffer(static_cast<T*>(::operator new(sizeof(T)* capacity)))
       {
              for (auto loop = begin; loop != end; ++loop)
              {
                      pushBackInternal(*loop);
              }
       }
       Vector(std::initializer_list<T> const& list)
              : Vector(std::begin(list), std::end(list))
       {}
       ~Vector()
       {
              // Make sure the buffer is deleted even with exceptions
              // This will be called to release the pointer at the end
              // of scope.
              std::unique_ptr<T, Deleter>     deleter(buffer, Deleter());
              clearElements<T>();
       }
       Vector(Vector const& copy)   //为什么这个没有那样的问题？ 因为它没有把copy赋值给non-const的东西。
              : capacity(copy.length)
              , length(0)
              , buffer(static_cast<T*>(::operator new(sizeof(T)* capacity)))
       {
              try
              {
                      for (int loop = 0; loop < copy.length; ++loop)
                      {
                             push_back(copy.buffer[loop]);
                      }
              }
              catch (...)
              {
                      std::unique_ptr<T, Deleter>     deleter(buffer, Deleter());
                      clearElements<T>();
                      // Make sure the exceptions continue propagating after
                      // the cleanup has completed.
                      throw;
              }
       }
       Vector& operator=(Vector const& copy)
       {
              copyAssign<T>(copy);
              return *this;
       }
       Vector(Vector&& move) noexcept
              : capacity(0)
              , length(0)
              , buffer(nullptr)
       {
              move.swap(*this);
       }
       Vector& operator=(Vector&& move) noexcept
       {
              move.swap(*this);
              return *this;
       }
       void swap(Vector& other) noexcept
       {
              using std::swap;
              swap(capacity, other.capacity);
              swap(length, other.length);
              swap(buffer, other.buffer);
       }
       // Non-Mutating functions
       size_type           size() const { return length; }
       bool                empty() const { return length == 0; }
       // Validated element access
       reference           at(size_type index) { validateIndex(index); return  buffer[index]; }
       const_reference     at(size_type index) const { validateIndex(index); return  buffer[index]; }
       // Non-Validated element access
       reference           operator[](size_type index) { return buffer[index]; }
       const_reference     operator[](size_type index) const { return buffer[index]; }
       reference           front() { return buffer[0]; }
       const_reference     front() const { return buffer[0]; }
       reference           back() { return buffer[length - 1]; }
       const_reference     back() const { return buffer[length - 1]; }
       // Iterators
       iterator            begin() { return buffer; }
       riterator           rbegin() { return riterator(end()); }
       const_iterator      begin() const { return buffer; }
       const_riterator     rbegin() const { return const_riterator(end()); }
       iterator            end() { return buffer + length; }
       riterator           rend() { return riterator(begin()); }
       const_iterator      end() const { return buffer + length; }
       const_riterator     rend() const { return const_riterator(begin()); }
       const_iterator      cbegin() const { return begin(); }
       const_riterator     crbegin() const { return rbegin(); }
       const_iterator      cend() const { return end(); }
       const_riterator     crend() const { return rend(); }
       // Comparison
       bool operator!=(Vector const& rhs) const { return !(*this == rhs); }
       bool operator==(Vector const& rhs) const
       {
              return  (size() == rhs.size())
                      && std::equal(begin(), end(), rhs.begin());
       }
       // Mutating functions
       void push_back(value_type const& value)
       {
              resizeIfRequire();
              pushBackInternal(value);
       }
       void push_back(value_type&& value)
       {
              resizeIfRequire();
              moveBackInternal(std::move(value));
       }
       template<typename... Args>
       void emplace_back(Args&&... args)
       {
              resizeIfRequire();
              emplaceBackInternal(std::move(args)...);
       }
       void pop_back()
       {
              --length;
              buffer[length].~T();
       }
       void reserve(size_type capacityUpperBound)
       {
              if (capacityUpperBound > capacity)
              {
                      reserveCapacity(capacityUpperBound);
              }
       }
private:
       void validateIndex(size_type index) const
       {
              if (index >= length)
              {
                      throw std::out_of_range("Out of Range");
              }
       }
       void resizeIfRequire()
       {
              if (length == capacity)
              {
                      size_type     newCapacity = std::max(2.0, capacity * 1.5);
                      reserveCapacity(newCapacity);
              }
       }
       void reserveCapacity(size_type newCapacity)
       {
              Vector<T>  tmpBuffer(newCapacity);
              simpleCopy<T>(tmpBuffer);
              tmpBuffer.swap(*this);
       }
       // Add new element to the end using placement new
       void pushBackInternal(T const& value)
       {
              new (buffer + length) T(value);
              ++length;
       }
       void moveBackInternal(T&& value)
       {
              new (buffer + length) T(std::move(value));
              ++length;
       }
       template<typename... Args>
       void emplaceBackInternal(Args&&... args)
       {
              new (buffer + length) T(std::move(args)...);     //终于不是傻看书而是能用一下这些东西了.. placement new.
              ++length;
       }
       // Optimizations that use SFINAE to only instantiate one
       // of two versions of a function.
       //      simpleCopy()        Moves when no exceptions are guaranteed, otherwise  copies.
       //      clearElements()     When no destructor remove loop.
       //      copyAssign()        Avoid resource allocation when no exceptions  guaranteed.
       //                          ie. When copying integers reuse the buffer if we can
       //                          to avoid expensive resource allocation.
       template<typename X>
       typename std::enable_if<std::is_nothrow_move_constructible<X>::value ==  false>::type
              simpleCopy(Vector<T>& dst)
       {
              std::for_each(buffer, buffer + length,
                      [&dst](T const& v) {dst.pushBackInternal(v); }
              );
       }
       template<typename X>
       typename std::enable_if<std::is_nothrow_move_constructible<X>::value ==  true>::type
              simpleCopy(Vector<T>& dst)
       {
              std::for_each(buffer, buffer + length,
                      [&dst](T& v) {dst.moveBackInternal(std::move(v)); }
              );
       }
       template<typename X>
       typename std::enable_if<std::is_trivially_destructible<X>::value ==  false>::type
              clearElements()
       {
              // Call the destructor on all the members in reverse order
              for (int loop = 0; loop < length; ++loop)
              {
                      // Note we destroy the elements in reverse order.
                      buffer[length - 1 - loop].~T();
              }
       }
       template<typename X>
       typename std::enable_if<std::is_trivially_destructible<X>::value == true>::type
              clearElements()
       {
              // Trivially destructible objects can be reused without using the  destructor.
       }
       template<typename X>
       typename std::enable_if<(std::is_nothrow_copy_constructible<X>::value
              && std::is_nothrow_destructible<X>::value) == true>::type
              copyAssign(const Vector<X>& copy)
       {
              // This function is only used if there is no chance of an exception  being
              // thrown during destruction or copy construction of the type T.
              // Quick return for self assignment.
              if (this == &copy)
              {
                      return;
              }
              if (capacity >= copy.length) //!如果容积大于要拷贝的东西，就直接复制过来，这里是作者写错了。
              {
                      // If we have enough space to copy then reuse the space we  currently
                      // have to avoid the need to perform an expensive resource  allocation.
                      clearElements<T>();     // Potentially does nothing (see above)
                                                                 // But if required  will call the destructor of
                                                                 // all elements.
                      // buffer now ready to get a copy of the data.
                      length = 0;
                      for (int loop = 0; loop < copy.length; ++loop)
                      {
                             pushBackInternal(copy[loop]);
                      }
              }
              else
              {
                      // Fallback to copy and swap if we need to more space anyway
                      Vector<T>  tmp(copy);
                      tmp.swap(*this);
              }
       }
       template<typename X>
       typename std::enable_if<(std::is_nothrow_copy_constructible<X>::value
              && std::is_nothrow_destructible<X>::value) == false>::type
              copyAssign( const Vector<X>& copy)
       {
              // Copy and Swap idiom
              Vector<T>  tmp(copy);
              tmp.swap(*this);
       }
};
