//update : 问题别人都帮忙定义好了，自己实现即可，这就是所谓的“自己写 而不是抄代码”，妈的我以前除了在网上找别人现成的实现来读或者修改，根本就不知道“如何知道该写什么“。
//https://leetcode-cn.com/problems/minesweeper/
//这一点本来是该谁来告诉你的？ 呵呵。

//Win32 Console Application
//Complier: MinGW(GCC), VC6.0, VC2003, VC2005
//Demo Game By yzfy(雨中飞燕) [url]http://yzfy.org[/url]
//I'm too lazy to write comments
// header file
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <conio.h>
// defines
#define KEY_UP    0xE048
#define KEY_DOWN  0xE050
#define KEY_LEFT  0xE04B
#define KEY_RIGHT 0xE04D
#define KEY_ESC   0x001B
#define KEY_1     '1'
#define KEY_2     '2'
#define KEY_3     '3'
#define GAME_MAX_WIDTH   100
#define GAME_MAX_HEIGHT  100
// Strings Resource
#define STR_GAMETITLE "ArrowKey:MoveCursor  Key1:Open\
  Key2:Mark  Key3:OpenNeighbors"
#define STR_GAMEWIN   "Congratulations! You Win! Thank you for playing!\n"
#define STR_GAMEOVER  "Game Over, thank you for playing!\n"
#define STR_GAMEEND   "Presented by yzfy . Press ESC to exit\n"
//-------------------------------------------------------------
// Base class
class CConsoleWnd
{
 public:
  static int TextOut(const char*);
  static int GotoXY(int, int);
  static int CharOut(int, int, const int);
  static int TextOut(int, int, const char*);
  static int GetKey();
 public:
};
//{{// class CConsoleWnd
 //
 //  int CConsoleWnd::GetKey()
 //  Wait for standard input and return the KeyCode
 //
 int CConsoleWnd::GetKey()
 {
  int nkey=getch(),nk=0;
  if(nkey>=128||nkey==0)nk=getch();
  return nk>0?nkey*256+nk:nkey;
 }
 //
 //  int CConsoleWnd::GotoXY(int x, int y)
 //  Move cursor to (x,y)
 //  Only Console Application
 //
 int CConsoleWnd::GotoXY(int x, int y)
 {
  COORD cd;
  cd.X = x;cd.Y = y;
  return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),cd);
 }
 //
 //  int CConsoleWnd::TextOut(const char* pstr)
 //  Output a string at current position
 //
 int CConsoleWnd::TextOut(const char* pstr)
 {
  for(;*pstr;++pstr)putchar(*pstr);
  return 0;
 }
 //
 //  int CConsoleWnd::CharOut(int x, int y, const int pstr)
 //  Output a char at (x,y)
 //
 int CConsoleWnd::CharOut(int x, int y, const int pstr)
 {
  GotoXY(x, y);
  return putchar(pstr);
 }
 //
 //  int CConsoleWnd::TextOut(const char* pstr)
 //  Output a string at (x,y)
 //
 int CConsoleWnd::TextOut(int x, int y, const char* pstr)
 {
  GotoXY(x, y);
  return TextOut(pstr);
 }
