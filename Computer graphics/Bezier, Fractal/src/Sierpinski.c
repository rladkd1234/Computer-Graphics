#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define W 600
#define H 600
#define M_PI 3.1415
#define ROUND(x) (int)(x+0.5)
#define STEP 3

unsigned char oImg[H][W][3];

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

	int px[3] = { H-1, H-1, 0 };
	int py[3] = { 0, W-1, W/2 };

	// 현재점 선택

	double ptx = px[0];
	double pty = py[0];
	
	int select;

	srand((unsigned int)time(NULL));

	for (int i = 0;  i < 10000; i++) {
		//임의의 점 선택
		select = rand() % 3;
		ptx = (ptx + px[select]) / 2.0;
		pty = (pty + py[select]) / 2.0;
		set_pixel((int)ptx, (int)pty, 0, 0, 0);

	}

	makePPM("SierpinskiGasket.ppm");

	return 0;
}