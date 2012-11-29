#include "base.h"
#include "DynamicCamera.h"
#include "GLTexture.h"
#include <stdio.h>
#include <windows.h>

static DynamicCamera	*l_pDynCam;


int scrw,scrh, lightVar;
float spotLightXdirection;
static GLUquadricObj *qobj1, *qobj2;

//CCamera staticCam;
int sys_Time=0;
static LARGE_INTEGER TimeFreq, TimeStart, TimeNow;
static bool animFlag = false;

//fps related vars
static char	titleBar[1024];
int xOffset = 10;
float g_frameTime = 0;

bool anim, inRide, isNight, isRedTextureForChamber;
float angle;
static double timerDelay = 100;

GLTexture steelWireTexture, redTexture, blueTexture, triangleTexture, fieldTexture, newblueTexture, yellowTexture, greenTexture
, whiteTexture, redBase;

/*Light0 properties*/
static GLfloat  ambientLight[] = { 0.30f, 0.3f, 0.3f, 1.0f };  
static GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };  
static GLfloat  lightPos[] = {  0.f, 0.0f, 0.0f, 1.0f }; 

static GLfloat  ambientLight1[] = { 0.2f, 0.2f, 0.2f, 1.0f };  
static GLfloat  diffuseLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };  
static GLfloat  specularLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };  
static GLfloat  lightPos1[] = {  50.0f, 1000.0f, 150.0f, 1.0f }; 
static GLfloat  spotDir[] = { 0.0f, -1.0f, 0.0f}; 

void drawCircle(float radius);

void loadTextures()
{
	steelWireTexture.LoadBMP("textures/rod-2.bmp");
	redTexture.LoadBMP("textures/red.bmp");
	blueTexture.LoadBMP("textures/blue.bmp");
	triangleTexture.LoadBMP("textures/triangle.bmp");	
	fieldTexture.LoadBMP ("textures/filed.bmp");	
	
	newblueTexture.LoadBMP("textures/blue.bmp");
	yellowTexture.LoadBMP("textures/yellow.bmp");
	greenTexture.LoadBMP("textures/green.bmp");
	whiteTexture.LoadBMP("textures/white.bmp");
	redBase.LoadBMP("textures/red-base.bmp");
}

void enableLighting(void)
{
	glClearColor (0.5, 0.5, 0.9, 0.0);	
	// glClearColor (1.0, 1.0, 1.0, 0.0);	
    
	// Enable lighting 
    glEnable(GL_LIGHTING); 
	glEnable(GL_NORMALIZE);

   // Setup and enable light 0  
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);  
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);  
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);  
    glEnable(GL_LIGHT0);

    // Enable color tracking  
    glEnable(GL_COLOR_MATERIAL);  
 
    // Set Material properties to follow glColor values  
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void init()
{
    glEnable( GL_BLEND ); 
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );    
    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0.5f );
	
	glClearColor (0.5, 0.5, 0.9, 0.0);
	// glClearColor (1.0, 1.0, 1.0, 0.0);
	
	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);        
 	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	enableLighting();

	l_pDynCam = new DynamicCamera(Vector(0,200,200));	
	l_pDynCam->LoadMat(-0.944829,0.000000,-0.327563,0.000000,1.000000,0.000000,0.327563,0.000000,-0.944829);
	
	// Enabling texture
	glEnable( GL_TEXTURE_2D );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//set up quadratic object
	qobj1 = gluNewQuadric();
	qobj2 = gluNewQuadric();
	gluQuadricDrawStyle(qobj1,GLU_FILL);  //POINT LINE
	gluQuadricNormals(qobj1,GLU_SMOOTH);
	gluQuadricTexture(qobj1,GL_TRUE);

	gluQuadricDrawStyle(qobj2,GLU_FILL);  //POINT LINE
	gluQuadricNormals(qobj2,GLU_SMOOTH);
	gluQuadricTexture(qobj2,GL_TRUE);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); // Set The Texture Generation Mode For S To Sphere Mapping (NEW)
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); // Set The Texture Generation Mode For T To Sphere Mapping (NEW)

	loadTextures();
	
	inRide = anim = isNight = isRedTextureForChamber = false;
	angle = 0.0;

	spotLightXdirection = 1.0f;
	lightVar = 0;
}

