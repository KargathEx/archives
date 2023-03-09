// myString.cpp : This file contains the 'main' function. Program execution begins and ends there.
//记录关于文本处理的万千烂事，就该扔给encoding.h来管！

#include <string>
#include<memory>
#include<iostream>
#include<type_traits>
#include<fstream>
#include <Windows.h>
using std::shared_ptr;
class String
{
  shared_ptr<wchar_t[]> buffer;
  char* multiByteBuffer;
  int length;
  void Free()
  {
    if (buffer) buffer = 0;
    if (multiByteBuffer) delete[] multiByteBuffer;
    buffer = 0;
    multiByteBuffer = 0;
    length = 0;
  }

public:
  static String FromBuffer(shared_ptr<wchar_t[]> buffer, int len)
  {
    String rs;
    rs.buffer = buffer;
    rs.length = len;
    return rs;
  }
  String()
    :buffer(0), multiByteBuffer(0), length(0)
  {}
  String(const wchar_t* str)
    :buffer(0), multiByteBuffer(0), length(0)
  {
    this->operator=(str);
  }
  String(const wchar_t ch)
    :buffer(0), multiByteBuffer(0), length(0)
  {
    wchar_t arr[] = { ch, 0 };
    *this = String(arr);
  }
  String(const char* str)
    :buffer(0), multiByteBuffer(0), length(0)
  {
    length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0) - 1;
    if (length < 0) length = 0;
    if (length != 0)
    {
      buffer = std::make_shared<wchar_t[]>(length + 1);
      MultiByteToWideChar(CP_UTF8, NULL, str, -1, buffer.get(), length + 1);
    }
  }
  String(const String& str)
    :buffer(0), multiByteBuffer(0), length(0)
  {
    this->operator=(str);
  }
  String(String&& other)noexcept
    :buffer(0), multiByteBuffer(0), length(0)
  {
    this->operator=(static_cast<String&&>(other));
  }
  ~String()
  {
    Free();
  }
  String& operator=(const wchar_t* str)
  {
    Free();
    length = wcslen(str);
    buffer = std::make_shared<wchar_t[]>(length + 1);
    wcscpy_s(buffer.get(), length + 1, str);
    return *this;
  }
  String& operator=(const String& str)
  {
    if (str.buffer == buffer)
      return *this;
    Free();
    if (str.buffer)
    {
      length = str.length;
      buffer = str.buffer;
      multiByteBuffer = 0;
    }
    return *this;
  }
  String& operator=(String&& other)
  {
    if (this != &other)
    {
      Free();
      buffer = std::move(other.buffer);
      length = other.length;
      multiByteBuffer = other.multiByteBuffer;
      other.buffer = 0;
      other.length = 0;
      other.multiByteBuffer = 0;
    }
    return *this;
  }
  wchar_t operator[](int id) const
  {
#if _DEBUG
    if (id < 0 || id >= length)
      throw "Operator[]: index out of range.";
#endif
    return buffer.get()[id];
  }

  wchar_t* getBuffer()
  {
    return buffer.get();
  }
  char* ToMultiByteString(int* len = 0) //转回u8而已
  {
    if (!buffer)
      return (char*)"";
    else
    {
      if (multiByteBuffer) return multiByteBuffer;
      size_t requiredBufferSize;
      requiredBufferSize = WideCharToMultiByte(CP_UTF8, NULL, buffer.get(), length, 0, 0, NULL, NULL) + 1;
      
      if (len)
        *len = requiredBufferSize - 1;  //why -1?
      if (requiredBufferSize)
      {
        multiByteBuffer = new char[requiredBufferSize];
        WideCharToMultiByte(CP_UTF8, NULL, buffer.get(), length, multiByteBuffer, requiredBufferSize, NULL, NULL);
        multiByteBuffer[requiredBufferSize - 1] = 0;
        return multiByteBuffer;
      }
      else
        return (char*)"";
    }
  }
};
int wmain()
{
  std::ifstream f("Text.txt");
  std::string s;
  std::getline(f, s);
  String S(s.c_str());
  auto p = S.getBuffer();
  //auto p = S.ToMultiByteString(); //写回去的时候再这样就行了
  return 0;
}
