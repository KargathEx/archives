#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include "Link.h"
#include "Error.h"
#include "Program.h"
#include "LytPtr.h"
#include "Lexer.h"
#include "Tree.h"

using namespace std;
//[why extern?
//[extern - 静态或线程存储期和外部连接，和static只差一点.
//[看起来取消了也没啥区别，继续前进...

extern LytPtr<Program> Parse(Node<Token*>* Current);	//[最上面的函数，用来调用其他的，对吗? 是的。
extern bool IsKeyWord(LytWString Content);				//[检查是否为关键字
extern bool IsDataType(LytWString Content);				//[直接检查字符串内容
extern LytPtr<Program> GetProgram(Node<Token*>*& Current);		//[Parse里调用的第一个函数.
extern LytPtr<Function> GetFunction(Node<Token*>*& Current);	//
extern LytPtr<Var> GetVar(Node<Token*>*& Current);
extern LytPtr<FunctionHead> GetFunctionHead(Node<Token*>*& Current);
extern LytPtr<Statement> GetStatement(Node<Token*>*& Current);
extern LytPtr<Statement> GetAssignStatementOrSimpleStatement(Node<Token*>*& Current);
extern LytPtr<IfStatement> GetIfStatement(Node<Token*>*& Current);
extern LytPtr<WhileStatement> GetWhileDoStatement(Node<Token*>*& Current);
extern LytPtr<WhileStatement> GetDoWhileStatement(Node<Token*>*& Current);
extern LytPtr<Block> GetCompoundStatement(Node<Token*>*& Current);
extern LytPtr<ControlStatement> GetControlStatement(Node<Token*>*& Current);
extern LytPtr<ReturnStatement> GetReturnStatement(Node<Token*>*& Current);
extern LytPtr<Expression> GetExpression(Node<Token*>*& Current);
extern LytPtr<Base> GetBinaryExpression(TreeNode<LytPtr<Base>>* Current);
extern void GetBinaryExpTree(Node<Token*>*& Current, Tree<LytPtr<Base>>& BinaryTree);
extern LytPtr<Expression> GetSimpleExpression(Node<Token*>*& Current);
extern LytPtr<CallFunctionExpression> GetCallFunctionExpression(Node<Token*>*& Current);

#endif