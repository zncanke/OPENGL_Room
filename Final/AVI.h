
#pragma once
#include "Definition.h"
#include <vfw.h>												// Header File For Video For Windows
#include <stdlib.h>

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking
#pragma comment( lib, "vfw32.lib" )								// Search For VFW32.lib While Linking

class AVI
{
private:
	void flipIt(void* buffer);
	AVISTREAMINFO		psi;										// Pointer To A Structure Containing Stream Info
	PAVISTREAM			pavi;										// Handle To An Open Stream
	PGETFRAME			pgf;										// Pointer To A GetFrame Object
	BITMAPINFOHEADER	bmih;										// Header Information For DrawDibDraw Decoding
	long				lastframe;									// Last Frame Of The Stream
	int					width;										// Video Width
	int					height;										// Video Height
	char				*pdata;										// Pointer To Texture Data
	int					mpf;										// Will Hold Rough Milliseconds Per Frame
	HDRAWDIB			hdd;										// Handle For Our Dib
	HBITMAP				hBitmap;									// Handle To A Device Dependant Bitmap
	HDC					hdc;										// Creates A Compatible Device Context
	unsigned char*		data;										// Pointer To Our Resized Image
	int					next;										// Used For Animation
	int					frame;										// Frame Counter
public:
	AVI(void);
	void OpenAVI(LPCSTR szFile);
	void GrabAVIFrame();
	void CheckFrame(DWORD milliseconds);
	void CloseAVI(void);
	~AVI(void);
	// Getters and Setters
	int getFrame();
	void setFrame(int newFrame);

	long getLastFrame();

	int getNext();
	void setNext(int newNext);

	int getMpf();
	void setMpf(int newMpf);	
};

