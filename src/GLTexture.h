#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library



class GLTexture  
{
public:
	char *texturename;								// The textures name
	unsigned int texture[1];						// OpenGL's number for the texture
	int width;										// Texture's width
	int height;										// Texture's height
	void Use();										// Binds the texture for use
	
	
	void LoadBMP(char *name);						// Loads a bitmap file
	


	GLTexture();									// Constructor
	virtual ~GLTexture();							// Destructor

};

#endif GLTEXTURE_H