#include "stdafx.h"
#include <stdio.h>
#include<time.h>
#include<graphics.h>
#include <signal.h>
//#include "KinectMain.h"

//全局变量
#define leftedge  70         //游戏区距左边框距离
#define topedge   140        //游戏区距上边框距离
#define iconNum	  24          //游戏图片数目

#define iconX	  67          //游戏图片的长
#define iconY	  70          //游戏图片的宽
#define iconXNum     6           //横排游戏图片个数
#define iconYNum     4           //游戏图片行数

#define promtime    2            //提示次数
#define shuffletime    1            //提示次数

#define countdwontime    360     //倒计时


extern int imageX;			  //游戏图片的长
extern int imageY;            //游戏图片的宽

extern MOUSEMSG mouse;

extern int gameGrid[12][14];  //游戏图纸

extern int timeCount;

typedef struct
{
	int no;
	char name[36];
	IMAGE file;
}image;
extern image iconInfo[iconNum];
extern IMAGE bkImage;         //填充图片


extern int imageX;
extern int imageY;

extern boolean isImageLoad;

extern int yLine;
extern int xLine;

void drawMain();
