#define _CRT_SECURE_NO_WARNINGS
#include "AVICube.h"
#include <math.h>
#include <time.h>
#include "glm.cpp"
#include "nurbs.cpp"
#define PI 3.1415926
#define  GLUT_WHEEL_UP 3           //定义滚轮操作  
#define  GLUT_WHEEL_DOWN 4  
using namespace std;

#define BITMAP_ID 0x4D42
Nurbs nurbs;
auto aviCube = new AVICube(5, 0, 0, 0, "Face3.avi", "taxi.avi", "windows.avi", "windows.avi", "windows.avi", "windows.avi");

float eye[] = { 15, 5, -5 };
float center[] = { 15, 5, -10 };
float polar[] = {5, -90, 90};
GLfloat light_p[] = { 10,8,-10,1 };
GLfloat yellow_l[] = { 1,1,1,1 };
GLfloat blue_l[] = { 0,0,1,1 };
int color = 0;
float zoom = 60;
float Height, Width;
float r = 0;
bool g_on = false;
bool tv_on = false;
GLMmodel *kitcken_model, *chaji_model, *sofa_model, *bed_model, *tv_model, *door_model,*light_model;
int l_button_down = FALSE;
struct P                      /*用于记录鼠标位置*/
{
	int x;
	int y;
}oldpt;

struct {
	bool exist = false;
	bool rotate = false;
	float zoom = 1;
	int color = 0;
	int tex = 0;
	float pos[3];
} object[10];
int object_id = 1;

GLuint texture[15];

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;	// 文件指针
	BITMAPFILEHEADER bitmapFileHeader;	// bitmap文件头
	unsigned char	*bitmapImage;		// bitmap图像数据
	int	imageIdx = 0;		// 图像位置索引
	unsigned char	tempRGB;	// 交换变量

								// 以“二进制+读”模式打开文件filename 
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL) return NULL;
	// 读入bitmap文件图
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// 验证是否为bitmap文件
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file\n");
		return NULL;
	}

	// 读入bitmap信息头
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// 将文件指针移至bitmap数据
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// 为装载图像数据创建足够的内存
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
	// 验证内存是否创建成功
	if (!bitmapImage) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}
	// 读入bitmap图像数据
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// 确认读入成功
	if (bitmapImage == NULL) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	//由于bitmap中保存的格式是BGR，下面交换R和B的值，得到RGB格式
	for (imageIdx = 0;
	imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	// 关闭bitmap图像文件
	fclose(filePtr);
	return bitmapImage;
}

void texload(int i, char *filename)
{
	BITMAPINFOHEADER bitmapInfoHeader;                                 // bitmap信息头
	unsigned char*   bitmapData;                                       // 纹理数据

	bitmapData = LoadBitmapFile(filename, &bitmapInfoHeader);
	glBindTexture(GL_TEXTURE_2D, texture[i]);
	// 指定当前纹理的放大/缩小过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);
}

void init()
{
	nurbs.init();
	glGenTextures(10, texture);                                         // 第一参数是需要生成标示符的个数, 第二参数是返回标示符的数组
	texload(0, "table2.bmp");
	texload(1, "3.bmp");
	texload(2, "qb1.bmp");
	texload(3, "qb0.bmp");

	texload(8, "chaji.bmp");
	texload(4, "metal.bmp");
	texload(5, "sofa.bmp");
	texload(6, "bed.bmp");
	texload(7, "tv.bmp");
	texload(9, "door.bmp");
	texload(10, "deng.bmp");

	kitcken_model = glmReadOBJ("obj/kitchen.obj");
	glmUnitize(kitcken_model);
	glmScale(kitcken_model, 6);
	glmFacetNormals(kitcken_model);
	glmVertexNormals(kitcken_model, 90);


	chaji_model = glmReadOBJ("obj/chaji.obj");
	glmUnitize(chaji_model);
	glmScale(chaji_model, 3);
	glmFacetNormals(chaji_model);
	glmVertexNormals(chaji_model, 90);


	sofa_model = glmReadOBJ("obj/sofa.obj");
	glmUnitize(sofa_model);
	glmScale(sofa_model, 4);
	glmFacetNormals(sofa_model);
	glmVertexNormals(sofa_model, 90);

	bed_model = glmReadOBJ("obj/bed.obj");
	glmUnitize(bed_model);
	glmScale(bed_model, 6);
	glmFacetNormals(bed_model);
	glmVertexNormals(bed_model, 90);

	tv_model = glmReadOBJ("obj/tv.obj");
	glmUnitize(tv_model);
	glmScale(tv_model, 6);
	glmFacetNormals(tv_model);
	glmVertexNormals(tv_model, 90);

	door_model = glmReadOBJ("obj/door.obj");
	glmUnitize(door_model);
	glmScale(door_model, 6);
	glmFacetNormals(door_model);
	glmVertexNormals(door_model, 90);


	light_model = glmReadOBJ("obj/deng.obj");
	glmUnitize(light_model);
	glmScale(light_model, 1);
	glmFacetNormals(light_model);
	glmVertexNormals(light_model, 90);





}

