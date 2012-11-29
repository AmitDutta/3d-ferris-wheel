//
// DynamicCamera.h
//
// DynamicCamera header file
//

#ifndef __DYNAMICCAMERA_H__
#define __DYNAMICCAMERA_H__

// includes
#include "base.h"
#include "vector.h"
//#include "matrix.h"

// defs

// structs

// vars

// funcs/classes
class DynamicCamera
{
public:
	DynamicCamera(Vector &vecSrc);
	DynamicCamera();
	~DynamicCamera();

	void Update();
	void LoadMat(double param0,double param1,double param2,double param3,double param4,double param5,double param6,double param7,double param8);
	void LoadCamSrc(double x,double y,double z);
	float screenWidth,screenHeight, g_frameTime;

//private:
	Vector	camSrc, camVel, camAng;
	float	angSpeed, moveSpeed;
	float	camFov, camRoll;
	Matrix	viewMat;
};


#endif //__DYNAMICCAMERA_H__