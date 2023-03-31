#pragma once

#define WIN32_LEAN_AND_MEAN

//#pragma comment(lib, ".\\lib\\bluetooth1.lib" )

#include <Kinect.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <Python.h>
#include "KinectJointFilter.h"
//#include "bluetooth1.h"
//#include "Menu.h"

#define ROBOTARM 1


template<class Interface>
inline void SafeRelease(Interface*& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}



class CBodyBasics
{
		//kinect 2.0 的深度空间的高*宽是 424 * 512，在官网上有说明
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;

public:
	CBodyBasics();
	~CBodyBasics();
	void KinectStart(int option); //打开kinect，最顶层的函数
	int Update(int option);//获得骨架、背景二值图和深度信息
	void InitializeDefaultSensor();//用于初始化kinect

	//通过获得到的信息，把骨架和背景二值图画出来
	int ProcessBody(int nBodyCount, IBody** ppBodies, int option);

	//舞蹈模式
	void ProcessDance(Joint joints[]);

	//传感器模式
	void ProcessSensor(Joint joints[]);

	//判断音频来源相对kinect2的方向，范围为-50~+50（左右）
	float ProcessAudio();

	//根据指定动作控制状态位
	int statusControl(Joint Vertex, Joint Vertex1, Joint Vertex2);

	static void* KinectThread(void* para);
	void DetectionControl();
	void RunKinect();

	void TempHandle();
	unsigned char Dist[108];

	static DWORD  framenumber;			//骨骼帧编号
	float spinemid_xin;		//重心
	float spinemid_yin;
	float spinemid_zin;
	float spinemid_xout;
	float spinemid_yout;
	float spinemid_zout;
	float spinemid_x;
	float spinemid_y;
	float spinemid_z;
	float spinebase_yin, spinebase_yout, spinebase_y;
	float rightfoot_yin = 0, rightfoot_yout = 0, rightfoot_y = 0;
	float rightAnkle_yin = 0, rightAnkle_yout = 0, rightAnkle_y = 0;
	float leftfoot_yin = 0, leftfoot_yout = 0, leftfoot_y = 0;
	float base_foot_in = 0, base_foot_out = 0, base_foot = 0;
	float spinetemp = 0;

	const double thresh_x = 0.15;
	const double thresh_z = 0.15;
	const double handdistance = 0.10;
	unsigned char Msg[10];
	char Msgsend[10];


	int flag = 0;		//下蹲标志位
	int handstate = 0;

	int armstate = 1; //1 for start

	char detectionstate = 0;

	bool Is_SendMsg = true;
	bool Is_SendPort = false;
	int close = 0;
	int Humiclose = 0;
	char TempData[4];

	static CBodyBasics* CurBodyBasics;
	//pthread_t threads_kinect;
	int rc_kinect;
	//pthread_t threads_humi;
	int rc_humi;
private:
	CBodyBasics* BodyBasicPtr;
	IKinectSensor* m_pKinectSensor;//kinect源
	ICoordinateMapper* m_pCoordinateMapper;//用于坐标变换
	IBodyFrameReader* m_pBodyFrameReader;//用于骨架数据读取
	IDepthFrameReader* m_pDepthFrameReader;//用于深度数据读取
	IBodyIndexFrameReader* m_pBodyIndexFrameReader;//用于背景二值图读取


	void Detection(Joint joints[]);
	double  Distance(Joint p1, Joint p2);

	void AngleHandle(Joint joints[]);
	int ElbowCalc(Joint Vertex, Joint Vertex1, Joint Vertex2);
	int HandCalc(Joint Vertex1, Joint Vertex2, Joint Vertex3);
	int ChestCalc(Joint Vertex1, Joint Vertex2, Joint Vertex3);
	void ShoulderCalc(Joint Vertex1, Joint Vertex2, int* Address);
};