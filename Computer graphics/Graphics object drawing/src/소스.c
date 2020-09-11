#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#define W 680
#define H 480
#define M_PI 3.1415
#define ROUND(x) (int)(x+0.5) 

unsigned char oImg[480][640][3];
unsigned char rImg[480][640][3];

enum kindOfLine { solid, dashed, dotted };
int lpalette[3][8] = { {1,1,1,1,1,1,1,1}, {1,1,1,1,0,0,0,0},{1,0,1,0,1,0,1,0} };

void makePPM(char* filename)
{
	FILE* fp;
	fp = fopen(filename, "wb");
	fprintf(fp, "P6\n"); //magic no.: ASCII header and Binary body.. 
	fprintf(fp, "640 480\n"); //file size 
	fprintf(fp, "255\n"); //max level 
	fwrite(oImg, sizeof(char), 640 * 480 * 3, fp);

	fclose(fp);
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
	float x, y, incx, incy;
	dx = x2 - x1, dy = y2 - y1;
	x = (float)x1, y = (float)y1;

	if (abs(dx) > abs(dy)) step = abs(dx);
	else step = abs(dy);

	incx = dx / (float)step;
	incy = dy / (float)step;

	for (k = 0; k < step; k++)
	{
		x += incx;
		y += incy;

		if (lpalette[kind][k % 8] == 1) {
			setPixel((int)(x + 0.5), (int)(y + 0.5), r, g, b);
		}

	}
}

void lineBres(int x1, int y1, int x2, int y2, int r, int g, int b, int kind)
{
	int dx = abs(x2 - x1), dy = abs(y2 - y1);
	int p, twoDy, twoDyDx;
	int x, y, xEnd, IorD;

	int twoDx, twoDxDy;
	int yEnd;

	if (dx > dy) {
		p = 2 * dy - dx; twoDy = 2 * dy; twoDyDx = 2 * (dy - dx);

		if (x1 > x2) { x = x2; y = y2; xEnd = x1; if (y1 - y2 > 0)IorD = 1; else IorD = -1; }

		else { x = x1; y = y1; xEnd = x2; if (y2 - y1 > 0)IorD = 1; else IorD = -1; }

		oImg[x][y][0] = r; oImg[x][y][1] = g; oImg[x][y][2] = b;// start point marking with a user-defined color(r,g,b) 

		while (x < xEnd) {
			x++;
			if (p < 0) p += twoDy;
			else { y += IorD; p += twoDyDx; }
			if (lpalette[kind][y % 8] == 1)
				setPixel(x, y, r, g, b);
		}// end of while
	}// end of if
	else {
		p = 2 * dx - dy; twoDx = 2 * dx; twoDxDy = 2 * (dx - dy);

		if (y1 > y2) {
			y = y2; x = x2; yEnd = y1;
			if (x1 - x2 > 0)
				IorD = 1;
			else
				IorD = -1;
		}
		else {
			y = y1; yEnd = y2; x = x1;
			if (x2 - x1 > 0)
				IorD = 1;
			else
				IorD = -1;
		}

		oImg[y][x][0] = r; oImg[y][x][1] = g; oImg[y][x][2] = b;

		while (y < yEnd) {
			y++;
			if (p < 0) p += twoDx;
			else {
				x += IorD; p += twoDxDy;
			}
			if (lpalette[kind][y % 8] == 1)
				setPixel(x, y, r, g, b);
		}
	}// end of else

}

void polarCircle(int xr, int yr, int radius, int r, int g, int b)
{
	int theta = 0;
	double x, y = 0;
	double rad = 0;
	for (theta = 0; theta < 360; theta++) {
		rad = theta * 3.14 / 180.0;
		x = radius * cos(rad) + xr;
		y = radius * sin(rad) + yr;
		setPixel((int)(x + 0.5), (int)(y + 0.5), r, g, b);
	}

}

void circlePlot(int Cx, int Cy, int x, int y, int r, int g, int b)
{
	setPixel(Cx + x, Cy + y, r, g, b);//marking function according to the output device
	setPixel(Cx - x, Cy + y, r, g, b);
	setPixel(Cx + x, Cy - y, r, g, b);
	setPixel(Cx - x, Cy - y, r, g, b);
	setPixel(Cx + y, Cy + x, r, g, b);
	setPixel(Cx - y, Cy + x, r, g, b);
	setPixel(Cx + y, Cy - x, r, g, b);
	setPixel(Cx - y, Cy - x, r, g, b);
}

void circleMidpoint(int xCenter, int yCenter, int radius, int r, int g, int b)
{
	int x = 0, y = radius, p = 1 - radius;

	circlePlot(xCenter, yCenter, x, y, r, g, b);//start pixel marking..

	while (x <= y) {
		x++;
		if (p < 0) p += 2 * x + 1;
		else { y--; p += 2 * (x - y) + 1; }

		circlePlot(xCenter, yCenter, x, y, r, g, b);//marking..
	}
}

void ellipsePlot(int Cx, int Cy, int x, int y, int r, int g, int b)
{
	setPixel(Cx + x, Cy + y, r, g, b);//marking function according to the output device
	setPixel(Cx - x, Cy + y, r, g, b);
	setPixel(Cx + x, Cy - y, r, g, b);
	setPixel(Cx - x, Cy - y, r, g, b);
}

