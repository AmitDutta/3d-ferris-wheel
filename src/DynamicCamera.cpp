//
// DynamicCamera.cpp
//
//
//

#include "DynamicCamera.h"
#include "stdio.h"


// vars

// functions

DynamicCamera::DynamicCamera()
{
	DynamicCamera(Vector(0,0,0));
}

DynamicCamera::DynamicCamera(Vector &vecSrc)
{
	camSrc = vecSrc;
	camAng.Set(0, 0, 0);
	viewMat.Identity();
	camFov = 60.f;
	camRoll = 0;
	angSpeed = 0.008f;
	moveSpeed = 2.0f;

	//glMatrixMode (GL_PROJECTION);
	//glLoadIdentity ();
	//gluPerspective(camFov, (GLdouble)screenWidth/(GLdouble)screenHeight, 1.0, 1500000.0);
	
	//glMatrixMode (GL_MODELVIEW);
}

DynamicCamera::~DynamicCamera()
{
}

void DynamicCamera::Update()
{
	float newAngSpeed = angSpeed * (g_frameTime);
	//newAngSpeed = newAngSpeed * 2.0 ;
	if (GetAsyncKeyState(VK_RIGHT))
		camAng.x -= newAngSpeed;
	if (GetAsyncKeyState(VK_LEFT))
		camAng.x += newAngSpeed;

	if (GetAsyncKeyState(VK_UP))
		camAng.y -= newAngSpeed;
	if (GetAsyncKeyState(VK_DOWN))
		camAng.y += newAngSpeed;

	if (GetAsyncKeyState(VK_PRIOR))
		camAng.z -= newAngSpeed;
	if (GetAsyncKeyState(VK_HOME))
		camAng.z += newAngSpeed;

	Matrix	angRotMat(camAng.x, camAng.y, camAng.z);

	viewMat = angRotMat*viewMat;

	camAng *= expf(-2.0f*g_frameTime);

	camRoll += camAng.z*70;

	Vector	sideVec(viewMat.p[0], viewMat.p[1], viewMat.p[2]);
	Vector	upVec(viewMat.p[3], viewMat.p[4], viewMat.p[5]);
	Vector	dirVec(viewMat.p[6], viewMat.p[7], viewMat.p[8]);

	// forward-backward
	if (GetAsyncKeyState('A'))
		camVel += dirVec*moveSpeed;
	if (GetAsyncKeyState('Z'))
		camVel -= dirVec*moveSpeed;

	// up-down
	if (GetAsyncKeyState(VK_SUBTRACT))
		camVel += upVec*moveSpeed;
	if (GetAsyncKeyState(VK_ADD))
		camVel -= upVec*moveSpeed;

	// left-right
	if (GetAsyncKeyState(VK_NEXT))
		camVel += sideVec*moveSpeed;
	if (GetAsyncKeyState(VK_END))
		camVel -= sideVec*moveSpeed;

	camSrc += camVel * g_frameTime;
	camVel *= expf(-2.0f*g_frameTime);
	// setup opengl camera


	glLoadIdentity ();
	
	/*printf("CamSrc x=%f  y=%f z=%f\n",camSrc.x,camSrc.y,camSrc.z);
	for(int k=0;k<9;k++)
	{
		printf("ViewMat %d=%f ",k,viewMat.p[k]);
	}*/

	float m[16] = {
		viewMat.p[0],viewMat.p[3],-viewMat.p[6],0,
		viewMat.p[1],viewMat.p[4],-viewMat.p[7],0,
		viewMat.p[2],viewMat.p[5],-viewMat.p[8],0,
		-(viewMat.p[0]*camSrc.x + viewMat.p[1]*camSrc.y + viewMat.p[2]*camSrc.z),
		-(viewMat.p[3]*camSrc.x + viewMat.p[4]*camSrc.y + viewMat.p[5]*camSrc.z),
		(viewMat.p[6]*camSrc.x + viewMat.p[7]*camSrc.y + viewMat.p[8]*camSrc.z),1
	};

	glLoadMatrixf(m);
}


void DynamicCamera::LoadMat(double param0,double param1,double param2,double param3,double param4,double param5,double param6,double param7,double param8)
{

	this->viewMat.p[0] =param0;//-0.944829;
	this->viewMat.p[1] =param1;// 0.000000;
	this->viewMat.p[2] =param2;//-0.327563;
	this->viewMat.p[3] =param3;// 0.000000;
	this->viewMat.p[4] =param4;// 1.000000;
	this->viewMat.p[5] =param5;// 0.000000;
	this->viewMat.p[6] =param6;// 0.327563;
	this->viewMat.p[7] =param7;// 0.000000;
	this->viewMat.p[8] =param8;//-0.944829;

}

void DynamicCamera::LoadCamSrc(double x,double y,double z)
{
	this->camSrc.x = x;//45.0 * cos((-angle+45.0)*PIdiv180);
	this->camSrc.y = y;//75 - 45.0 * sin((-angle+45.0)*PIdiv180);//75+
	this->camSrc.z = z;
}