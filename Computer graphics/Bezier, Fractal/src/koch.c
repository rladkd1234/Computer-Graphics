#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define W 600
#define H 600
#define M_PI 3.1415
#define ROUND(x) (int)(x+0.5)
#define STEP 3

unsigned char oImg[H][W][3];

enum kindOfLine { solid, dashed, dotted };
int lpalette[3][8] = { {1,1,1,1,1,1,1,1}, {1,1,1,1,0,0,0,0},{1,0,1,0,1,0,1,0} };

void init_pixel()
{
	memset(oImg, 255, sizeof(oImg));
}

void set_pixel(int x, int y, int r, int g, int b)
{
	oImg[y][x][0] = r;
	oImg[y][x][1] = g;
	oImg[y][x][2] = b;
}

void lineDDA(int x1, int y1, int x2, int y2, int r, int g, int b, int kind)
{
	int dx, dy, step, k;
	double x, y, incx, incy;
	dx = x2 - x1, dy = y2 - y1;
	x = (double)x1, y = (double)y1;

	if (abs(dx) > abs(dy)) step = abs(dx);
	else step = abs(dy);

	incx = dx / (double)step;
	incy = dy / (double)step;

	for (k = 0; k < step; k++)
	{
		x += incx;
		y += incy;

		if (lpalette[kind][k % 8] == 1) {
			set_pixel((int)(x + 0.5), (int)(y + 0.5), r, g, b);
		}

	}
}

void makePPM(char* filename)
{
	FILE* fp;
	fp = fopen(filename, "wb");
	fprintf(fp, "P6\n"); //magic no.: ASCII header and Binary body.. 
	fprintf(fp, "%d %d\n", H, W); //file size 
	fprintf(fp, "255\n"); //max level 
	fwrite(oImg, sizeof(char), H * W * 3, fp);

	fclose(fp);
}


int main(void)
{
	init_pixel();	// oImg 흰바탕으로 설정

	int px[4] = { 50, 300, 500, 50 };
	int py[4] = { 450, 50, 500, 450 };
	int *kx, *ky;	//kouch 좌표
	int *tmpX, *tmpY;	
	int tp = sizeof(px) / sizeof(int);	

	tmpX = malloc(sizeof(int) * tp);
	tmpY = malloc(sizeof(int) * tp);
	
	for (int i = 0; i < tp; i++) {
		tmpX[i] = px[i];
		tmpY[i] = py[i];
	}

	for (int i = 0; i < STEP; i++) {

		tp += (tp - 1) * 3;
		//새로운 좌표 수 만큼 확장
		kx = malloc(sizeof(int) * tp);
		ky = malloc(sizeof(int) * tp);

		//p0, p1 값 설정
		for (int j = 0; j < tp; j += 4) {
			kx[j] = tmpX[j / 4];
			ky[j] = tmpY[j / 4];
		}
		//I1 계산
		for (int j = 1; j < tp; j += 4) {
			//k[j-1] p0 + 1/3dp
			kx[j] = kx[j - 1] + (kx[j + 3] - kx[j - 1]) / 3;
			ky[j] = ky[j - 1] + (ky[j + 3] - ky[j - 1]) / 3;
		}
		//I3 계산 p1 - 1/3dp
		for (int j = 3; j < tp; j += 4) {
			kx[j] = kx[j + 1] - (kx[j + 1] - kx[j - 3]) / 3;
			ky[j] = ky[j + 1] - (ky[j + 1] - ky[j - 3]) / 3;
		}
		//I2 계산 (I1을 고정점 I3를 ccw(60도) 회전)
		for (int j = 2; j < tp; j += 4) {
			kx[j] = (kx[j + 1] - kx[j - 1]) * cos(-M_PI / 180 * 60) - (ky[j + 1] - ky[j - 1])*sin(-M_PI / 180 * 60) + kx[j - 1];
			ky[j] = (kx[j + 1] - kx[j - 1]) * sin(-M_PI / 180 * 60) + (ky[j + 1] - ky[j - 1])*cos(-M_PI / 180 * 60) + ky[j - 1];
		}

		tmpX = malloc(sizeof(int) * tp);
		tmpY = malloc(sizeof(int) * tp);
		memcpy(tmpX, kx, sizeof(int)*tp);
		memcpy(tmpY, ky, sizeof(int)*tp);

		if (i != STEP - 1) 
		{ 
			free(kx); 
			free(ky);
		}
	}

	for (int i = 0; i < tp - 1; i++)
		lineDDA(kx[i], ky[i], kx[i + 1], ky[i + 1], 0, 0, 0, 0);
	
	makePPM("KochCurve.ppm");

	return 0;
}