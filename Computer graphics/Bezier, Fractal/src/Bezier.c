#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define W 600
#define H 600
#define M_PI 3.1415
#define ROUND(x) (int)(x+0.5)
#define STEP 100

unsigned char oImg[H][W][3];

enum kindOfLine { solid, dashed, dotted };
int lpalette[3][8] = { {1,1,1,1,1,1,1,1}, {1,1,1,1,0,0,0,0},{1,0,1,0,1,0,1,0} };

struct point {
	double xp;
	double yp;
};

struct point drawPoint[101];

int factorial(int n)
{
	if (n == 0 || n == 1)
		return 1;

	return n * factorial(n - 1);
}

int combination(int n, int r)
{
	return factorial(n) / factorial(n-r) / factorial(r);
}

void initPixel()
{
	memset(oImg, 255, sizeof(oImg));
}

void setPixel(int x, int y, int r, int g, int b)
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
			setPixel((int)(x + 0.5), (int)(y + 0.5), r, g, b);
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

	int numOfpoints = 0;

	printf("제어점 개수 (범위 :[4,9]) : \n");
	scanf("%d", &numOfpoints);

	if (!(numOfpoints >= 4 && numOfpoints <= 9))
		return 0;

	struct point* inputPoint = (struct point*)malloc(sizeof(struct point) * numOfpoints);
	//struct point* drawPoint = (struct point*)malloc(sizeof(struct point) * STEP);
	int* com = (int*)malloc(sizeof(int) * numOfpoints);
	//memset(inputPoint, 0, sizeof(inputPoint));
	//memset(&drawPoint, 0, sizeof(drawPoint));
	memset(com, 0, sizeof(com));

	for (int i = 0; i < numOfpoints; i++)
	{
		printf("px[%d] py[%d] 입력 [0,1] : ", i, i);
		scanf("%lf %lf", &(inputPoint[i].xp), &(inputPoint[i].yp));
	}

	for (int i = 0; i < numOfpoints; i++)
	{
		printf("px[%d] : %lf , py[%d]: %lf \n", i, inputPoint[i].xp, i, inputPoint[i].yp);
	}

	for (int i = 0; i < numOfpoints ; i++)
		com[i] = combination(numOfpoints - 1, i);

	initPixel();

	double u = 0;
	
	/**/
	for (int i = 0; i < STEP; i++)
	{
		u = (double)i / STEP;

		printf(" drawPoint[%d] : %lf\n", i, drawPoint[i].xp);
		for (int j = 0; j < numOfpoints; j++)
		{
			drawPoint[i].xp +=  inputPoint[j].xp * com[j] * (double)pow(u, j) * (double)pow(1 - u, numOfpoints - 1 - j);
			drawPoint[i].yp +=  inputPoint[j].yp * com[j] * (double)pow(u, j) * (double)pow(1 - u, numOfpoints - 1 - j);
		//	printf("aaa : %lf\n", inputPoint[j].xp * com[j] * (double)pow(u, j) * (double)pow(1 - u, numOfpoints - 1 - j));
		//	printf(" inputPoint[%d].xp : %lf\n", j, inputPoint[j].xp);
		//	printf("Combination((numOfpoints - 1) : %d\n", Combination((numOfpoints - 1), j));
		//	printf("pow(u, j) : %lf\n", pow(u, j));
		//	printf("pow(1 - u, numOfpoints - 1) : %lf\n", pow(1 - u, numOfpoints - 1 - j));
	//	printf("drawPoint[%d].xp : %lf \ndrawPoint[%d].yp : %lf\n\n", i, drawPoint[i].xp, i, drawPoint[i].yp);
		}
		//printf("drawPoint[%d].xp : %lf \ndrawPoint[%d].yp : %lf\n\n", i, drawPoint[i].xp, i, drawPoint[i].yp);
		drawPoint[i].xp *= W;
		drawPoint[i].yp *= H;
		printf("drawPoint[%d].xp : %d \ndrawPoint[%d].yp : %d\n\n", i, (int)drawPoint[i].xp, i, (int)drawPoint[i].yp);
		if (drawPoint[i].xp < 0) drawPoint[i].xp = 0;
		if (drawPoint[i].xp > W - 1) drawPoint[i].xp = W - 1;
		if (drawPoint[i].yp < 0) drawPoint[i].yp = 0;
		if (drawPoint[i].yp > H - 1) drawPoint[i].yp = H - 1;

		if (i != 0){
			lineDDA((int)drawPoint[i- 1].xp, (int)drawPoint[i -1].yp, (int)drawPoint[i].xp, (int)drawPoint[i].yp, 0, 0, 0, 0);
			}
		}

	makePPM("BezierSpline.ppm");



	return 0;
}