void push()
{
	glPushMatrix();
}

void pop()
{
	glPopMatrix();
}

void drawSolidCube(float val)
{
	val = val / 2.0;	
	glBegin(GL_QUADS);
	
		//front face
		glNormal3f(0,0,1);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-val,-val,val);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(val,-val,val);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(val, val, val);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-val, val, val);

		//left face	
		glNormal3f(-1,0,0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-val,-val,-val);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-val,-val,val);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-val, val,val);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-val, val,-val);

		//rightface
		glNormal3f( 1,0,0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( val,-val,val);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( val,-val,-val);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( val, val,-val);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( val, val, val);

		//backface
		glNormal3f( 0,0,-1);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( val,-val,-val);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-val,-val,-val);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-val, val,-val);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( val, val,-val);

		//bottom face
		glNormal3f( 0,-1,0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-val,-val,-val);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( val,-val,-val);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( val,-val,val);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-val,-val,val);

		//top face
		glNormal3f( 0,1,0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-val,val,val);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( val,val,val);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( val,val,-val);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-val,val,-val);

	glEnd();
}

void reshape(int w, int h)
{
	if (w == 0 || h == 0) return; 
	float ratio = (float) w / (float) h;
	
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();
	gluPerspective(60.0,ratio,0.5,10000.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,(GLsizei)w,(GLsizei)h);  //Use the whole window for rendering
	l_pDynCam->screenHeight = h;
	l_pDynCam->screenWidth = w;
	l_pDynCam->g_frameTime = 100;
	scrw = w;
	scrh = h;
}

void _preDisplay(void)
{
	static float	lastTime = 0.0f;
	float			fTime = sys_Time/1000.f;
	static float	lastSecTime = fTime;
	static dword	frameCounter = 0;

	frameCounter++;	

	l_pDynCam->g_frameTime = fTime-lastTime;
	QueryPerformanceCounter(&TimeStart);
    QueryPerformanceFrequency(&TimeFreq);
}

void _postDisplay(void)
{
	QueryPerformanceCounter(&TimeNow);
    sys_Time = ((TimeNow.QuadPart-TimeStart.QuadPart)*2000/TimeFreq.QuadPart);
}

void FPS()					// This function calculates FPS
{
	static float fps           = 0.0f;						
    static float previousTime  = 0.0f;	
	static char  strFPS[20]    = {0};
	
	float currentTime = (GetTickCount() * 0.001f);				

    ++fps;	// Increment the FPS counter

    if( currentTime - previousTime > 1.0f )
    {
	    previousTime = currentTime;
		sprintf(strFPS, "FPS: %d", int(fps));
		glutSetWindowTitle(strFPS);
        fps = 0.0f;
    }
}

void drawAxis()
{
	push();
		glColor3f(0,0,1);
		glBegin(GL_LINES);
			
			glColor3f(1,0,0);
			glVertex3f(0,0,0);
			glVertex3f(1000,0,0);

			glColor3f(0,1,0);
			glVertex3f(0,0,0);
			glVertex3f(0,1000,0);
		
			glColor3f(0,0,1);
			glVertex3f(0,0,0);
			glVertex3f(0,0,1000);

		glEnd();
	pop();
}

