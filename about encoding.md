关于编码的问题.md

起源是一个[zhihu问题](https://www.zhihu.com/question/598846108/answer/3012015722):  
```
c++ GBK的字符串存在char*里，怎么转成utf-8存在Wchar_t里？  
这个char*是从int main(int argc, char ** argv)那里接手过来的，应该是GBK吧？  
```
以下是问题的解释和相关概念的梳理  

1.c++ GBK的字符串存在`char*`里  
首先，仅仅在当前的Current system locale is Chinese(Simplified, China)  的时候，从argv接收到的字符串内容的编码才是gbk.  
其次，除了main,还有一个windows特有的wmain函数，其签名为  
```cpp
int wamain(int argc, wchar_t* argv[])
```

此函数除了argv的类型是wchar_t* 外，与main函数无任何区别,虽然SO提到了这一点，msdn也提到了:  
SO: [wmain 和 main 有什么区别？](https://stackoverflow.com/a/2438063/13792395)
msdn: [main and wmain](https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-6.0/aa299386(v=vs.60)?redirectedfrom=MSDN)  
但是它们都仅仅是简单的提了一下，本着“知道自己在说什么”的原则，我用了一下此函数，觉得这样就可以直接把gbk的文字装到类型为wchar_t* 类型的argv里，然后遇到了一些问题,这里记录一下。  

从最简单的说起，大家都知道如果main函数带参数，argc会是传递给它的参数的个数，argv[0]是包含了路径的可执行程序的全名，比如`C:\Users\mingy\source\repos\Project2\x64\Debug\Project2.exe`,而从argv[1]开始才是传递给它的参数，所以写下这段代码:  
```cpp
int main(int argc, char* argv[])
{
  string str = argv[1];
  cout << str << endl;
  return 0;
}
```

在执行此程序时，传给它中文参数 `"衬衫价格"`.  
由于从命令行传递给程序的字符串受到命令行本身的编码格式影响，所以不管argv[1]本身是什么格式，它里面存的东西都是gbk编码格式来储存的，所以输出的时候，由于cmd.exe的编码也是gbk,所以传递给main的字符串就是输出的字符串。  

~~1.这里为了免得每次都得去生成exe的目录下执行，用了插件[CommandlineArguments](https://github.com/MBulli/SmartCommandlineArgs)~~  
~~2.我知道[cmd不是控制台,也不是运行在控制台中](https://devblogs.microsoft.com/commandline/windows-command-line-inside-the-windows-console/#launching-the-console--or-not),这个以后再说。~~  

在我读过的代码里，唯一一次遇到wmain的情况，人家的String是自己写的，所以在此之前我没用过wmain。  
所以当我把代码改成w格式时候，终端不输出任何东西。  
```cpp
int wmain(int argc, wchar_t* argv[])
{
  wstring str = argv[1];
  wcout << str << endl;
  return 0;
}
```

经过到处问人，才知道  
>locale不对的话应该会char也出不来,可以试试在程序里把自己的locale设成空的，不然会有打不开中文路径的文件,str转wstr有中文会失败等等一系列问题。  
具体做法是，在函数内第一行加上`std::setlocale(LC_ALL, "");`  
[文档](https://zh.cppreference.com/w/cpp/locale/setlocale#.E6.B3.A8.E6.84.8F) 里的解释为:  
```bash
程序启动过程中，运行任何用户代码前会执行 std::setlocale(LC_ALL, "C"); 的等价代码。
"C"相当于最小本地环境。 ""则是用户偏好的本地环境。
```
“C”是默认的，应该只支持ascii,  
而 `setlocale(LC_ALL,"");`将会[采用系统环境 ( LANG) 中的默认值](https://stackoverflow.com/a/34036039/13792395)  

设置为空，意味着适配本地locale设置,但是这是不够的，  
因为windows一共有三个locale，界面的locale、系统的locale、每一个硬盘还有自己locale。  
//这些得用api才能读到,翻译成对应的英文后搜索SO有相关内容，这里不试验了。  

然后wmain里的argv[1]也可以存到wstring里然后输出了。  


接下来写non-w版本的，一切正常，想要转换成utf-8的话，有大量的库可用，boost也行,一搜[一大把](),随便选一个  
```cpp

std::string utf8_encode(const std::wstring& wstr)
{
  if (wstr.empty()) return std::string();
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
  return strTo;
}
```

考虑输出到文件,写了如下代码  
```cpp
int wmain(int argc, wchar_t* argv[])
{
  std::setlocale(LC_ALL, "");
  wstring str = argv[1];
  auto str_to_write = utf8_encode(str);
  ofstream output("in.txt", std::ios::ate | std::ios::app);
  output << str_to_write;
  output.close();
  return 0;
}
```
如果用gbk编码的in.txt,自然是乱码的:琛～浠锋牸  
如果用utf8编码的，则一切正常。  
这个很好理解，因为写进去的实际上是二进制串，我们的vscode解读它的格式只要正确，就能得到正确的值。  

但是，如果我想用wofstream来写入wstring呢？也就是像这样:  
```cpp
int wmain(int argc, wchar_t* argv[])
{
  std::setlocale(LC_ALL, "");
  wstring str = argv[1];
  wofstream output("in.txt", std::ios::ate | std::ios::app);
  output<<str;
  output.close();
  return 0;
}

```
我觉得如果in.txt是gbk编码，应该能正确显示，如果是utf-8编码的文本，则应该出现乱码,或者。  
结果无论in.txt以utf8还是gbk,都是不显示任何东西。  
搜到: wofstream supposedly needs to know locale and encoding settings
from:[How to portably write std::wstring to file?](https://stackoverflow.com/questions/4053918/how-to-portably-write-stdwstring-to-file)  
下面的答案我一个没懂...  
之后搜到了这个:  
[Unable to write a std::wstring into wofstream](https://stackoverflow.com/a/5105192/13792395)
添加一行这个就行了。  
```cpp
std::locale::global(std::locale("")); 
```

上面链接的评论区提到了[字符串缩窄的问题](
https://stackoverflow.com/questions/1509277/why-does-wide-file-stream-in-c-narrow-written-data-by-default?rq=1):  
现在我终于看懂群佬那句"wofstream默认会根据字符集做过滤"是什么意思了，比如有如下字符  
```cpp
  wstring someString = L"Hello Stack但是如果有中文?Overflow!";
  wofstream file(L"in.txt");
  file << someString; // the output file will consist of ASCII characters!
```
输出就该不包含中文的部分，但是我得到的只有`但`字前面的部分，不知为啥，暂时挂起...  

好，现在继续主线剧情。  
>char/wchar_t 与 utf8/gbk是什么关系呢？  
互相正交，没关系。  
  
>char和wchar_t能储存的东西有啥区别，比如我有utf-8的文本，存在char[]里和存在wchar_t[]里有什么区别吗？

自己用其实就两个问题  
1. 编译期把你的代码里的字符串用什么编码编译  
2. 你怎么输出   
wchar_t和char唯一的区别就是你在调用Windows API的时候是用本机的编码还是宽字符,所以也影响写屏幕, 
至于什么写文件之类的，对你完全没有影响,你直接按byte把wcahr吐进txt里,他就是utf16,你按byte把utf8编码的char流塞进txt里,他就是utf8,标准库是你程序怎么解析这个字节流的问题.  

Q:所以只要以二进制形式写进去就没那么多事了，对吧?  
A:写文件是这样的  

utf-8一律放char8_t，这才C++20  
放wchat_t会出问题	[补链接，我几个小时以前还见过]()  


vczh, [2023/5/4 17:25]  
应该写个lint，发现你的程序里面用了char，就报错,字节应该用uint8_t,char是什么,标准里甚至都没说char就一定是一个字节（逃,而uint8_t不可能有其他解释了  

Yangff, [2023/5/4 17:32]  
反正Windows这个编码问题除非你在处理老程序,否则一程序律wchar宽字符,IO一律用utf8,设置好locales的话  

其他问题
1.[std::string 有什么缺点](https://www.zhihu.com/question/35967887/answer/124999718)  

>std::string 是「字节串」而非「字符串」,
你的名字简化后为例，"王铭烨 Art" ，utf8_string.length() 是7，[0] 是 '王'，[4] 是 'A'，具体编码每个字符所用的字节数是被隐藏的（就是Python3的解决方案）  
但c++不是这样。  

《Unicode in C++》，从很多不同角度讲了目前的 string、u16string、u32string 在字符这方面的缺陷；  
todo:   
《Unicode in C++》的
[视频](https://www.youtube.com/watch?v=n0GK-9f4dl8)和[ppt](https://github.com/CppCon/CppCon2014/blob/master/Presentations/Unicode%20in%20C%2B%2B/Unicode%20in%20C%2B%2B%20-%20McNellis%20-%20CppCon%202014.pdf)  
>std::string顶多只等同于Qt的QByteArray，接口还不够丰富，如split，而Qt的QString才叫字符串  

>QString全支持,可以独立于 Qt 存在.  

其他群佬提到的链接，可能能用来解决我关于[文本编辑器](http://www.catch22.net/tuts/neatpad)教程里遇到的问题，暂时放着...  
https://faultlore.com/blah/text-hates-you/#additional-links
  
