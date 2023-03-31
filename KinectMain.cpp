#include "KinectMain.h"
#include "Python.h"

using namespace std;
//using   namespace   cv;
using   namespace   Sample;

#define PI 3.141592653

CBodyBasics* CBodyBasics::CurBodyBasics = nullptr;

//构造函数
CBodyBasics::CBodyBasics() :
	m_pKinectSensor(NULL),
	m_pCoordinateMapper(NULL),
	m_pBodyFrameReader(NULL),
	m_pDepthFrameReader(NULL),
	m_pBodyIndexFrameReader(NULL)
{
	//pDlg = new CMFC_DEMO01Dlg();
}

// 析构函数
CBodyBasics::~CBodyBasics()
{
	SafeRelease(m_pBodyFrameReader);
	SafeRelease(m_pCoordinateMapper);
	SafeRelease(m_pDepthFrameReader);
	SafeRelease(m_pBodyIndexFrameReader);

	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}


//第一层，打开Kinect
void CBodyBasics::KinectStart(int option)
{
	
	
	Py_Initialize();
	PyRun_SimpleString("print('Please say start!')\n");

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyRun_SimpleString("import speechrec");
	PyRun_SimpleString("speechrec.speech_rec()");
	Py_Finalize();

	

	ofstream outfile(".\\angles.txt");
	outfile << "*";


	int jiaoyan = 0;
	for (int i = 0; i < 10; i++) {
		Msg[i] = 255;
	}
	for (int i = 1; i < 8; i++) {
		jiaoyan += (int)Msg[i];
		cout << (int)Msg[i] << endl;
		outfile << "#" << (int)Msg[i];
	}

	//状态位status
	outfile << "#" << 3;

	//奇偶校验位
	jiaoyan += 3;
	jiaoyan = jiaoyan % 2;
	outfile << "#" << jiaoyan;

	outfile.close();



	Py_Initialize();

	PyRun_SimpleString("print('Program is starting...')\n");

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyRun_SimpleString("import main");
	PyRun_SimpleString("main.serial_comm()");

	Py_Finalize();


	CBodyBasics* BodyBasic = new CBodyBasics();
	cout << "Kinect start!" << endl;
	if (option == ROBOTARM) {
		//进入第二层
		BodyBasic->RunKinect();
	}  
}


/*第二层，运行Kinect*/
void CBodyBasics::RunKinect() {
	this->close = 0;
	CBodyBasics* BodyBasics = new CBodyBasics();
	cout << "Kinect is running!" << endl;

	/*初始化Msgsend*/
	if (BodyBasics->Is_SendMsg) {
		//BodyBasics->SendMsg.Initialize();
		for (int i = 0; i < 10; i++) {
			BodyBasics->Msgsend[i] = 127;
		}
		BodyBasics->Msgsend[0] = 10;
		//BodyBasics->SendMsg.Send_msg(BodyBasics->Msgsend);
	}
	/*初始化sensor*/
	BodyBasics->InitializeDefaultSensor();

	cout << "Kienct sensor has been initialized!" << endl;
	while (1) {
		//BodyBasics->DetectionControl();
		int status = 2;

		//判断进入跳舞模式还是模仿模式
		cout << "请选择模式：" << endl;
		cout << "1. 康复治疗模式  2.跳舞模式  3.传感器模式" << endl;

		int option = 1;
		cin >> option;
		
		/*
		while (1) {
			BodyBasics->Update(option);
			Sleep(1000);
		}
		*/
		

		
		while (1) {
			int isBreak = BodyBasics->Update(option);
			//cout << "Updating data..." << endl; 
			if (isBreak) {
				break;
			}
			Sleep(8000);
		}
		
		
		//_sleep(1);
		Sleep(1000);

		if (BodyBasics->close)
		{
			break;
		}
	}
}

