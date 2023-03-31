#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include<stdlib.h>
#include<graphics.h>
#include<io.h>
#include<time.h>
#include<windows.h>
#include <process.h>
#include<mmsystem.h>
#include "Menu.h"
#pragma comment(lib, "winmm.lib")

void drawMain();        //绘制主界面 
void drawMenu();      	//绘制菜单
void drawLine();        //绘制menu下面的分割线
void drawGameMenu();    //绘制游戏菜单界面 
void produceGameArray();  //产生游戏区数组 
void drawGameArea();      //绘制游戏区域 

void drawSettingDiag();   //绘制游戏设置界面 
void drawRankDiag();       //绘制排行榜界面 
void drawHelpDiag();       //绘制帮助界面 

void mainMouseHit();       //主界面鼠标点击 

void settingMouseHit();    //游戏设置鼠标点击 
void rankMouseHit();       //排行榜界面鼠标点击 
void helpMouseHit();       //游戏帮助界面鼠标点击 


void gamePlaying();   //玩游戏 
void gameOver();        //游戏结束 

void drawMain()//画欢迎菜单
{

	// 用背景色清空屏幕
	cleardevice();
	loadimage(NULL, _T(".\\pic\\background.jpeg"), 800, 600, true);

	setbkmode(TRANSPARENT);

	IMAGE tmpImage;
	loadimage(&tmpImage, _T(".\\pic\\welcome.jpg"), 540, 540, true);
	putimage(210, 50, &tmpImage);



	/*
	//游戏不在进行时，可以点击开始游戏按钮
	IMAGE tmpImage1;
	loadimage(&tmpImage1, _T("pic\\button\\startgame0.jpg"), 100, 40, true);
	putimage(50, 50, &tmpImage1);
	*/

}