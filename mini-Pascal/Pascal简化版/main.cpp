#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <locale.h>
#include "LytWString.h"
#include "File.h"
#include "Interpret.h"
//#include<filesystem>
//#include<boost/dll/runtime_symbol_info.hpp>
using namespace std;

int main(int argc , char* argv[])
 {
	setlocale(LC_ALL,"chs");
	LytString AppPath=argv[0];
	for(int i=AppPath.Size()-1;i>=0;i--)
	{
		if(AppPath[i]==L'\\')
		{
			AppPath=AppPath.left(i+1);	
			//返回从左边数i+1个字符,哦，我知道了，这么做可以留下一个"\"!别瞎想了，根本没有的事.只一次就break了哪来的"对每个都删掉一个'\', 那到底为啥要+1呢？ 以及，如果默认的argv[0]就是单反引号的格式，为啥还要=='\\'? 因为在内存里就是以'\\'的格式存在的！ 原因? 我个大傻逼，这个叫转义字符。
			break;
		}
	}	//为了获得当前文件执行的目录，噗这么麻烦的吗？不是有现成的api可用? GetCurrentDirectoryA  冲突了.. 这个法子挺好的 =_= 看看其他地方对于windows.h的以来，然后重写.

	//cout << AppPath << endl;	//可执行文件的位置
	//cout << std::filesystem::current_path();	//源文件的位置？就是如此，并且没法提供统一的,这不是我认知不足导致的，而是事实如此。
	// 如何获得可执行文件的位置呢，如果用这个的话?
	//auto dir = weakly_canonical(filesystem::path(argv[0])).parent_path();	
	//可达到同样的效果.
	// https://stackoverflow.com/a/55579815/13792395
	// https://stackoverflow.com/a/55983201/13792395

	LytWString Content=ReadFile(AppPath+"File.txt").ToWString();
	try
	{
		Interpret TheInterpret(Content.ToLower());
		TheInterpret.Print();
	}
	catch (LytPtr<Error> Info)
	{
		wcout<<Info->GetMessage()<<L"【"<<Info->Content.Content<<L"】("<<Info->Content.Position<<L")"<<endl;
	}
	_getch();
}