/*初始化sensor*/
void CBodyBasics::InitializeDefaultSensor()
{
	//搜索kinect
	GetDefaultKinectSensor(&m_pKinectSensor);

	//找到kinect设备
	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;//读取骨架
		IDepthFrameSource* pDepthFrameSource = NULL;//读取深度信息
		IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//读取背景二值图

		//打开kinect
		m_pKinectSensor->Open();
		cout << "kinect is open!" << endl;

		//ProcessAudio();


		//coordinatemapper
		m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);

		//bodyframe    每种图都是通过source，reader，frame三个类
		m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);

		pBodyFrameSource->OpenReader(&m_pBodyFrameReader);

		//depth frame
		m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		pDepthFrameSource->OpenReader(&m_pDepthFrameReader);


		//body index frame
		m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);

		pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);

		SafeRelease(pBodyFrameSource);
		SafeRelease(pDepthFrameSource);
		SafeRelease(pBodyIndexFrameSource);
	}
	/*
	//skeletonImg,用于画骨架、背景二值图的MAT
	skeletonImg.create(cDepthHeight, cDepthWidth, CV_8UC3);
	skeletonImg.setTo(0);

	//depthImg,用于画深度信息的MAT
	depthImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthImg.setTo(0);
	*/
}


/*第三层，实时更新和处理数据*/
/// Main processing function
int CBodyBasics::Update(int option)
{

	//如果丢失了kinect，则不继续操作
	if (!m_pBodyFrameReader) {
		return 1;
	}

	IBodyFrame* pBodyFrame = NULL;//骨架信息
	IDepthFrame* pDepthFrame = NULL;//深度信息
	IBodyIndexFrame* pBodyIndexFrame = NULL;//背景二值图

	//记录每次操作的成功与否
	HRESULT hr = S_OK;

	//-----------------------------获取骨架并显示----------------------------
	if (SUCCEEDED(hr))
	{
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);//获取骨架信息
	}
	if (SUCCEEDED(hr))
	{
		cout << "ok" << endl;
		IBody* ppBodies[BODY_COUNT] = { 0 };//每一个IBody可以追踪一个人，总共可以追踪六个人

		if (SUCCEEDED(hr))
		{
			//把kinect追踪到的人的信息，分别存到每一个IBody中
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr) )
		{
			//对每一个IBody，我们找到他的骨架信息，并且画出来
			cout << "processing body, option = " << option << endl;
			//ProcessBody(BODY_COUNT, ppBodies, option);
			//判断是否要跳出循环
			int isBreak = ProcessBody(BODY_COUNT, ppBodies, option);
			for (int i = 0; i < _countof(ppBodies); ++i)
			{
				SafeRelease(ppBodies[i]);//释放所有
			}
			SafeRelease(pBodyFrame);//必须要释放，否则之后无法获得新的frame数据
			return isBreak;
		}
	}
	//SafeRelease(pBodyFrame);//必须要释放，否则之后无法获得新的frame数据
	return 1;
}

/*第四层，处理骨骼信息*/
int CBodyBasics::ProcessBody(int nBodyCount, IBody** ppBodies, int option)
{
	//记录操作结果是否成功
	HRESULT hr;
	FilterDoubleExponential filterKinect;	//霍尔特双指数滤波器

	//对于每一个IBody
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody) {
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);
			filterKinect.Update(pBody);		//平滑和稳定身体骨架

			if (SUCCEEDED(hr) && bTracked)
			{
				Joint joints[JointType_Count];//存储关节点类
				HandState leftHandState = HandState_Unknown;//左手状态
				HandState rightHandState = HandState_Unknown;//右手状态

				//获取左右手状态
				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);


				//如果手掌状态变化，则输出当前手掌状态
				if (rightHandState == HandState_Open ) {
					handstate = 2;
					//cout << "手掌张开" << endl;
				}
				else if (rightHandState == HandState_Closed) {
					handstate = 3;
					//cout << "手掌闭合" << endl;
				}
				else if (rightHandState == HandState_NotTracked) {
					handstate = 1;
				}

				//存储深度坐标系中的关节点位置
				DepthSpacePoint* depthSpacePosition = new DepthSpacePoint[_countof(joints)];

				//获得关节点类
				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
					for (int j = 0; j < _countof(joints); ++j)
					{
						//将关节点坐标从摄像机坐标系（-1~1）转到深度坐标系（424*512）
						m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthSpacePosition[j]);
					}
					filterKinect.Update(joints);		//平滑身体每个骨骼点

					
					if (option == 1) {
						//动作检测函数
						if (joints[JointType_SpineBase].Position.Z > 0.5 && joints[JointType_SpineBase].Position.Z < 3.5) {
							//Detection(joints);

							cout << "正在获取关节信息" << endl;
							AngleHandle(joints);
							cout << "\n\n\n\n angle handle is completed!" << endl;
							cout << "\n\n hand state: " <<leftHandState << " " << rightHandState << endl;

							if (leftHandState == HandState_Open && rightHandState == HandState_Open) {
								return 1;
							}
							else {
								return 0;
							}
							
						}

						else
							cout << "为了检测的准确度，请尽量站在离Kinect 0.5--3.5 米之间，谢谢！" << endl;
					}
					else if (option == 2) {
						cout << "\n\n\n\n angle handle is completed!" << endl;
						cout << "\n\n hand state: " << leftHandState << " " << rightHandState << endl;

						ProcessDance(joints);
						
						if (leftHandState == HandState_Open && rightHandState == HandState_Open) {
							return 1;
						}
						else {
							return 0;
						}
						
					}
					else if (option == 3) {
						cout << "\n\n\n hand state: " << leftHandState << " " << rightHandState << endl;

						ProcessSensor(joints);

						if (leftHandState == HandState_Open && rightHandState == HandState_Open) {
							return 1;
						}
						else {
							return 0;
						}
					}
					
				}
				delete[] depthSpacePosition;
			}
		}
	}
	return 0;
}

