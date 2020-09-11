#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define W 1000
#define H 1000
#define M_PI 3.1415
#define ROUND(x) (int)(x+0.5)
#define STEP 3
#define RATIO 0.6

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

void BST(int x1, int y1, int x2, int y2, int rt, int n)
{
	if (n == 0)
		return;

	if (rt == 0)
	{
		// 1.축소점 계산
		int xr = x1 + (1 - RATIO) * (x2 - x1);
		int yr = y1 + (1 - RATIO) * (y2 - y1);

		// 2. 회전점 계산
		// Right Point

		int Rx = (xr - x2) * cos(M_PI / 180 * (180 + 45)) - (yr - y2) * sin(M_PI / 180 * (180 + 45)) + x2;
		int Ry = (xr - x2) * sin(M_PI / 180 * (180 + 45)) + (yr - y2) * cos(M_PI / 180 * (180 + 45)) + y2;
		lineDDA(x2, y2, Rx, Ry, 0, 0, 0, 0);
		BST(x2, y2, Rx, Ry, rt, n - 1);

		// Left Point
		int Lx = (xr - x2) * cos(M_PI / 180 * (180 - 45)) - (yr - y2) * sin(M_PI / 180 * (180 - 45)) + x2;
		int Ly = (xr - x2) * sin(M_PI / 180 * (180 - 45)) + (yr - y2) * cos(M_PI / 180 * (180 - 45)) + y2;
		lineDDA(x2, y2, Lx, Ly, 0, 0, 0, 0);
		BST(x2, y2, Lx, Ly, rt, n - 1);
	}

	else
	{


		float changedRatio = (rand() % 101 - 50) / 500;
		float changedRatio2 = (rand() % 101 - 50) / 500;
		int randomAngle = (rand() % 80 - 40);

		// 1.축소점 계산
		int xr = x1 + (1 - RATIO + changedRatio) * (x2 - x1);
		int yr = y1 + (1 - RATIO + changedRatio2) * (y2 - y1);

		// 2. 회전점 계산
		// Right Point
	//	srand((unsigned int)time(NULL));
		int Rx = (xr - x2) * cos(M_PI / 180 * (180 + 45 + randomAngle)) - (yr - y2) * sin(M_PI / 180 * (180 + 45 + randomAngle)) + x2;
		int Ry = (xr - x2) * sin(M_PI / 180 * (180 + 45 + randomAngle)) + (yr - y2) * cos(M_PI / 180 * (180 + 45 + randomAngle)) + y2;
		lineDDA(x2, y2, Rx, Ry, 0, 0, 0, 0);
		BST(x2, y2, Rx, Ry, rt, n - 1);

		// Left Point
	//	srand((unsigned int)time(NULL));
		int Lx = (xr - x2) * cos(M_PI / 180 * (180 - 45 + randomAngle)) - (yr - y2) * sin(M_PI / 180 * (180 - 45 + randomAngle)) + x2;
		int Ly = (xr - x2) * sin(M_PI / 180 * (180 - 45 + randomAngle)) + (yr - y2) * cos(M_PI / 180 * (180 - 45 + randomAngle)) + y2;
		lineDDA(x2, y2, Lx, Ly, 0, 0, 0, 0);
		BST(x2, y2, Lx, Ly, rt, n - 1);

	}
}

int main(void)
{
	init_pixel();	// oImg 흰바탕으로 설정
	srand((unsigned int)time(NULL));

	lineDDA(500, 900, 500, 600, 0, 0, 0, 0);
	BST(500, 900, 500, 600, 0, 10);
	makePPM("Binarytree(normal).ppm");

	init_pixel();
	lineDDA(500, 900, 500, 600, 0, 0, 0, 0);
	BST(500, 900, 500, 600, 1, 10);
	makePPM("Binarytree(random).ppm");
	return 0;
}