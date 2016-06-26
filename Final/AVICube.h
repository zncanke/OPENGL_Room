#pragma once // comment out the header files you dont need
/*
#include <windows.h>											// Header File For Windows
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include <vfw.h>												// Header File For Video For Windows
#include "NeHeGL.h"												// Header File For NeHeGL
*/
#include "AVI.h" // added myself

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking
#pragma comment( lib, "vfw32.lib" )								// Search For VFW32.lib While Linking
class AVICube
{
private:
	int size; // size of the cube being drawn
	float xOff;
	float yOff;
	float zOff;

	// Video Files for each face
	AVI video1; // Front Face
	AVI video2; // Back Face
	AVI video3; // Top Face
	AVI video4; // Bottom Face	
	AVI video5; // Right Face
	AVI video6; // Left Face
public:
	AVICube(int cubeSize, LPCSTR szFile1, LPCSTR szFile2, LPCSTR szFile3, LPCSTR szFile4, LPCSTR szFile5, LPCSTR szFile6);
	AVICube(int cubeSize, float newX, float newY, float newZ, LPCSTR szFile1, LPCSTR szFile2, LPCSTR szFile3, LPCSTR szFile4, LPCSTR szFile5, LPCSTR szFile6);
	void render();
	void CheckFrame(DWORD milliseconds);
	void CloseVideo();
	~AVICube(void);
};