void drawTrangularJoiningPart(float width, float height, int depth)
{
	float val = width/2;
	float zval = depth/2;

	// front triangluar face	
	triangleTexture.Use();
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);		
		glTexCoord2f(0.0f, 1.0f); glVertex3f(val, 0.0, zval);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-val, 0.0, zval);
		glTexCoord2f(0.0f, 0.5f); glVertex3f(0, height, zval);		
	glEnd();
	
	// rare triangluar face	
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(val, 0.0, -zval);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-val, 0.0, -zval);
		glTexCoord2f(0.0f, 0.5f); glVertex3f(0, height, -zval);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glColor3f(1,0,0);

	// left square face	
	glTexCoord2f(0.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);

	glColor3f(0.27,0.58,0.8);
	blueTexture.Use();
	glBegin(GL_POLYGON);				
		glTexCoord2f(0.0f, 0.0f); glVertex3f(val, 0.0, zval);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0, height, zval);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0, height, -zval);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(val, 0.0, -zval);
	glEnd();

	// right square face	
	glBegin(GL_POLYGON);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-val, 0.0, zval);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0, height, zval);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0, height, -zval);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-val, 0.0, -zval);
	glEnd();

	//bottom face
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-val, 0.0, zval);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(val, 0.0, zval);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(val, 0.0, zval);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-val, 0.0, -zval);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void drawFrame()
{
	push();		
		glTranslatef(170.0,0.0,80.0);		
		glColor3f(105.0/255,15.0/255.0,14.0/255.0);
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		glRotatef(30.0, 1.0, 0.0, 0.0);
		glScalef(20,700,20);
		glEnable(GL_TEXTURE_2D);
		redBase.Use();
		drawSolidCube(1);		
	pop();

	push();		
		glTranslatef(-170.0,0.0,80.0);				
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glRotatef(30.0, 1.0, 0.0, 0.0);
		glScalef(20,700,20);
		drawSolidCube(1);		
	pop();

	push();		
		glTranslatef(170.0,0.0,-80.0);			
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		glRotatef(30.0, 1.0, 0.0, 0.0);
		glScalef(20,700,20);
		drawSolidCube(1);		
	pop();

	push();		
		glTranslatef(-170.0,0.0,-80.0);						
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glRotatef(30.0, 1.0, 0.0, 0.0);
		glScalef(20,700,20);
		drawSolidCube(1);
		glDisable(GL_TEXTURE_2D);
	pop();

	// straight horizontal bars
	push();		
		glColor3f(0.27,0.58,0.8);
		glTranslatef(0.0,-20.0,80.0);
		glScalef(380.0,10.0,5.0);
		glEnable(GL_TEXTURE_2D);
		blueTexture.Use();
		drawSolidCube(1);		
	pop();
	
	push();		
		glTranslatef(0.0,-20.0,-80.0);
		glScalef(380.0,10.0,5.0);
		blueTexture.Use();
		drawSolidCube(1);
		glDisable(GL_TEXTURE_2D);
	pop();

	// the cylinders
	push();		
		glEnable(GL_TEXTURE_2D);
		glTranslatef(0.0,300.0,-95);
		glScalef(1.0,1.0,2.5);
		glColor3f(1.0,0.0,0.0);
		redTexture.Use();
		gluCylinder(qobj1,5,5,75,200,2);
		glDisable(GL_TEXTURE_2D);
	pop();

	push();
		glEnable(GL_TEXTURE_2D);
		glTranslatef(0.0,300.0,-58);
		glScalef(1.0,1,1.6);		
		glColor3f(0.8, 0.8, 0.82);
		steelWireTexture.Use();
		gluCylinder(qobj1,10,10,75,200,2);	
		glDisable(GL_TEXTURE_2D);
	pop();

	push();
		glTranslatef(0.0,280.0,80.0);
		drawTrangularJoiningPart(50,40,25);
	pop();

	push();
		glTranslatef(0.0,280.0,-80.0);
		drawTrangularJoiningPart(50,40,25);
	pop();

	// the circles on front of cylinders
	push();
		glEnable(GL_TEXTURE_2D);
		glTranslatef(0.0,300.0,93);
		glColor3f(242.0/255,49.0/255.0,34.0/255.0);
		redTexture.Use();
		drawCircle(5.0);
		glDisable(GL_TEXTURE_2D);
	pop();

	push();
		glEnable(GL_TEXTURE_2D);
		glTranslatef(0.0,300.0,-95);
		glColor3f(242.0/255,49.0/255.0,34.0/255.0);
		redTexture.Use();
		drawCircle(5.0);
		glDisable(GL_TEXTURE_2D);
	pop();
}

void drawCircle(float radius)
{
	glutSolidCone(radius,0,40,1);
}