/*第五层，传输指定格式的所有角度数据*/
void CBodyBasics::AngleHandle(Joint joints[]) {
	int AngleElbow, AngleHand, AngleChest;
	int AngleSholder[2];
	if (armstate == 1) {
		//先输出一下当前声音的角度和置信度
		float fAngle = ProcessAudio();
		/*初始化Msg*/
		for (int i = 0; i < 10; i++) {
			Msg[i] = 255;
		}

		int status = 2;

			if (joints[JointType_ElbowRight].Position.X - joints[JointType_ShoulderRight].Position.X < 0.05) {
				cout << "请将手臂放于身体外部" << endl;
				return;
			}


			if (handstate == 2 || handstate == 3) {

				AngleElbow = ElbowCalc(joints[JointType_ElbowRight], joints[JointType_ShoulderRight], joints[JointType_WristRight]);
				AngleHand = HandCalc(joints[JointType_WristRight], joints[JointType_ThumbRight], joints[JointType_HandTipRight]);
				ShoulderCalc(joints[JointType_ElbowRight], joints[JointType_ShoulderRight], AngleSholder);
				AngleChest = ChestCalc(joints[JointType_HipRight], joints[JointType_HipLeft], joints[JointType_SpineShoulder]);



				if (handstate == 2) {

					Msg[0] = 1;
					Msg[1] = fAngle;
					Msg[2] = AngleSholder[0];
					Msg[3] = AngleSholder[1];
					Msg[4] = AngleElbow;
					Msg[5] = AngleElbow;
					Msg[6] = AngleHand;
					Msg[7] = 255;
				}
				else if (handstate == 3) {
					Msg[0] = 2;
					Msg[1] = fAngle;
					Msg[2] = AngleSholder[0];
					Msg[3] = AngleSholder[1];
					Msg[4] = AngleElbow;
					Msg[5] = AngleElbow;
					Msg[6] = AngleHand;
					Msg[7] = 1;
				}
			}
		

		//把一个状态+七个角度存入txt
		ofstream outfile(".\\angles.txt");
		outfile << "*";

		int jiaoyan = 0;
		for (int i = 1; i < 8; i++) {
			Msgsend[i] = (char)Msg[i];
			jiaoyan += (int)Msg[i];
			cout << (int)Msg[i] << endl;
			outfile << "#" << (int)Msg[i] ;
		}
		
		//状态位status
		outfile << "#" << status;

		//奇偶校验位
		jiaoyan += status;
		jiaoyan = jiaoyan % 2;
		outfile << "#" << jiaoyan;
 
		outfile.close();
		

		Py_Initialize();
		PyRun_SimpleString("print('Hello!')\n");

		PyRun_SimpleString("import sys");
		PyRun_SimpleString("sys.path.append('./')");
		PyRun_SimpleString("import main");
		PyRun_SimpleString("main.serial_comm()");

		Py_Finalize();
		
		Sleep(2000);
		

		//system(".\\bluetooth.exe");
		
		/*
		if (Is_SendMsg && mapstate == 0) {
			SendMsg.Send_msg(Msgsend);
			inttohex(Msg, 10);
		}
		*/
	}
}




