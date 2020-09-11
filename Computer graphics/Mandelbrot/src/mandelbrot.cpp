#define _CRT_SECURE_NO_WARNINGS
#include <gl/glut.h>
#include <math.h>
#include <stdio.h>
#include <windows.h>

GLint TopLeftX, TopLeftY, BottomRightX, BottomRightY;

#define H 500.0
#define W 500.0
#define ITER 100

typedef double complex[2]; // Complex 
unsigned char oImg[500][500];

double h = 2.0;
double w = 2.0;
double cx = -0.5;
double cy = 0;

int mandelbrot_flag = 1;
int mouse = 0;

void add(complex a, complex b, complex p)
{
	p[0] = a[0] + b[0];
	p[1] = a[1] + b[1];
}

void multply(complex a, complex b, complex p)
{
	p[0] = a[0] * b[0] - a[1] * b[1];
	p[1] = a[0] * b[1] + a[1] * b[0];
}

void form(double a, double b, complex p)
{
	p[0] = a;
	p[1] = b;
}

double mag2(complex p)
{
	return p[0] * p[0] + p[1] * p[1];
}

void mandelbrot()
{
	double x = 0;
	double y = 0;
	double v;

	complex c, z, z2;

	glBegin(GL_POINTS);
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			x = (j * w / (W - 1)) - (w / 2.0) + cx;
			y = (i * h / (H - 1)) - (h / 2.0) + cy;

			form(0, 0, z);
			form(x, y, c);

			for (int k = 0; k < ITER; k++)
			{
				multply(z, z, z2);
				add(z2, c, z);
				v = mag2(z);

				if (v > 4.) break;
			}

			if (v > 1.) v = 1.;
			v *= 255;
			glColor3ub(v, v, v);
			glVertex3d((double)j * 2 / W, (double)i * 2 / H, 0.0);
			//glVertex3d((double)j/(W/2),(double)i/(H/2), 0.0);
			oImg[i][j] = v;

		}
	}
	glEnd();
}

void MyDisplay()
{
	//glViewport(0, 0, 500, 500);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.0, -1.0, 0.0);
	if (mandelbrot_flag == 1) mandelbrot();
	glFlush();
	//glutSwapBuffers();
}

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y)
{
	double tempcx = 0, tempcy = 0;
	double squareLength = 0;
	double temp = 0;

	if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN)
	{
		TopLeftX = X;
		TopLeftY = Y;
		mandelbrot_flag = 0;
	}

	//GLint TopLeftX, TopLeftY, BottomRightX, BottomRightY;
	if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP && mouse == 1) {

		BottomRightX = X;
		BottomRightY = Y;

		//큰 면이 정사각형 길이
		if (abs(BottomRightX - TopLeftX) > abs(BottomRightY - TopLeftY)) squareLength = (BottomRightX - TopLeftX);
		else squareLength = BottomRightY - TopLeftY;


		// 비례식 H : squareLength = h : ??
		temp = h * (squareLength / H);
		printf("squareLength : %lf\n", squareLength);
	
		//x = (j * w / (W - 1)) - (w / 2.0) + cx;
		//y = (i * h / (H - 1)) - (h / 2.0) + cy;

		//중점이동
		printf("temp : %lf, cx : %lf , cy : %lf\n", temp, cx, cy);
		printf("BottmRightx : %d TopLeftX : %d BottmRight Y : %d TopLeftY : %d \n", BottomRightX, TopLeftX, BottomRightY, TopLeftY);
		printf("w : %lf h : %lf\n", w, h);

	/*	tempcx = ((BottomRightX + TopLeftX) / 2) * w / W - (w / 2);
		tempcy = ((BottomRightY + TopLeftY) / 2) * h / H - (h / 2);
		cx = cx + tempcx;
		cy = cy - tempcy;
*/	

	/* 이게 맞는건가?
	printf("계산 값 : %lf, 계산값 : %lf \n",
		((BottomRightX + TopLeftX) / 2) * (w / W) - (w / 2), (((BottomRightY + TopLeftY) / 2) * (h / H) - (h / 2)));
		cx = cx + ((BottomRightX + TopLeftX) / 2.0 * w / W - w / 2.0);
		cy = cy - ((BottomRightY + TopLeftY) / 2.0 * h / H - h / 2.0);

		printf("temp : %lf, cx : %lf , cy : %lf\n", temp, cx, cy);
		h = w = temp;
*/
	
		cx = cx + ((BottomRightX + TopLeftX) / 2.0* (w / W) - (w / 2.0)); 
		cy = cy - ((BottomRightY + TopLeftY) / 2.0 * (h / H) - (h / 2.0));
		h = w = temp;
		printf("temp : %lf, cx : %lf , cy : %lf\n", temp, cx, cy);


		mandelbrot_flag = 1;
		mouse = 0;
		glutPostRedisplay();
	}

	if (Button == GLUT_RIGHT_BUTTON && State == GLUT_UP)
	{
		cx = -0.5;
		cy = 0;
		h = w = 2.0;

		mandelbrot_flag = 1;
		glutPostRedisplay();
	}

}

void MyMouseMove(GLint X, GLint Y)
{
	mouse = 1;
	double v;
	BottomRightX = X;
	BottomRightY = Y;

	glBegin(GL_POINTS);
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			v = (double)oImg[i][j];
			glColor3ub(v, v, v);
			glVertex3d((double)j * 2 / W, (double)i * 2 / H, 0.0);
		}
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0, 0., 0.);

	glVertex3f(TopLeftX * (2 / W), (500 - TopLeftY) * (2 / H), 0.0);
	glVertex3f(TopLeftX * (2 / W), (500 - BottomRightY) * (2 / H), 0.0);
	glVertex3f(BottomRightX * (2 / W), (500 - BottomRightY) * (2 / H), 0.0);
	glVertex3f(BottomRightX * (2 / W), (500 - TopLeftY) * (2 / H), 0.0);
	glEnd();
	glFlush();
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Mouse Callback");
	glutDisplayFunc(MyDisplay);
	glutMouseFunc(MyMouseClick);
	glutMotionFunc(MyMouseMove);

	glutMainLoop();
	return 0;
}