void drawChamber()
{
	glColor3f(0.0,0.0,1.0);
	glRotatef(-90,1,0,0);		
	push();	
		glEnable(GL_TEXTURE_2D);

		glColor3f(225.0/255.0,225.0/255.0,225.0/255.0);
		whiteTexture.Use();
		gluCylinder(qobj1,5,5,75,200,2);
		glTranslatef(0,0,-5);
		gluCylinder(qobj1,40,40,30,200,2);				

		if (isRedTextureForChamber)
		{
			glColor3f(242.0/255,49.0/255.0,34.0/255.0);
			redTexture.Use();
		}
		else
		{
			glColor3f(235.0/255.0,197.0/255.0,2.0/255.0);		
			yellowTexture.Use();
		}	
		
		drawCircle(40);
		glTranslatef(0,0,55);		
		glutWireCone(40,10,200,2);

		glDisable(GL_TEXTURE_2D);
	pop();
}

void drawStructure()
{	
		int i,j;
		glRotatef(angle, 0, 0, 1);	
		
		float xScale, yScale, zScale;
		float limit = 360 / 2;
		float innerRadius = 375;
		xScale = 5;
		yScale = 750;
		zScale = 2;

		glEnable(GL_TEXTURE_2D);		

		int p = 0;
		for (i = 0; i <= limit; i += 15)
		{
			glEnable(GL_TEXTURE_2D);
			if (p == 0)
			{
				glColor3f(167.0/255.0,48.0/255.0,52.0/255.0);
				redTexture.Use();								
				p++;
			}

			else  if (p == 1)
			{
				glColor3f(98.0/255.0,152.0/255.0,199.0/255.0);
				newblueTexture.Use();
				p++;
			}

			else if (p == 2)
			{
				glColor3f(235.0/255.0,197.0/255.0,2.0/255.0);
				yellowTexture.Use();
				p++;
			}
			else if (p == 3)
			{
				glColor3f(135.0/255.0,146.0/255.0,77.0/255.0);
				greenTexture.Use();
				p = 0;
			}					

			push();
				glRotatef(i, 0, 0, 1);
				glTranslatef(0.0,0.0,28.0);
				glScalef(xScale, yScale, zScale);
				drawSolidCube(1);				
				glTranslatef(0.0,0.0,-60.0);
				drawSolidCube(1);			
			pop();

			glDisable(GL_TEXTURE_2D);
		}

		glColor3f(1.0,0.0,0.0);
		push();
			p = 0;
			int q = 0;
			for (i = 0, j = 15; i < 360; i += 15, j += 15)
			{
				float x1 = sin(i*PI/180) * innerRadius;
				float y1 = cos(i*PI/180) * innerRadius;
				float x2 = sin(i*PI/180) * (innerRadius + 5);
				float y2 = cos(i*PI/180) * (innerRadius + 5);

				// calculate the next set of points				
				float x3 = sin(j*PI/180) * innerRadius;
				float y3 = cos(j*PI/180) * innerRadius;
				float x4 = sin(j*PI/180) * (innerRadius + 5);
				float y4 = cos(j*PI/180) * (innerRadius + 5);

				// front wheel				
				glEnable(GL_TEXTURE_2D);
				if (p == 0)
				{
					glColor3f(167.0/255.0,48.0/255.0,52.0/255.0);
					redTexture.Use();
					p++;
				}
				else if (p == 1)
				{
					glColor3f(98.0/255.0,152.0/255.0,199.0/255.0);
					newblueTexture.Use();
					p++;
				}
				else if (p == 2)
				{
					glColor3f(235.0/255.0,197.0/255.0,2.0/255.0);
					yellowTexture.Use();
					p++;
				}
				else if (p == 3)
				{
					glColor3f(135.0/255.0,146.0/255.0,77.0/255.0);
					greenTexture.Use();
					p = 0;
				}

				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x1,y1,29);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x2,y2,29);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x4,y4,29);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x3,y3,29);
				glEnd();

				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x1,y1,27);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x2,y2,27);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x4,y4,27);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x3,y3,27);
				glEnd();				

				if (q == 0)
				{
					glColor3f(98.0/255.0,152.0/255.0,199.0/255.0);
					newblueTexture.Use();					
					q++;
				}
				else if (q == 1)
				{
					glColor3f(235.0/255.0,197.0/255.0,2.0/255.0);
					yellowTexture.Use();
					q++;
				}
				else if (q == 2)
				{
					glColor3f(135.0/255.0,146.0/255.0,77.0/255.0);
					greenTexture.Use();
					q++;
				}
				else if (q == 3)
				{
					glColor3f(167.0/255.0,48.0/255.0,52.0/255.0);
					redTexture.Use();
					q = 0;
				}				

				// top part of front wheel
				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x1,y1,29);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x3,y3,29);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x3,y3,27);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x1,y1,27);
				glEnd();
				
				//bottom part of front wheel
				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x2,y2,29);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x4,y4,29);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x4,y4,27);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x2,y2,27);
				glEnd();				
				
				if (p == 0)
				{
					glColor3f(167.0/255.0,48.0/255.0,52.0/255.0);
					redTexture.Use();					
				}
				else if (p == 1)
				{
					glColor3f(98.0/255.0,152.0/255.0,199.0/255.0);
					newblueTexture.Use();					
				}
				else if (p == 2)
				{
					glColor3f(235.0/255.0,197.0/255.0,2.0/255.0);
					yellowTexture.Use();					
				}
				else if (p == 3)
				{
					glColor3f(135.0/255.0,146.0/255.0,77.0/255.0);
					greenTexture.Use();					
				}

				// rare wheel
				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x1,y1, -91);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x2,y2, -91);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x4,y4, -91);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x3,y3, -91);
				glEnd();

				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x1,y1, -93);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x2,y2, -93);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x4,y4, -93);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x3,y3, -93);
				glEnd();

				if (q == 0)
				{
					glColor3f(98.0/255.0,152.0/255.0,199.0/255.0);
					newblueTexture.Use();					
				}
				else if (q == 1)
				{
					glColor3f(235.0/255.0,197.0/255.0,2.0/255.0);
					yellowTexture.Use();
				}
				else if (q == 2)
				{
					glColor3f(135.0/255.0,146.0/255.0,77.0/255.0);
					greenTexture.Use();
				}
				else if (q == 3)
				{
					glColor3f(167.0/255.0,48.0/255.0,52.0/255.0);
					redTexture.Use();
				}

				// top part of rare wheel				
				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x1,y1,-91);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x3,y3,-91);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x3,y3,-93);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x1,y1,-93);
				glEnd();
				
				//bottom part of rare wheel
				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x2,y2,-91);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x4,y4,-91);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x4,y4,-93);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x2,y2,-93);
				glEnd();				

				//connection between front and rare part
				glColor3f(0,0,0);
				glBegin(GL_POLYGON);
					glVertex3f(x1, y1, 29);
					glVertex3f(x2, y2, 29);
					glVertex3f(x2, y2, -91);
					glVertex3f(x1, y1, -91);
				glEnd();

				// connection between previous points
				float x5 = sin((i + 1)*PI/180) * (innerRadius + 4.2);
				float y5 = cos((i + 1)*PI/180) * (innerRadius + 4.2);
				float x6 = sin((i - 1)*PI/180) * (innerRadius + 4.2);
				float y6 = cos((i - 1)*PI/180) * (innerRadius + 4.2);

				float x7 = sin((i + 1)*PI/180) * (innerRadius - 0.9);
				float y7 = cos((i + 1)*PI/180) * (innerRadius - 0.9);
				float x8 = sin((i - 1)*PI/180) * (innerRadius - 0.9);
				float y8 = cos((i - 1)*PI/180) * (innerRadius - 0.9);		

				if (p == 0)
				{
					glColor3f(167.0/255.0,48.0/255.0,52.0/255.0);
					redTexture.Use();					
				}
				else if (p == 1)
				{
					glColor3f(98.0/255.0,152.0/255.0,199.0/255.0);
					newblueTexture.Use();					
				}
				else if (p == 2)
				{
					glColor3f(235.0/255.0,197.0/255.0,2.0/255.0);
					yellowTexture.Use();					
				}
				else if (p == 3)
				{
					glColor3f(135.0/255.0,146.0/255.0,77.0/255.0);
					greenTexture.Use();					
				}

				// lower part				
				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x2, y2, 29);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x2, y2, -91);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x5, y5, -91);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x5, y5, 29); 
				glEnd();				
				
				// upper part				
				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y1, 29);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x1, y1, -91);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x7, y7, -91);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x7, y7, 29);
				glEnd();

				if (p == 0)
				{
					glColor3f(235.0/255.0,197.0/255.0,2.0/255.0);
					yellowTexture.Use();									
				}
				else if (p == 1)
				{
					glColor3f(135.0/255.0,146.0/255.0,77.0/255.0);
					greenTexture.Use();	
				}
				else if (p == 2)
				{
					glColor3f(167.0/255.0,48.0/255.0,52.0/255.0);
					redTexture.Use();						
				}
				else if (p == 3)
				{
					glColor3f(98.0/255.0,152.0/255.0,199.0/255.0);
					newblueTexture.Use();
				}

				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x2, y2, 29);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x2, y2, -91);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x6, y6, -91);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x6, y6, 29);
				glEnd();

				glBegin(GL_POLYGON);					
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y1, 29);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x1, y1, -91);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x8, y8, -91);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x8, y8, 29);
				glEnd();

				if (q == 0)
				{
					glColor3f(98.0/255.0,152.0/255.0,199.0/255.0);
					newblueTexture.Use();					
				}
				else if (q == 1)
				{
					glColor3f(235.0/255.0,197.0/255.0,2.0/255.0);
					yellowTexture.Use();
				}
				else if (q == 2)
				{
					glColor3f(135.0/255.0,146.0/255.0,77.0/255.0);
					greenTexture.Use();
				}
				else if (q == 3)
				{
					glColor3f(167.0/255.0,48.0/255.0,52.0/255.0);
					redTexture.Use();
				}

				// connect top and bottom parts				
				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x5, y5, 29);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x5, y5, -91);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x7, y7, -91);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x7, y7, 29);
				glEnd();

				glBegin(GL_POLYGON);
					glTexCoord2f(0.0f, 0.0f); glVertex3f(x6, y6, 29);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(x6, y6, -91);
					glTexCoord2f(1.0f, 1.0f); glVertex3f(x8, y8, -91);
					glTexCoord2f(0.0f, 1.0f); glVertex3f(x8, y8, 29);
				glEnd();
				
				glDisable(GL_TEXTURE_2D);

				float chamberX = (x2 + x5)/2;
				float chamberY = (y2 + y5)/2;
				float chamberZ = (-67-1)/2;
				push();					
					glTranslatef(x1, y1, chamberZ);
					glRotatef(-angle, 0, 0, 1);
					push();
						glTranslatef(0.0,-75.0,0.0);
						isRedTextureForChamber = !isRedTextureForChamber;
						drawChamber();					
					pop();
				pop();
			}

		pop();
}