//}}
//-------------------------------------------------------------
//Application class
class CSLGame:public CConsoleWnd
{
 private:
 private:
  int curX,curY;
  int poolWidth,poolHeight;
  int bm_gamepool[GAME_MAX_HEIGHT+2][GAME_MAX_WIDTH+2];
 public:
  CSLGame():curX(0),curY(0){poolWidth=poolHeight=0;}
  int InitPool(int, int, int);
  int MoveCursor(){return CConsoleWnd::GotoXY(curX, curY);}
  int DrawPool(int);
  int WaitMessage();
  int GetShowNum(int, int);
  int TryOpen(int, int);
 private:
  int DFSShowNum(int, int);
 private:
  const static int GMARK_BOOM;
  const static int GMARK_EMPTY;
  const static int GMARK_MARK;
};
const int CSLGame::GMARK_BOOM =  0x10;
const int CSLGame::GMARK_EMPTY=  0x100;
const int CSLGame::GMARK_MARK =  0x200;
//{{// class CSLGame:public CConsoleWnd
 //
 //  int CSLGame::InitPool(int Width, int Height, int nBoom)
 //  Initialize the game pool.
 //  If Width*Height <= nBoom, or nBoom<=0,
 //  or Width and Height exceed limit , then return 1
 //  otherwise return 0
 //
 int CSLGame::InitPool(int Width, int Height, int nBoom)
 {
  poolWidth = Width; poolHeight = Height;
  if(nBoom<=0 || nBoom>=Width*Height
   || Width <=0 || Width >GAME_MAX_WIDTH
   || Height<=0 || Height>GAME_MAX_HEIGHT
  ){
   return 1;
  }
  // zero memory
  for(int y=0; y<=Height+1; ++y)
  {
   for(int x=0; x<=Width+1; ++x)
   {
    bm_gamepool[y][x]=0;
   }
  }
  // init seed
  srand(time(NULL));
  // init Booms
  while(nBoom)
  {
   int x = rand()%Width + 1, y = rand()%Height + 1;
   if(bm_gamepool[y][x]==0)
   {
    bm_gamepool[y][x] = GMARK_BOOM;
    --nBoom;
   }
  }
  // init cursor position
  curX = curY = 1;
  MoveCursor();
  return 0;
 }
 //
 //  int CSLGame::DrawPool(int bDrawBoom = 0)
 //  Draw game pool to Console window
 //
 int CSLGame::DrawPool(int bDrawBoom = 0)
 {
  for(int y=1;y<=poolHeight;++y)
  {
   CConsoleWnd::GotoXY(1, y);
   for(int x=1;x<=poolWidth;++x)
   {
    if(bm_gamepool[y][x]==0)
    {
     putchar('.');
    }
    else if(bm_gamepool[y][x]==GMARK_EMPTY)
    {
     putchar(' ');
    }
    else if(bm_gamepool[y][x]>0 && bm_gamepool[y][x]<=8)
    {
     putchar('0'+bm_gamepool[y][x]);
    }
    else if(bDrawBoom==0 && (bm_gamepool[y][x] & GMARK_MARK))
    {
     putchar('#');
    }
    else if(bm_gamepool[y][x] & GMARK_BOOM)
    {
     if(bDrawBoom)
      putchar('*');
     else
      putchar('.');
    }
   }
  }
  return 0;
 }
 //
 //  int CSLGame::GetShowNum(int x, int y)
 //  return ShowNum at (x, y)
 //
 int CSLGame::GetShowNum(int x, int y)
 {
  int nCount = 0;
  for(int Y=-1;Y<=1;++Y)
   for(int X=-1;X<=1;++X)
  {
   if(bm_gamepool[y+Y][x+X] & GMARK_BOOM)++nCount;
  }
  return nCount;
 }
 //
 //  int CSLGame::TryOpen(int x, int y)
 //  Try open (x, y) and show the number
 //  If there is a boom, then return EOF
 //
 int CSLGame::TryOpen(int x, int y)
 {
  int nRT = 0;
  if(bm_gamepool[y][x] & GMARK_BOOM)
  {
   nRT = EOF;
  }
  else
  {
   int nCount = GetShowNum(x,y);
   if(nCount==0)
   {
    DFSShowNum(x, y);
   }
   else bm_gamepool[y][x] = nCount;
  }
  return nRT;
 }
 //
 //  int CSLGame::DFSShowNum(int x, int y)
 //  Private function, no comment
 //
 int CSLGame::DFSShowNum(int x, int y)
 {
  if((0<x && x<=poolWidth) &&
   (0<y && y<=poolHeight) &&
   (bm_gamepool[y][x]==0))
  {
   int nCount = GetShowNum(x, y);
   if(nCount==0)
   {
    bm_gamepool[y][x] = GMARK_EMPTY;
    for(int Y=-1;Y<=1;++Y)
     for(int X=-1;X<=1;++X)
    {
     DFSShowNum(x+X,y+Y);
    }
   }
   else bm_gamepool[y][x] = nCount;
  }
  return 0;
 }
 //
 //  int CSLGame::WaitMessage()
 //  Game loop, wait and process an input message
 //  return:  0: not end;  1: Win; otherwise: Lose
 //
 int CSLGame::WaitMessage()
 {
  int nKey = CConsoleWnd::GetKey();
  int nRT = 0, nArrow = 0;
  switch (nKey)
  {
   case KEY_UP:
   {
    if(curY>1)--curY;
    nArrow=1;
   }break;
   case KEY_DOWN:
   {
    if(curY<poolHeight)++curY;
    nArrow=1;
   }break;
   case KEY_LEFT:
   {
    if(curX>1)--curX;
    nArrow=1;
   }break;
   case KEY_RIGHT:
   {
    if(curX<poolWidth)++curX;
    nArrow=1;
   }break;
   case KEY_1:
   {
    nRT = TryOpen(curX, curY);
   }break;
   case KEY_2:
   {
    if((bm_gamepool[curY][curX]
     & ~(GMARK_MARK|GMARK_BOOM))==0)
    {
     bm_gamepool[curY][curX] ^= GMARK_MARK;
    }
   }break;
   case KEY_3:
   {
    if(bm_gamepool[curY][curX] & 0xF)
    {
     int nb = bm_gamepool[curY][curX] & 0xF;
     for(int y=-1;y<=1;++y)
      for(int x=-1;x<=1;++x)
     {
      if(bm_gamepool[curY+y][curX+x] & GMARK_MARK)
       --nb;
     }
     if(nb==0)
     {
      for(int y=-1;y<=1;++y)
       for(int x=-1;x<=1;++x)
      {
       if((bm_gamepool[curY+y][curX+x]
        & (0xF|GMARK_MARK)) == 0)
       {
        nRT |= TryOpen(curX+x, curY+y);
       }
      }
     }
    }
   }break;
   case KEY_ESC:
   {
    nRT = EOF;
   }break;
  }
  if(nKey == KEY_1 || nKey == KEY_3)
  {
   int y=1;
   for(;y<=poolHeight;++y)
   {
    int x=1;
    for(;x<=poolWidth; ++x)
    {
     if(bm_gamepool[y][x]==0)break;
    }
    if(x<=poolWidth) break;
   }
   if(! (y<=poolHeight))
   {
    nRT = 1;
   }
  }
  if(nArrow==0)
  {
   DrawPool();
  }
  MoveCursor();
  return nRT;
 }
//}}
//-------------------------------------------------------------
//{{
 //
 //  main function
 //
 int main(void)
 {
  int x=50, y=20, b=100,n; // define width & height & n_booms
  CSLGame slGame;
  // Init Game
  {
   CConsoleWnd::GotoXY(0,0);
   CConsoleWnd::TextOut(STR_GAMETITLE);
   slGame.InitPool(x,y,b);
   slGame.DrawPool();
   slGame.MoveCursor();
  }
  while((n=slGame.WaitMessage())==0) // Game Message Loop
   ;
  // End of the Game
  {
   slGame.DrawPool(1);
   CConsoleWnd::TextOut("\n");
   if(n==1)
   {
    CConsoleWnd::TextOut(STR_GAMEWIN);
   }
   else
   {
    CConsoleWnd::TextOut(STR_GAMEOVER);
   }
   CConsoleWnd::TextOut(STR_GAMEEND);
  }
  while(CConsoleWnd::GetKey()!=KEY_ESC)
   ;
  return 0;
 }
//}}

//http://blog.sina.com.cn/s/blog_66ad7bba0100hf79.html
//别人的轮子
