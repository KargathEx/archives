#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "LytString.h"

using namespace std;

/*功能：读取txt格式文件
  参数：记录文件名（包含路径），类型为LytString
  返回值：返回文件包含内容，类型为LytString */
extern LytString ReadFile(LytString);

/*功能：写txt格式文件
  参数：记录所写内容，类型为LytString */
extern void WriteFile(LytString , LytString);

#endif