void draw_cuboid(float x, float y, float z) {
	//1
	glBegin(GL_QUADS);
	glTexCoord2i(1, 1); glVertex3f(0, 0, 0);
	glTexCoord2i(1, 0); glVertex3f(x, 0, 0);
	glTexCoord2i(0, 0); glVertex3f(x, 0, -z);
	glTexCoord2i(0, 1); glVertex3f(0, 0, -z);
	glEnd();

	//2
	glBegin(GL_QUADS);
	glTexCoord2i(1, 1); glVertex3f(0, y, 0);
	glTexCoord2i(1, 0); glVertex3f(x, y, 0);
	glTexCoord2i(0, 0); glVertex3f(x, y, -z);
	glTexCoord2i(0, 1); glVertex3f(0, y, -z);
	glEnd();

	//3
	glBegin(GL_QUADS);
	glTexCoord2i(1, 1); glVertex3f(0, 0, 0);
	glTexCoord2i(1, 0); glVertex3f(x, 0, 0);
	glTexCoord2i(0, 0); glVertex3f(x, y, 0);
	glTexCoord2i(0, 1); glVertex3f(0, y, 0);
	glEnd();

	//4
	glBegin(GL_QUADS);
	glTexCoord2i(1, 1); glVertex3f(0, 0, -z);
	glTexCoord2i(1, 0); glVertex3f(x, 0, -z);
	glTexCoord2i(0, 0); glVertex3f(x, y, -z);
	glTexCoord2i(0, 1); glVertex3f(0, y, -z);
	glEnd();

	//5
	glBegin(GL_QUADS);
	glTexCoord2i(1, 1); glVertex3f(0, 0, 0);
	glTexCoord2i(1, 0); glVertex3f(0, y, 0);
	glTexCoord2i(0, 0); glVertex3f(0, y, -z);
	glTexCoord2i(0, 1); glVertex3f(0, 0, -z);
	glEnd();

	//6
	glBegin(GL_QUADS);
	glTexCoord2i(1, 1); glVertex3f(x, 0, 0);
	glTexCoord2i(1, 0); glVertex3f(x, y, 0);
	glTexCoord2i(0, 0); glVertex3f(x, y, -z);
	glTexCoord2i(0, 1); glVertex3f(x, 0, -z);
	glEnd();
}

void draw_fan(float rotate) {
	glPushMatrix();
	glRotatef(rotate, 0, 1, 0);
	glScalef(4, 0.1, 1);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glRotatef(rotate + 90, 0, 1, 0);
	glScalef(4, 0.1, 1);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0.5, 0);
	glScalef(0.2, 1, 0.2);
	glutSolidCube(1);
	glPopMatrix();
}