void drawField()
{
	glEnable(GL_TEXTURE_2D);
	push();		
		glColor3f(0.0,1.0,0.0);		
		fieldTexture.Use();
		glBegin(GL_QUADS);
			glNormal3f( 0,1,0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(1000.0,0.0,1000.0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1000.0,0.0,1000.0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1000.0,0.0,-1000.0);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(1000.0,0.0,-1000.0);
		glEnd();		
	pop();
	glDisable(GL_TEXTURE_2D);
}

void display(void)
{
	_preDisplay();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0); 
	glDisable(GL_LIGHT1);
	glLoadIdentity();

	if (inRide)
	{
		float x = xOffset + 375* cos((angle)*PIdiv180);
		float y = 490 + 375 * sin((angle)*PIdiv180);
		l_pDynCam->camSrc.x = x;
		l_pDynCam->camSrc.y = y;
		l_pDynCam->camSrc.z = 0;
	}

	l_pDynCam->Update();
	glEnable(GL_LIGHTING);
	if(isNight)
	{
		glClearColor (0.0, 0.0, 0.0, 0.0);
		glDisable(GL_LIGHT0);

		//glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);  
		//glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);  
		//glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
		ambientLight1[0] = 0.1f;//red;//
		ambientLight1[1] = 0.1f;//green;//
		ambientLight1[2] = 0.2f;//blue;//
		ambientLight1[3] = 0.2f;
		

		diffuseLight1[0] = 0.5f;
		diffuseLight1[1] = 0.5f;
		diffuseLight1[2] = 0.5f;
		diffuseLight1[3] = 1.0f;
		
		specularLight1[0] = 1.0f;
		specularLight1[1] = 1.0f;
		specularLight1[2] = 1.0f;
		specularLight1[3] = 1.0f;
		

		lightPos1[0] =  0.0f;
		lightPos1[1] = 200.0f;
		lightPos1[2] =  0.0f;
		lightPos1[3] =1.0f;


		spotDir[0] = spotLightXdirection;
		spotDir[1] =1.0f;
		spotDir[2] = 1.0f; 
		

		//glLightfv(GL_LIGHT1,GL_AMBIENT,ambientLight1);  
		glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuseLight1);  
		glLightfv(GL_LIGHT1,GL_SPECULAR,specularLight1);  
		glLightfv(GL_LIGHT1,GL_POSITION,lightPos1); 

		glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,0.01); 
		glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,0); 
		glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,0);
		
		glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,60.0);
		glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,spotDir);
		glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,2.0);


		//glDisable(GL_LIGHT1);

		glEnable(GL_LIGHT1);
	}
	else
	{
		enableLighting();
	}

	//push();
	//	drawAxis();
	//pop() */

	push();			
		drawField();				
	pop();

	push();	
		glTranslatef(0.0,215,0.0);
		drawFrame();				
	pop();

	push();
		glTranslatef(0.0, 520.0, 34.0);
		drawStructure();		
	pop();

	/*push();		
		drawChamber();
	pop();*/

	FPS();
	glutPostRedisplay();	
	glutSwapBuffers();
	_postDisplay();
}