float CBodyBasics::ProcessAudio() {
	// 音频数据获取  
	IAudioSource* audios = nullptr;
	HRESULT hr = m_pKinectSensor->get_AudioSource(&audios);
	IAudioBeam* audiobm = nullptr;
	IAudioBeamList* audiobml = nullptr;
	audios->get_AudioBeams(&audiobml);
	audiobml->OpenAudioBeam(0, &audiobm); // 目前只支持第一个  

	float fAngle = 0.0f;
	float fAngleConfidence = 0.0f;


	//Mat img(200, 400, CV_8UC3, Scalar(255, 255, 255));
	//	circle(img, Point(200, 0), 100, Scalar(0, 0, 0));
		fAngle = 0.0f;
		fAngleConfidence = 0.0f;
		audiobm->get_BeamAngle(&fAngle); // 获取音频的角度
		audiobm->get_BeamAngleConfidence(&fAngleConfidence); // 获取音频的可信度（0 - 1）  
		fAngle = (fAngle / 3.1415926f)*180.0f;
		fAngle = (fAngle + 50.0f) * 255.0f / 100.0f;
		printf("Angle: %3.2f  Angle Confidence: (%1.2f)\n", fAngle, fAngleConfidence);
		//画方向图(img, fAngle, fAngleConfidence);
		//imshow("", img);
		//Sleep(1000);
		return fAngle;
}

void CBodyBasics::ProcessDance(Joint joints[]) {
	int status = 2;
	status = statusControl(joints[JointType_WristRight], joints[JointType_WristLeft], joints[JointType_Head]);
	if (status == 6 || status == 7) {
		status = 2;
	}

	/*初始化Msg*/
	for (int i = 0; i < 10; i++) {
		Msg[i] = 255;
	}


	//把一个状态+七个角度存入txt
	ofstream outfile(".\\angles.txt");
	outfile << "*";

	int jiaoyan = 0;
	for (int i = 1; i < 8; i++) {
		jiaoyan += (int)Msg[i];
		cout << (int)Msg[i] << endl;
		outfile << "#" << (int)Msg[i];
	}

	//状态位status
	outfile << "#" << status;

	//奇偶校验位
	jiaoyan += status;
	jiaoyan = jiaoyan % 2;
	outfile << "#" << jiaoyan;

	outfile.close();

	cout << "dancing status: " << status << endl;


	Py_Initialize();
	PyRun_SimpleString("print('Hello, the robot is dancing!')\n");

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyRun_SimpleString("import main");
	PyRun_SimpleString("main.serial_comm()");

	Py_Finalize();

	Sleep(2000);


}

void CBodyBasics::ProcessSensor(Joint joints[]) {
	int status = 2;
	status = statusControl(joints[JointType_WristRight], joints[JointType_WristLeft], joints[JointType_Head]);
	if (status == 5) {
		status = 2;
	}

	/*初始化Msg*/
	for (int i = 0; i < 10; i++) {
		Msg[i] = 255;
	}


	//把一个状态+七个角度存入txt
	ofstream outfile(".\\angles.txt");
	outfile << "*";

	int jiaoyan = 0;
	for (int i = 1; i < 8; i++) {
		jiaoyan += (int)Msg[i];
		cout << (int)Msg[i] << endl;
		outfile << "#" << (int)Msg[i];
	}

	//状态位status
	outfile << "#" << status;

	//奇偶校验位
	jiaoyan += status;
	jiaoyan = jiaoyan % 2;
	outfile << "#" << jiaoyan;

	outfile.close();

	cout << "sensor status: " << status << endl;


	Py_Initialize();
	PyRun_SimpleString("print('Hello, the sensors are working!')\n");

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyRun_SimpleString("import main");
	PyRun_SimpleString("main.serial_comm()");

	Py_Finalize();

	Sleep(2000);


}

int CBodyBasics::statusControl(Joint WristRight, Joint WristLeft, Joint Head) {
	int status = 2;
	//双臂展开
	if (Distance(WristLeft, WristRight) > 1) {
		status = 9;
	}
	//左手举过头顶，开始舞蹈
	if (WristLeft.Position.Y > Head.Position.Y && WristRight.Position.Y < Head.Position.Y) {
		status = 5;
	}
	//右手举过头顶，心率
	if (WristRight.Position.Y > Head.Position.Y && WristLeft.Position.Y < Head.Position.Y) {
		status = 6;
	}
	//双手举过头顶，温湿度
	if (WristLeft.Position.Y > Head.Position.Y && WristRight.Position.Y > Head.Position.Y) {
		status = 7;
	}
	return status;
}