void ellipseRotation(int xCenter, int yCenter, int angle, int r, int g, int b)
{
	int transX, transY;
	double theta = angle * M_PI / 180; //

	for (int y = 0; y < H; y++)
		for (int x = 0; x < W; x++)
		{
			if (!(oImg[y][x][0] == 255 && oImg[y][x][1] == 255 && oImg[y][x][2] == 255))
			{
				// 타원의 좌표를 원점(0,0)기준으로 변환
				transX = x - xCenter;
				transY = y - yCenter;

				rImg[(int)(transX * sin(theta) + transY * cos(theta)) + yCenter][(int)(transX * cos(theta) - transY * sin(theta)) + xCenter][0] = r;
				rImg[(int)(transX * sin(theta) + transY * cos(theta)) + yCenter][(int)(transX * cos(theta) - transY * sin(theta)) + xCenter][1] = g;
				rImg[(int)(transX * sin(theta) + transY * cos(theta)) + yCenter][(int)(transX * cos(theta) - transY * sin(theta)) + xCenter][2] = b;


			}
		}
}

void ellipseMidpoint(int xCenter, int yCenter, int Rx, int Ry, int r, int g, int b, int angle)
{
	int Rx2 = Rx * Rx, Ry2 = Ry * Ry, twoRx2 = 2 * Rx2, twoRy2 = 2 * Ry2;
	int p, x = 0, y = Ry, px = 0, py = twoRx2 * y;

	//int transX, transY;

	ellipsePlot(xCenter, yCenter, x, y, r, g, b);//start pixel marking..

	//Region 1
	p = ROUND(Ry2 - (Rx2*Ry) + (0.25*Rx2));

	while (px < py) {
		x++;
		px += twoRy2;
		if (p < 0) p += Ry2 + px;
		else { y--; py -= twoRx2; p += Ry2 + px - py; }

		ellipsePlot(xCenter, yCenter, x, y, r, g, b);//marking..
	}

	//Region 2
	p = ROUND(Ry2*(x + 0.5)*(x + 0.5) + Rx2 * (y - 1)*(y - 1) - Rx2 * Ry2);
	while (y > 0) {
		y--;
		py -= twoRx2;
		if (p > 0) p += Rx2 - py;
		else { x++; px += twoRy2; p += Rx2 - py + px; }

		ellipsePlot(xCenter, yCenter, x, y, r, g, b);//marking..

	}
	//Sleep(1);
	ellipseRotation(xCenter, yCenter, angle, r, g, b);
}


int main(void)
{
	int x1, y1, x2, y2, lines = 0;
	int xCenter, yCenter, Rx, Ry, radius, angle;
	int input = 0;
	int r, g, b;

	memset(oImg, 255, sizeof(oImg));
	memset(rImg, 255, sizeof(rImg));

	printf("파일 크기 : %d x %d\n", W, H);

	while (1)
	{
		printf("1. DDA 선 2. Bresenham 선 3. 원 4. 타원(회전)\n");
		scanf("%d", &input);

		switch (input) {

		case 1: {
			printf("DDA 기법으로 선그리기 (종료 : q)\n");


			printf("입력 : 시작점x 끝점x 시작점y 끝점y 선색(R G B) 선 유형(실선:0 파선:1 점선:2)\n");
			scanf("%d %d %d %d %d %d %d %d", &x1, &x2, &y1, &y2, &r, &g, &b, &lines);

			clock_t start = clock();
			lineDDA(x1, y1, x2, y2, r, g, b, lines);
			printf("%0.5f\n", (float)clock() - start / CLOCKS_PER_SEC);

			makePPM("LineDDA.ppm");

			break;
		}

		case 2: {
			//memset(oImg, 255, sizeof(oImg));
			printf("Bresenham 기법으로 선그리기 (종료 : q)\n");

			printf("입력 : 시작점x 끝점x 시작점y 끝점y 선색(R G B) 선 유형(실선:0 파선:1 점선:2)\n");
			scanf("%d %d %d %d %d %d %d %d", &x1, &x2, &y1, &y2, &r, &g, &b, &lines);


			clock_t start2 = clock();
			lineBres(x1, y1, x2, y2, r, g, b, lines);
			printf("%0.5f\n", (float)clock() - start2 / CLOCKS_PER_SEC);

			makePPM("LineBresenham.ppm");

			break;
		}
				/*memset(oImg, 255, sizeof(oImg));
				polarCircle(200, 200, 100, 0);
				makePPM("polarCircle.ppm");*/
				//polarCircle(100, 100, 100, 0);

		case 3: {
			//memset(oImg, 255, sizeof(oImg));
			printf("중간점 기법으로 원 그리기 (종료 : q)\n");
			printf("입력 : 중심점x 중심점y 반지름 원색(R G B)\n");
			scanf("%d %d %d %d %d %d", &xCenter, &yCenter, &radius, &r, &g, &b);


			circleMidpoint(xCenter, yCenter, radius, r, g, b);

			makePPM("Circle.ppm");

			break;
		}

		case 4: {
			memset(oImg, 255, sizeof(oImg));
			//memset(rImg, 255, sizeof(rImg));
			printf("중간점 기법으로 타원 그리기 (종료 : q)\n");

			printf("입력 : 중심점x 중심점y x축반지름 y축반지름 타원색(R G B) 회전각도\n");
			scanf("%d %d %d %d %d %d %d %d", &xCenter, &yCenter, &Rx, &Ry, &r, &g, &b, &angle);

			ellipseMidpoint(xCenter, yCenter, Rx, Ry, r, g, b, angle);
			//makePPM("ellipse.ppm");

			FILE* fp;
			fp = fopen("ellipseRotate.ppm", "wb");
			fprintf(fp, "P6\n"); //magic no.: ASCII header and Binary body.. 
			fprintf(fp, "640 480\n"); //file size 
			fprintf(fp, "255\n"); //max level 
			fwrite(rImg, sizeof(char), 640 * 480 * 3, fp);
			fclose(fp);

			break;
		}

		default:
			break;
		}
	}
	return 0;
}