void processMouse(int button, int state, int x, int y)
{
	/* printf("%f%", l_pDynCam->viewMat.p[0]);
	printf("%f%", l_pDynCam->viewMat.p[1]);
	printf("%f%", l_pDynCam->viewMat.p[2]);
	printf("%f%", l_pDynCam->viewMat.p[3]);
	printf("%f%", l_pDynCam->viewMat.p[4]);
	printf("%f%", l_pDynCam->viewMat.p[5]);
	printf("%f%", l_pDynCam->viewMat.p[6]);
	printf("%f%", l_pDynCam->viewMat.p[7]);
	printf("%f%", l_pDynCam->viewMat.p[8]); */
}

void printCameraFunctions()
{
	printf("Up arrow: look down\n");
	printf("Down arrow: look up\n");
	printf("Left arrow: look left\n");
	printf("Right arrow: look right\n");
	printf("a: go ahead\n");
	printf("z: go back\n");
	printf("Home: rotate clock wise\n");
	printf("Page up: rotate counter clock wise\n");
	printf("End: Translate left\n");
	printf("Page Down: Translate right\n");
	printf("-: Translate up\n");
	printf("+: Translate down\n");
}

void processKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'r':
		anim = !anim;		
		break;
	case 't':
		inRide = !inRide;
		break;
	case 'y':
		xOffset = xOffset == 10 ?  -10 : 10;
		break;
	case 'n':
		isNight = !isNight;
		if (isNight)
		{
			lightVar = 0;
		}

		break;
	case '1':		
		l_pDynCam->LoadCamSrc(79.795479, 134.108841, 643.598572);
		l_pDynCam->LoadMat(-0.995264, -0.003803, -0.097135, -0.041919, 0.918344, 0.393556, 0.087706, 0.395764, -0.914153);
		break;
	case '2':		
		l_pDynCam->LoadCamSrc(16.698103, 464.935120, 220.219070);
		l_pDynCam->LoadMat(-0.924322, 0.175184, -0.339031, -0.079173, 0.781046, 0.619436, 0.373314, 0.599400, -0.708068);
		break;
	default:
		break;
	}
}

void processTimer(int value)
{	
	if(anim)
	{
		angle = angle > 360 ? 0 : angle + 1;
	}

	if (lightVar == 50)
	{
		spotLightXdirection = spotLightXdirection == 1.0f ? -1.0f : 1.0f;
		lightVar = 0;
	}
	else
	{
		lightVar++;
	}

	glutTimerFunc(timerDelay, processTimer, 0);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition (0,0);
	glutInitWindowSize(1024,768);
	glutCreateWindow("3D Ferris Wheel");
	init();
	printCameraFunctions();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(processMouse);
	glutKeyboardFunc(processKeyboard);
	glutTimerFunc(timerDelay ,processTimer, 0);
	glutMainLoop();	
	return 0;
}