//求2个骨骼点之间的距离
double CBodyBasics::Distance(Joint p1, Joint p2)
{
	return sqrt(pow(p2.Position.X - p1.Position.X, 2) + pow(p2.Position.Y - p1.Position.Y, 2) + pow(p2.Position.Z - p1.Position.Z, 2));
}



/*三个关节角度的计算*/
int CBodyBasics::ElbowCalc(Joint ElbowRight, Joint ShoulderRight, Joint WristRight) {
	float vector1[3], vector2[3];
	float dotresult, length1, length2;
	float cosine, angle;
	vector1[0] = ShoulderRight.Position.X - ElbowRight.Position.X;
	vector1[1] = ShoulderRight.Position.Y - ElbowRight.Position.Y;
	vector1[2] = ShoulderRight.Position.Z - ElbowRight.Position.Z;
	vector2[0] = WristRight.Position.X - ElbowRight.Position.X;
	vector2[1] = WristRight.Position.Y - ElbowRight.Position.Y;
	vector2[2] = WristRight.Position.Z - ElbowRight.Position.Z;
	dotresult = vector1[0] * vector2[0] + vector1[1] * vector2[1] + vector1[2] * vector2[2];
	length1 = sqrt(pow(vector1[0], 2) + pow(vector1[1], 2) + pow(vector2[2], 2));
	length2 = sqrt(pow(vector2[0], 2) + pow(vector2[1], 2) + pow(vector2[2], 2));
	cosine = dotresult / (length1 * length2);
	angle = acos(cosine) * 180.0 / PI;
	if (angle < 20) {
		angle = 20;
	}
	if (angle > 160) {
		angle = 160;
	}
	angle = angle / 2.0;
	angle = angle * (254.0 / 70.0) - 35.285714;
	int intangle = (int)angle;
	if (intangle < 1) {
		intangle = 1;
	}
	if (intangle > 254) {
		intangle = 254;
	}
	intangle = -intangle + 256;

	return intangle;
}

int CBodyBasics::HandCalc(Joint Vertex1, Joint Vertex2, Joint Vertex3) {
	float edge1[3], edge2[3];
	edge1[0] = Vertex2.Position.X - Vertex1.Position.X;
	edge1[1] = Vertex2.Position.Y - Vertex1.Position.Y;
	edge1[2] = Vertex2.Position.Z - Vertex1.Position.Z;
	edge2[0] = Vertex3.Position.X - Vertex1.Position.X;
	edge2[1] = Vertex3.Position.Y - Vertex1.Position.Y;
	edge2[2] = Vertex3.Position.Z - Vertex1.Position.Z;
	float NormalVector[3];
	NormalVector[0] = edge1[1] * edge2[2] - edge1[2] * edge2[1];
	NormalVector[1] = edge1[2] * edge2[0] - edge1[0] * edge2[2];
	NormalVector[2] = edge1[0] * edge2[1] - edge1[1] * edge2[0];
	double length = sqrt(pow(NormalVector[0], 2) + pow(NormalVector[1], 2) + pow(NormalVector[2], 2));
	float UnitNormalVector[3];
	UnitNormalVector[0] = NormalVector[0] / length;
	UnitNormalVector[1] = NormalVector[1] / length;
	UnitNormalVector[2] = NormalVector[2] / length;
	float Yaxis[3] = { 0,1,0 };
	float dotresult;
	float angle;
	dotresult = UnitNormalVector[0] * Yaxis[0] + UnitNormalVector[1] * Yaxis[1] + UnitNormalVector[2] * Yaxis[2];
	angle = acos(dotresult) * 180.0 / PI;
	if (angle < 40) {
		angle = 40;
	}
	if (angle > 140) {
		angle = 140;
	}
	angle = angle * (254.0 / 100.0) - 100.6;
	int intangle = int(angle);
	if (intangle < 1) {
		intangle = 1;
	}
	if (intangle > 254) {
		intangle = 254;
	}
	return intangle;
}

