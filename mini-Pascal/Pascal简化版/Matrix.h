#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>

using namespace std;

template<typename _Type>
class Matrix
{
public:
	int Row; // 行
	int Column; // 列
	_Type** Data;

	Matrix()
	{
		Row=0;
		Column=0;
	}

	Matrix(const int TempRow, const int TempColumn)
	{
		Row=TempRow;
		Column=TempColumn;
		New();
	}

	Matrix(Matrix<_Type>& Object)
	{
		Row=Object.Row;
		Column=Object.Column;
		New();

		for (int i=0; i<=Row-1; ++i)
			for (int j=0; j<=Column-1; ++j)
				Data[i][j]=Object.Data[i][j];
	}

	~Matrix()
	{
		Release();
	}

	void New()
	{
		for (int i=0; i<=Row-1; ++i)
			Data=new _Type*[Row];
		for (int i=0; i<=Row-1; ++i)
			Data[i]=new _Type[Column];
	}

	void New(const int TempRow, const int TempColumn)
	{
		Row=TempRow;
		Column=TempColumn;
		New();
	}

	void Release() // 内存管理: 释放矩阵数据
	{
		for (int i=0; i<=Row-1; ++i)
			delete[] Data[i];
		Data=0;
		Row=0;
		Column=0;
	}

	void Initialize(const _Type& Value) // 设置矩阵每个点的数据均为Value
	{
		for (int i=0; i<=Row-1; ++i)
			for (int j=0; j<=Column-1; ++j)
				Data[i][j]=Value;
	}

	void Copy(Matrix<_Type>& Object) // 复制矩阵
	{
		Release();
		Row=Object.Row;
		Column=Object.Column;
		New();

		for (int i=0; i<=Row-1; ++i)
			for (int j=0; j<=Column-1; ++j)
				Data[i][j]=Object.Data[i][j];
	}

	_Type& operator()(int x, int y) // 返回点 (x,y) 的值
	{
		return Data[x][y];
	}

	Matrix<_Type>& operator=(Matrix<_Type>& Object)
	{
		Copy(Object);
		return *this;
	}
};

#endif