
#include <iostream>
template<class Class>
class Array
{
public:
  std::size_t size;
  Array() :size{ 0 }, values{ nullptr } {};
  Array(std::initializer_list<Class>);
  Array(const Array<Class>&);
  Class& operator [] (std::size_t index); //为了可以修改元素值，需要返回左值
  Class operator [] (std::size_t index) const;
  Array<Class>& operator = (const Array<Class>&);
  ~Array();

private:
  Class* values;
};
template<class Class>
Array<Class>::Array(std::initializer_list<Class> initList)
{
  size = initList.size();
  values = new Class[size];
  std::size_t index = 0;
  for (Class initListValue : initList)
  {
    values[index] = initListValue;
    ++index;
  }
}

template<class Class>
Array<Class>::Array(const Array<Class>& anArray)
{
  size = anArray.size;
  values = new Class[size];
  for (std::size_t i = 0; i < size; ++i)
  {
    values[i] = anArray[i];
  }
}

template<class Class>
Class& Array<Class>::operator [] (std::size_t index)
{
  return values[index];
}

template<class Class>
Class Array<Class>::operator [] (std::size_t index) const
{
  return values[index];
}

template<class Class>
Array<Class>& Array<Class>::operator = (const Array<Class>& anArray)
{
  if (this != &anArray)
  {
    delete[] values;
    values = new Class[anArray.size];
    size = anArray.size;  //!我竟然调了半天，最后还是看提示才确定了的。
    for (std::size_t i = 0; i < anArray.size; ++i)
    {
      values[i] = anArray[i];
    }
  }
  return *this;
}

template<class Class>
Array<Class>::~Array()
{
  delete[] values;
}

template<class Class>
void print1DArray(const Array<Class>& anArray)
{
  for (std::size_t x = 0; x < anArray.size; ++x)
  {
    std::cout << anArray[x] << " ";
    //cout << "|";
  }
  std::cout << "\n";
}

template<class Class>
void print2DArray(const Array<Array<Class>>& anArray)
{

  for (std::size_t x = 0; x < anArray.size; ++x)
  {
    for (std::size_t y = 0; y < anArray[0].size; ++y)
    {
      std::cout << anArray[x][y] << " ";
    }
    std::cout << "\n";
  }
}

int main()
{
  Array<Array<int>> val =
  {
    /*Array<int>*/ {2, 1, 3},
    /*Array<int> */{1, 0, 9},
    /*Array<int> */{2, 3, 0},
  };

  val[2][0] = 9;
  print2DArray(val);
}