int CBodyBasics::ChestCalc(Joint ShoulderRight, Joint ShoulderLeft, Joint SpineMid) {
	float edge1[3], edge2[3];
	edge1[0] = ShoulderLeft.Position.X - ShoulderRight.Position.X;
	edge1[1] = ShoulderLeft.Position.Y - ShoulderRight.Position.Y;
	edge1[2] = ShoulderLeft.Position.Z - ShoulderRight.Position.Z;
	edge2[0] = SpineMid.Position.X - ShoulderRight.Position.X;
	edge2[1] = SpineMid.Position.Y - ShoulderRight.Position.Y;
	edge2[2] = SpineMid.Position.Z - ShoulderRight.Position.Z;
	float NormalVector[3];
	NormalVector[0] = edge1[1] * edge2[2] - edge1[2] * edge2[1];
	NormalVector[1] = edge1[2] * edge2[0] - edge1[0] * edge2[2];
	NormalVector[2] = edge1[0] * edge2[1] - edge1[1] * edge2[0];
	double length = sqrt(pow(NormalVector[0], 2) + pow(NormalVector[1], 2) + pow(NormalVector[2], 2));
	float UnitNormalVector[3];
	UnitNormalVector[0] = NormalVector[0] / length;
	UnitNormalVector[1] = NormalVector[1] / length;
	UnitNormalVector[2] = NormalVector[2] / length;
	float Yaxis[3] = { 1,0,0 };
	float dotresult;
	float angle;
	int intangle;
	dotresult = UnitNormalVector[0] * Yaxis[0] + UnitNormalVector[1] * Yaxis[1] + UnitNormalVector[2] * Yaxis[2];
	angle = acos(dotresult) * 180.0 / PI;
	if (angle > 120) {
		angle = 120;
	}
	if (angle < 60) {
		angle = 60;
	}
	angle = angle * (254.0 / 60.0) - 253.0;
	intangle = (int)angle;
	if (intangle < 1) {
		intangle = 1;
	}
	if (intangle > 254) {
		intangle = 254;
	}
	return intangle;
}

void CBodyBasics::ShoulderCalc(Joint ElbowRight, Joint ShoulderRight, int* AngleShoulder) {
	float UnitVector[3];
	UnitVector[0] = ElbowRight.Position.X - ShoulderRight.Position.X;
	UnitVector[1] = ElbowRight.Position.Y - ShoulderRight.Position.Y;
	UnitVector[2] = ElbowRight.Position.Z - ShoulderRight.Position.Z;
	double length = sqrt(pow(UnitVector[0], 2) + pow(UnitVector[1], 2) + pow(UnitVector[2], 2));
	UnitVector[0] = UnitVector[0] / length;
	UnitVector[1] = UnitVector[1] / length;
	UnitVector[2] = UnitVector[2] / length;
	float SphericalAngle[2];

	SphericalAngle[0] = atan(UnitVector[0] / UnitVector[2]) * 180.0 / PI; //phi
	SphericalAngle[1] = acos(UnitVector[1]) * 180.0 / PI; //theta

	if (SphericalAngle[1] < 20) {
		SphericalAngle[1] = 20;
	}
	if (SphericalAngle[1] > 160) {
		SphericalAngle[1] = 160;
	}
	SphericalAngle[1] = SphericalAngle[1] * (254.0 / 140.0) - 35.285714;

	if (SphericalAngle[0] >= 0) {
		SphericalAngle[0] = SphericalAngle[0];
	}
	else if (SphericalAngle[0] < 0) {
		SphericalAngle[0] = SphericalAngle[0] + 180.0;
	}
	if (SphericalAngle[0] < 70) {
		SphericalAngle[0] = 70;
	}
	if (SphericalAngle[0] > 140) {
		SphericalAngle[0] = 140;
	}
	SphericalAngle[0] = SphericalAngle[0] * (254.0 / 70.0) - 253.0;

	AngleShoulder[0] = (int)SphericalAngle[0];
	AngleShoulder[1] = (int)SphericalAngle[1];



	if (AngleShoulder[0] < 1) {
		AngleShoulder[0] = 1;
	}
	if (AngleShoulder[0] > 254) {
		AngleShoulder[0] = 254;
	}
	if (AngleShoulder[1] < 1) {
		AngleShoulder[1] = 1;
	}
	if (AngleShoulder[1] > 254) {
		AngleShoulder[1] = 254;
	}
}