void Draw(){
	glPushMatrix();
	glTranslatef(15, 0, -10);
	nurbs.drawNurbs();
	glPopMatrix();

	for (int i = 1; i < 3; i++) {
		if (object[i].exist) {
			switch (i) {
			case 1: {
				if (object[i].tex) {
					glEnable(GL_TEXTURE_2D);
					//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
					glBindTexture(GL_TEXTURE_2D, texture[object[i].tex + 8]);
				}
				glPushMatrix();
				glTranslatef(object[i].pos[0], object[i].pos[1], object[i].pos[2]);
				if (object[i].rotate)
					glRotatef(r, 0, 1, 0);
				glScalef(object[i].zoom, object[i].zoom, object[i].zoom);
				glutSolidTeapot(0.4);
				glPopMatrix();
				if (object[i].tex) {
					glDisable(GL_TEXTURE_2D);
				}
				break;
			}
			case 2: {
				if (object[i].color) {
					GLfloat teapot[] = { 0.85, 0.65, 0.25, 1.0 };
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, teapot);
				}
				glPushMatrix();
				glTranslatef(object[i].pos[0], object[i].pos[1], object[i].pos[2]);
				if (object[i].rotate)
					glRotatef(r, 0, 1, 0);
				glScalef(object[i].zoom, object[i].zoom, object[i].zoom);
				glutSolidCube(0.4);
				glPopMatrix();
				if (object[i].color) {
					GLfloat default[] = { 0.8, 0.8, 0.8, 1.0 };
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, default);
				}
				break;
			}
			}
		}
	}

	glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	
	//glColor3f(1, 0, 1);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2i(100, 100); glVertex3f(100, 0, 100);
	glTexCoord2i(100, 0); glVertex3f(100, 0, -100);
	glTexCoord2i(0, 0); glVertex3f(-100, 0, -100);
	glTexCoord2i(0, 100); glVertex3f(-100, 0, 100);
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	
	glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	//glColor3f(0, 1, 0);
	glPushMatrix();
	draw_cuboid(30, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	draw_cuboid(15, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.5, 0, -15);
	draw_cuboid(7.5, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.5, 0, -15);
	glRotatef(90, 0, 1, 0);
	draw_cuboid(15, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7, 0, -30);
	draw_cuboid(30.5, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(37.5, 0, -12);
	glRotatef(90, 0, 1, 0);
	draw_cuboid(18, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(30, 0, -11.5);
	draw_cuboid(7.5, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(30, 0, -0.5);
	glRotatef(90, 0, 1, 0);
	draw_cuboid(11.5, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(16, 0, -11.5);
	draw_cuboid(13.5, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(16.5, 0, -5.5);
	glRotatef(90, 0, 1, 0);
	draw_cuboid(6, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(16.5, 7, -2.5);
	glRotatef(90, 0, 1, 0);
	draw_cuboid(3, 2, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(16.5, 0, -0.5);
	glRotatef(90, 0, 1, 0);
	draw_cuboid(2, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.5, 0, -16.5);
	draw_cuboid(10, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(17.5, 7, -16.5);
	draw_cuboid(3, 2, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(20.5, 0, -16.5);
	draw_cuboid(3.5, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(24, 7, -16.5);
	draw_cuboid(3, 2, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(27, 0, -16.5);
	draw_cuboid(10, 9, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(22.5, 0, -16.5);
	glRotatef(90, 0, 1, 0);
	draw_cuboid(13.5, 9, 0.5);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	//glColor3f(1, 0, 1);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2i(100, 100); glVertex3f(100, 9, 100);
	glTexCoord2i(100, 0); glVertex3f(100, 9, -100);
	glTexCoord2i(0, 0); glVertex3f(-100, 9, -100);
	glTexCoord2i(0, 100); glVertex3f(-100, 9, 100);
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glPushMatrix();
	glTranslatef(10, 8, -10);
	draw_fan(r);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D);//chu gui
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTranslatef(23, 3, -9.5);
	glRotatef(270, 0, 1, 0);
	glmDraw(kitcken_model, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D);//zhuozi
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glTranslatef(14, 1, -23);
	glRotatef(270, 0, 1, 0);
	glmDraw(chaji_model, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D); // sofa
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTranslatef(19.5, 1, -23);
	glRotatef(270, 0, 1, 0);
	glmDraw(sofa_model, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D); // bed
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glTranslatef(33.5, 4, -23);
	glRotatef(270, 0, 1, 0);
	glmDraw(bed_model, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D); // tv
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	glTranslatef(9, 3, -23);
	glRotatef(90, 0, 1, 0);
	glScalef(1.8, 1.8, 1);
	glmDraw(tv_model, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D); // door
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[9]);
	glTranslatef(4, 0, -0.3);
	glScalef(1, 1.4, 1);
	glmDraw(door_model, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	/*glPushMatrix();
	glTranslatef(light_p[0], light_p[1], light_p[2]);
	glutSolidSphere(2, 10, 10);

	glPopMatrix();*/

}

void reshape(int width, int height){
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}
	Height = height;
	Width = width;

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	float whRatio = (GLfloat)width / (GLfloat)height;
	gluPerspective(zoom, whRatio, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void idle(){
	r += 0.8;
	glutPostRedisplay();
}

//屏幕保存
BOOL WriteBitmapFile(char * filename, int width, int height, unsigned char * bitmapData)
{
	//填充BITMAPFILEHEADER
	BITMAPFILEHEADER bitmapFileHeader;
	memset(&bitmapFileHeader, 0, sizeof(BITMAPFILEHEADER));
	bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
	bitmapFileHeader.bfType = 0x4d42;	//BM
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//填充BITMAPINFOHEADER
	BITMAPINFOHEADER bitmapInfoHeader;
	memset(&bitmapInfoHeader, 0, sizeof(BITMAPINFOHEADER));
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = width;
	bitmapInfoHeader.biHeight = height;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfoHeader.biSizeImage = width * abs(height) * 3;

	//////////////////////////////////////////////////////////////////////////
	FILE * filePtr;			//连接要保存的bitmap文件用
	unsigned char tempRGB;	//临时色素
	int imageIdx;

	//交换R、B的像素位置,bitmap的文件放置的是BGR,内存的是RGB
	for (imageIdx = 0; imageIdx < bitmapInfoHeader.biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapData[imageIdx];
		bitmapData[imageIdx] = bitmapData[imageIdx + 2];
		bitmapData[imageIdx + 2] = tempRGB;
	}

	filePtr = fopen(filename, "wb");
	if (NULL == filePtr)
	{
		return FALSE;
	}

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	fwrite(bitmapData, bitmapInfoHeader.biSizeImage, 1, filePtr);

	fclose(filePtr);
	return TRUE;
}

void saveScreen()
{
	int clnWidth, clnHeight;	//client width and height
	static void * screenData;
	RECT rc;
	int len = 800 * 600 * 3;
	screenData = malloc(len);
	memset(screenData, 0, len);
	glReadPixels(0, 0, 800, 600, GL_RGB, GL_UNSIGNED_BYTE, screenData);

	//生成文件名字符串，以时间命名
	time_t tm = 0;
	tm = time(NULL);
	char lpstrFilename[256] = { 0 };
	sprintf_s(lpstrFilename, sizeof(lpstrFilename), "pic/%d.bmp", tm);

	WriteBitmapFile(lpstrFilename, 800, 600, (unsigned char*)screenData);
	//释放内存
	free(screenData);

}

bool checkCollision(float pos[3]) {
	if (pos[0] > 0.2&&pos[0] < 15.8) {
		if (pos[2] < -0.7 && pos[2] > -14.8)
			return false;
	}

	if (pos[0] > 7.7&&pos[0] < 36.8) {
		if (pos[2] < -12.2 && pos[2] > -16.3)
			return false;
	}

	if (pos[0] > 16.7&&pos[0] < 29.3) {
		if (pos[2] < -0.7 && pos[2] > -11.3)
			return false;
	}

	if (pos[0] > 0.2&&pos[0] <= 11.8) {
		if (pos[2] < -0.7 && pos[2] > -14.8)
			return false;
	}

	if (pos[0] > 7.7&&pos[0] < 21.8) {
		if (pos[2] < -17.2 && pos[2] > -29.8)
			return false;
	}

	if (pos[0] > 22.7&&pos[0] < 36.8) {
		if (pos[2] < -17.2 && pos[2] > -29.8)
			return false;
	}

	if (pos[0] >= 17.7&&pos[0] <= 20.3) {
		if (pos[2] <= -16.3 && pos[2] >= -17.2 && pos[1]<6.8)
			return false;
	}

	if (pos[0] >= 24.2&&pos[0] <= 26.8) {
		if (pos[2] <= -16.3 && pos[2] >= -17.2 && pos[1]<6.8)
			return false;
	}

	if (pos[0] >= 15.8&&pos[0] <= 16.7) {
		if (pos[2] <= -2.7 && pos[2] >= -5.3 && pos[1]<6.8)
			return false;
	}

	return true;                 
}

void checkPos(unsigned char k, int x, int y) {
	GLfloat dz = center[2] - eye[2];
	GLfloat dx = center[0] - eye[0];
	GLfloat angle = abs(atan(dz/dx));
	float old_eye[3], old_center[3];
	old_eye[0] = eye[0]; old_eye[1] = eye[1]; old_eye[2] = eye[2];
	old_center[0] = center[0]; old_center[1] = center[1]; old_center[2] = center[2];

	switch (k)
	{
	case 'a': {
		eye[0] += 0.2f * sin(angle) * (dz == 0 ? 0 : (dz / abs(dz)));
		center[0] += 0.2f * sin(angle) * (dz == 0 ? 0 : (dz / abs(dz)));
		eye[2] -= 0.2f * cos(angle) * (dx == 0 ? 0 : (dx / abs(dx)));
		center[2] -= 0.2f * cos(angle) * (dx == 0 ? 0 : (dx / abs(dx)));
		break;
	}
	case 'd': {
		eye[0] -= 0.2f * sin(angle) * (dz == 0 ? 0 : (dz / abs(dz)));
		center[0] -= 0.2f * sin(angle) * (dz == 0 ? 0 : (dz / abs(dz)));
		eye[2] += 0.2f * cos(angle) * (dx == 0 ? 0 : (dx / abs(dx)));
		center[2] += 0.2f * cos(angle) * (dx == 0 ? 0 : (dx / abs(dx)));
		break;
	}
	case 'c': {
		eye[1] -= 0.2f;
		center[1] -= 0.2f;
		break;
	}
	case 'z': {
		eye[1] += 0.2f;
		center[1] += 0.2f;
		break;
	}
	case 'w': {
		eye[0] += 0.2f * cos(angle) * (dx == 0 ? 0 : (dx / abs(dx)));
		center[0] += 0.2f * cos(angle) * (dx == 0 ? 0 : (dx / abs(dx)));
		eye[2] += 0.2f * sin(angle) * (dz == 0 ? 0 : (dz / abs(dz)));
		center[2] += 0.2f * sin(angle) * (dz == 0 ? 0 : (dz / abs(dz)));
		break;
	}
	case 's': {
		eye[0] -= 0.2f * cos(angle) * (dx == 0 ? 0 : (dx / abs(dx)));
		center[0] -= 0.2f * cos(angle) * (dx == 0 ? 0 : (dx / abs(dx)));
		eye[2] -= 0.2f * sin(angle) * (dz == 0 ? 0 : (dz / abs(dz)));
		center[2] -= 0.2f * sin(angle) * (dz == 0 ? 0 : (dz / abs(dz)));
		break;
	}
	}

	if(!g_on)
		if (eye[1] >= 8.8 || eye[1] <= 0.5 || checkCollision(eye)) {
			eye[0] = old_eye[0]; eye[1] = old_eye[1]; eye[2] = old_eye[2];
			center[0] = old_center[0]; center[1] = old_center[1]; center[2] = old_center[2];
		}
}

void key(unsigned char k, int x, int y){
	if (k <= '9' && k >= '0') {
		object_id = k - '0';
	}
	switch (k)
	{
	case 'h':{
		tv_on = !tv_on;
	}
	case 'x': {
		object[object_id].rotate = !object[object_id].rotate;
		break;
	}
	case 'v': {
		if (object[object_id].zoom >= 0.5)
			object[object_id].zoom -= 0.1;
		break;
	}
	case 'b': {
		if (object[object_id].zoom <= 2)
			object[object_id].zoom += 0.1;
		break;
	}
	case 'i': {
		object[object_id].pos[2] -= 0.2;
		break;
	}
	case 'k': {
		object[object_id].pos[2] = object[object_id].pos[2] + 0.2;
		break;
	}
	case 'j': {
		object[object_id].pos[0] -= 0.2;
		break;
	}
	case 'l': {
		object[object_id].pos[0] += 0.2;
		break;
	}
	case 'n': {
		object[object_id].pos[1] += 0.2;
		break;
	}
	case 'm': {
		object[object_id].pos[1] -= 0.2;
		break;
	}
	case '=': {
		if (!object[object_id].exist) {
			object[object_id].exist = true;
			object[object_id].pos[0] = center[0];
			object[object_id].pos[1] = center[1];
			object[object_id].pos[2] = center[2];
		}
		break;
	}
	case '-': {
		object[object_id].exist = false;
		break;
	}
	case 't': {
		if (object[object_id].exist) {
			if (++object[object_id].tex == 3)
				object[object_id].tex = 0;
		}
		break;
	}
	case 'y': {
		if (object[object_id].exist) {
			if (++object[object_id].color == 3)
				object[object_id].color = 0;
		}
		break;
	}
	case 'a': {
		checkPos(k, x, y);
		break;
	}
	case 'd': {
		checkPos(k, x, y);
		break;
	}
	case 'c': {
		checkPos(k, x, y);
		break;
	}
	case 'z': {
		checkPos(k, x, y);
		break;
	}
	case 'w': {
		checkPos(k, x, y);
		break;
	}
	case 's': {
		checkPos(k, x, y);
		break;
	}
	case ',': {
		if (zoom >= 15) zoom -= 1;
		reshape(Width, Height);
		break;
	}
	case '.': {
		if (zoom <= 80) zoom += 1;
		reshape(Width, Height);
		break;
	}
	case '/': {
		zoom = 60;
		reshape(Width, Height);
		break;
	}
	case 'p': {
		saveScreen();
	}
	case 'g':{
		g_on = !g_on;
	}

	case '6': {light_p[0]++; break; }
	case '4': {light_p[0]--; break; }
	case '8': {light_p[1]++; break; }
	case '2': {light_p[1]--; break; }
	case '7': {light_p[2]++; break; }
	case '9': {light_p[2]--; break; }
	case '*': {color=1-color; break; }
	}
}

void redraw(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);

	glEnable(GL_DEPTH_TEST);

	Draw();
	if (tv_on) {
		aviCube->render();
		aviCube->CheckFrame(16);
	}
		

	// light
	GLfloat sun_light_ambient[] = { 1, 1, 1, 1.0f };
	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0, GL_POSITION, light_p); //指定光源的位置
	if (color == 0)
	{
		glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow_l);  //设定漫反射效果
		glLightfv(GL_LIGHT0, GL_SPECULAR, yellow_l); //设定高光反射效果
	}
	else
	{
		glLightfv(GL_LIGHT0, GL_DIFFUSE, blue_l);  //设定漫反射效果
		glLightfv(GL_LIGHT0, GL_SPECULAR, blue_l); //设定高光反射效果
	}
	glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_LIGHTING); //启用光源
	glEnable(GL_LIGHT0);   //使用指定灯光
	glEnable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y)        /*当鼠标按下或拿起时会回调该函数*/
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		l_button_down = TRUE;                /*当l_button_down为true时才可改变相机位置*/
		oldpt.x = x;                      /*记录旧坐标，以便移动发生时计算鼠标移动方向*/
		oldpt.y = y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		l_button_down = FALSE;        /*拿起时不能改变摄像机位置*/
	}
}

void OnMouseMove(int x, int y)          /*当鼠标移动时会回调该函数*/
{
	float angle;
	if (l_button_down)         /*如果鼠标没有按下则不改变摄像机位置*/
	{
		float dir_x = float(x - oldpt.x) / 15;
		float dir_y = float(y - oldpt.y) / 15;       /*根据增量重新设定摄像机位置*/
			
		GLfloat alpha, fy, D[3];                  /*和它的名字一样，不过是单位是弧度*/
		if ((polar[2] + dir_y)>5.0f && (polar[2] + dir_y)<175.0f)
		{     /*定义竖直偏角只能在5°到175°之间*/
			polar[1] += dir_x;                  /*根据鼠标移动的方向设置新的球坐标*/
			polar[2] += dir_y;

			if (polar[1]>360.0f) polar[1] -= 360.0f;
			if (polar[1]<0.0f) polar[1] += 360.0f;      /*将水平偏角锁定在0°到360°之间*/

			alpha = polar[1]*PI / 180;
			fy = polar[2]*PI / 180;               /*角度转弧度*/
			D[0] = -1 * polar[0] * sin(fy) * cos(alpha);       /*极坐标转直角坐标*/
			D[2] = polar[0] * sin(fy) * sin(alpha);
			D[1] = -1 * polar[0] * cos(fy);                 /*注意：竖直方向的是y轴*/
			center[0] = eye[0] + D[0];
			center[1] = eye[1] + D[1];
			center[2] = eye[2] + D[2];
		}
		oldpt.x = x;              /*将当前坐标置为旧坐标*/
		oldpt.y = y;
	}
}

int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	int windowHandle = glutCreateWindow("Room");

	init();

	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutMouseFunc(Mouse);
	glutMotionFunc(OnMouseMove);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}