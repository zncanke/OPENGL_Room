#include "AVI.h"


AVI::AVI(void)
{
	hdd = DrawDibOpen();
	hdc = CreateCompatibleDC(0);								// Creates A Compatible Device Context
	data = 0;													// Pointer To Our Resized Image
	frame = 0;													// Frame Counter
	next = 0;
}

 void AVI::OpenAVI(LPCSTR szFile)
{
	AVIFileInit();												// Opens The AVIFile Library

	// Opens The AVI Stream
	if (AVIStreamOpenFromFileA(&pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL) !=0)
	{
		// An Error Occurred Opening The Stream
		//MessageBox (HWND_DESKTOP, "Failed To Open The AVI Stream", "Error", MB_OK | MB_ICONEXCLAMATION);
		// Exit Program
		exit(0);
	}

	AVIStreamInfo(pavi, &psi, sizeof(psi));						// Reads Information About The Stream Into psi
	width=psi.rcFrame.right-psi.rcFrame.left;					// Width Is Right Side Of Frame Minus Left
	height=psi.rcFrame.bottom-psi.rcFrame.top;					// Height Is Bottom Of Frame Minus Top

	lastframe=AVIStreamLength(pavi);							// The Last Frame Of The Stream

	mpf=AVIStreamSampleToTime(pavi,lastframe)/lastframe;		// Calculate Rough Milliseconds Per Frame

	bmih.biSize = sizeof (BITMAPINFOHEADER);					// Size Of The BitmapInfoHeader
	bmih.biPlanes = 1;											// Bitplanes	
	bmih.biBitCount = 24;										// Bits Format We Want (24 Bit, 3 Bytes)
	bmih.biWidth = 256;											// Width We Want (256 Pixels)
	bmih.biHeight = 256;										// Height We Want (256 Pixels)
	bmih.biCompression = BI_RGB;								// Requested Mode = RGB

	hBitmap = CreateDIBSection (hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&data), NULL, NULL);
	SelectObject (hdc, hBitmap);								// Select hBitmap Into Our Device Context (hdc)

	pgf=AVIStreamGetFrameOpen(pavi, NULL);						// Create The PGETFRAME	Using Our Request Mode

	if (pgf==NULL)
	{
		// An Error Occurred Opening The Frame
		MessageBoxA (HWND_DESKTOP, "Failed To Open The AVI Frame", "Error", MB_OK | MB_ICONEXCLAMATION);
		// Exit Program
		exit(0);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// Set Texture Max Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Set Texture Min Filter
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For S To Sphere Mapping
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For T To Sphere Mapping
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // added by me
}

 void AVI::GrabAVIFrame()										// Grabs A Frame From The Stream
{
	LPBITMAPINFOHEADER lpbi;									// Holds The Bitmap Header Information
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);	// Grab Data From The AVI Stream
	pdata=(char *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

	// Convert Data To Requested Bitmap Format
	DrawDibDraw (hdd, hdc, 0, 0, 256, 256, lpbi, pdata, 0, 0, width, height, 0);

	flipIt(data);												// Swap The Red And Blue Bytes (GL Compatability)

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// Set Texture Max Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Set Texture Min Filter
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For S To Sphere Mapping
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);		// Set The Texture Generation Mode For T To Sphere Mapping
	// Update The Texture
	//glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

 void AVI::CloseAVI(void)										// Properly Closes The Avi File
{
	DeleteObject(hBitmap);										// Delete The Device Dependant Bitmap Object
	DrawDibClose(hdd);											// Closes The DrawDib Device Context
	AVIStreamGetFrameClose(pgf);								// Deallocates The GetFrame Resources
	AVIStreamRelease(pavi);										// Release The Stream
	AVIFileExit();												// Release The File
}
 
 void AVI::flipIt(void* buffer)									// Flips The Red And Blue Bytes (256x256)
{
	void* b = buffer;											// Pointer To The Buffer
	__asm														// Assembler Code To Follow
	{
		mov ecx, 256*256										// Counter Set To Dimensions Of Our Memory Block
		mov ebx, b												// Points ebx To Our Data (b)
		label:													// Label Used For Looping
			mov al,[ebx+0]										// Loads Value At ebx Into al
			mov ah,[ebx+2]										// Loads Value At ebx+2 Into ah
			mov [ebx+2],al										// Stores Value In al At ebx+2
			mov [ebx+0],ah										// Stores Value In ah At ebx
			
			add ebx,3											// Moves Through The Data By 3 Bytes
			dec ecx												// Decreases Our Loop Counter
			jnz label											// If Not Zero Jump Back To Label
	}
}

  void AVI::CheckFrame(DWORD milliseconds)
 {
	next+=milliseconds;											// Increase next Based On The Timer
	frame=next/mpf;												// Calculate The Current Frame

	if (frame>=lastframe)										// Are We At Or Past The Last Frame?
	{
		frame=0;												// Reset The Frame Back To Zero (Start Of Video)
		next=0;													// Reset The Animation Timer (next)
	}
 }

 int AVI::getFrame()
 {
	 return frame;
 }

 void AVI::setFrame(int newFrame)
 {
	 frame = newFrame;
 }

 long AVI::getLastFrame()
 {
	 return lastframe;
 }

 int AVI::getNext()
 {
	 return next;
 }

 void AVI::setNext(int newNext)
 {
	next=newNext;
 }

 int AVI::getMpf()
 {
	 return mpf;
 }

 void AVI::setMpf(int newMpf)
 {
	 mpf=newMpf;
 }

AVI::~AVI(void) // deconstructor
{
}
