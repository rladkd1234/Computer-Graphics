#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <gl/glut.h>
#include <math.h>
#include <string.h>
#define MAX_V 40000
#define MAX_F 80000

FILE* fp;

struct Vertex {
	float x, y, z; //x,y,z coordinates
};

struct Tri {
	long vertIDs[3]; //edge information
};

struct Vector {
	float normal[3]; //normal information
};

struct Mesh {
	Vertex verts[MAX_V];
	Tri faces[MAX_F];
	Vector Tnorm[MAX_V];
	Vector Fnorm[MAX_F];
};

long nVerts, nFaces;
// for gluLookAt parameters
float cameraPos[3]; //camera position
float cameraLookAt[3]; //camera Look at
float cameraUpvector[3] = { 0.0f, 1.0f, 0.0f }; // upvector
float lightPos[3];

float surfaceColor[3] = { 0.8f, 0.8f, 0.0f }; //gray
float g_x_rot, g_y_rot;
float z_near, z_far;
float dx, dy, dz; // for zooming

int tangentNormalFlag = 0; // Face : 0 , Tangent : 1
int wireFrameFlag = 0;

Mesh Data;

void init(void);
void calFaceNormal(long v1, long v2, long v3, float* norm);
void Display(void);
void Reshape(int w, int h);
void Key(int key, int x, int y);
void SetupRender(void);

void main(void)
{
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_SINGLE);
	glutCreateWindow("Bunny");
	init();
	SetupRender();

	glutReshapeFunc(Reshape);
	glutSpecialFunc(Key);
	glutDisplayFunc(Display);

	glutMainLoop();
}

void init(void)
{
	int i, j;
	float xMin = 1000., xMax = -1000.;
	float yMin = 1000., yMax = -1000.;
	float zMin = 1000., zMax = -1000.;
	float xCenter, yCenter, zCenter;
	char wideAxis;

	fp = fopen("bunny1.txt", "r");
	fscanf(fp, "%d %d\n", &nVerts, &nFaces);

	for (i = 0; i < nVerts; i++)
		fscanf(fp, "%f %f %f", &Data.verts[i].x, &Data.verts[i].y, &Data.verts[i].z);

	for (i = 0; i < nFaces; i++)
		for (j = 0; j < 3; j++)
			fscanf(fp, "%d", &Data.faces[i].vertIDs[j]);
	fclose(fp);

	// find lookat parameters
	// max and min coordinates

	for (i = 0; i < nVerts; i++)
	{
		if (xMin > Data.verts[i].x) xMin = Data.verts[i].x;
		if (yMin > Data.verts[i].y) yMin = Data.verts[i].y;
		if (zMin > Data.verts[i].z) zMin = Data.verts[i].z;
		if (xMax < Data.verts[i].x) xMax = Data.verts[i].x;
		if (yMax < Data.verts[i].y) yMax = Data.verts[i].y;
		if (zMax < Data.verts[i].z) zMax = Data.verts[i].z;
	}

	xCenter = (xMax + xMin) / 2.;
	yCenter = (yMax + yMin) / 2.;
	zCenter = (zMax + zMin) / 2.;

	// find the wide axis
	if ((fabs(xMax - xMin) >= fabs(yMax - yMin)) && (fabs(xMax - xMin) >= fabs(zMax - zMin))) wideAxis = 'X';
	else if ((fabs(yMax - yMin) > fabs(xMax - xMin)) && (fabs(yMax - yMin) > fabs(zMax - zMin))) wideAxis = 'Y';
	else if ((fabs(zMax - zMin) > fabs(yMax - yMin)) && (fabs(zMax - zMin) > fabs(xMax - xMin))) wideAxis = 'Z';

	// determine LookAt parameter
	if (wideAxis == 'X') {
		cameraPos[0] = xCenter; //camera position
		cameraPos[1] = yCenter;
		cameraPos[2] = zMax + fabs(xMax - xMin);
		lightPos[0] = cameraPos[0]; //light position
		lightPos[1] = cameraPos[1];
		lightPos[2] = cameraPos[2];
		cameraLookAt[0] = xCenter; //camera LookAt
		cameraLookAt[1] = yCenter;
		cameraLookAt[2] = zCenter;
		z_near = 0.5 * fabs(xMax - xMin);
		z_far = 10. * z_near;
	}

	if (wideAxis == 'Y') {
		cameraPos[0] = xCenter; //camera position
		cameraPos[1] = yCenter;
		cameraPos[2] = zMax + fabs(yMax - yMin);
		lightPos[0] = cameraPos[0]; //light position
		lightPos[1] = cameraPos[1];
		lightPos[2] = cameraPos[2];
		cameraLookAt[0] = xCenter; //camera LookAt
		cameraLookAt[1] = yCenter;
		cameraLookAt[2] = zCenter;
		z_near = 0.5 * fabs(yMax - yMin);
		z_far = 10. * z_near;
	}

	if (wideAxis == 'Z') {
		cameraPos[0] = xMax + fabs(zMax - zMin); //camera position
		cameraPos[1] = yCenter;
		cameraPos[2] = zCenter;
		lightPos[0] = cameraPos[0]; //light position
		lightPos[1] = cameraPos[1];
		lightPos[2] = cameraPos[2];
		cameraLookAt[0] = xCenter; //camera LookAt
		cameraLookAt[1] = yCenter;
		cameraLookAt[2] = zCenter;
		z_near = 0.5 * fabs(zMax - zMin);
		z_far = 10. * z_near;
	}

	// Since the center is shifted to the origin..
	dx = (cameraPos[0] - xCenter) / 20.;
	dy = (cameraPos[1] - yCenter) / 20.;
	dz = (cameraPos[2] - zCenter) / 20.;

	//cal face normal(Bunny1.txt에서 Tri index 1부터 시작)
	for (long faceIDs = 0; faceIDs < nFaces; faceIDs++)
	{
		calFaceNormal(Data.faces[faceIDs].vertIDs[0] - 1,
			Data.faces[faceIDs].vertIDs[1] - 1,
			Data.faces[faceIDs].vertIDs[2] - 1,
			Data.Fnorm[faceIDs].normal);
	}


	//vertex normal 계산
	//printf("Data.Tnorm : %d", sizeof(Data.Tnorm));
	//memset(&Data.Tnorm, 1.0, sizeof(Data.Tnorm));

	//for (int vertIDs = 0; vertIDs < nVerts; vertIDs++)
	//{
	//	for (i = 0; i < 3; i++)
	//	{
	//	Data.Tnorm[vertIDs].normal[i] = 0;
	//	printf("%lf", Data.Tnorm[vertIDs].normal[i]);
	//	}
	//	printf("\n");
	//}

	//cal Tangent Face
	float len, norm[3];

	for (long faceIDs = 0; faceIDs < nFaces; faceIDs++)
	{
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				Data.Tnorm[Data.faces[faceIDs].vertIDs[i]].normal[j] += Data.Fnorm[faceIDs].normal[j];
			}

			norm[0] = Data.Tnorm[Data.faces[faceIDs].vertIDs[i]].normal[0];
			norm[1] = Data.Tnorm[Data.faces[faceIDs].vertIDs[i]].normal[1];
			norm[2] = Data.Tnorm[Data.faces[faceIDs].vertIDs[i]].normal[2];

			len = (float)sqrt(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);

			if (len == 0.)
			{
				len = 1.;
			}

			Data.Tnorm[Data.faces[faceIDs].vertIDs[i]].normal[0] /= len;
			Data.Tnorm[Data.faces[faceIDs].vertIDs[i]].normal[1] /= len;
			Data.Tnorm[Data.faces[faceIDs].vertIDs[i]].normal[2] /= len;
		}
	}

}

void calFaceNormal(long v1, long v2, long v3, float* norm)
//important in shading
{
	float e1[3], e2[3];
	e1[0] = Data.verts[v2].x - Data.verts[v1].x;
	e1[1] = Data.verts[v2].y - Data.verts[v1].y;
	e1[2] = Data.verts[v2].z - Data.verts[v1].z;

	e2[0] = Data.verts[v3].x - Data.verts[v1].x;
	e2[1] = Data.verts[v3].y - Data.verts[v1].y;
	e2[2] = Data.verts[v3].z - Data.verts[v1].z;

	// cross product e1 and e2
	norm[0] = e1[1] * e2[2] - e1[2] * e2[1];
	norm[1] = e1[2] * e2[0] - e1[0] * e2[2];
	norm[2] = e1[0] * e2[1] - e1[1] * e2[0];

	//normalization
	float len = (float)sqrt(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);

	if (len == 0.) len = 1.;

	norm[0] /= len;
	norm[1] /= len;
	norm[2] /= len;
}

void Display(void)
{
	float normal[3];

	glClearColor(0.0, 0.0, 1.0, 0.0); // set background color -> blue
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear and paint background

	glColor3f(surfaceColor[0], surfaceColor[1], surfaceColor[2]); //set surface color

	//model view matrix setting
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//view matrix setting
	gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], //camera position
		cameraLookAt[0], cameraLookAt[1], cameraLookAt[2], //camera look at
		cameraUpvector[0], cameraUpvector[1], cameraUpvector[2]); //camera upvector

	glRotatef(g_x_rot, 1.0f, 0.0f, 0.0f); //controlled by key
	glRotatef(g_y_rot, 0.0f, 1.0f, 0.0f);


	glBegin(GL_TRIANGLES);

	// Vertex verts[]
	// Tri	  faces[]
	// Vector Tnorm[]
	// Vector Fnorm[]	//

	for (long faceIDs = 0; faceIDs < nFaces; faceIDs++)
	{
		if (!tangentNormalFlag)
		{
			glNormal3f(Data.Fnorm[faceIDs].normal[0], Data.Fnorm[faceIDs].normal[1], Data.Fnorm[faceIDs].normal[2]);
		}

		for (int p = 0; p < 3; p++)
		{
			if (tangentNormalFlag)
			{
				glNormal3f(Data.Tnorm[Data.faces[faceIDs].vertIDs[p]].normal[0],
					Data.Tnorm[Data.faces[faceIDs].vertIDs[p]].normal[1],
					Data.Tnorm[Data.faces[faceIDs].vertIDs[p]].normal[2]);	// glNoram(Tnorms(v))
			}

			glVertex3f(Data.verts[Data.faces[faceIDs].vertIDs[p] - 1].x,
				Data.verts[Data.faces[faceIDs].vertIDs[p] - 1].y,
				Data.verts[Data.faces[faceIDs].vertIDs[p] - 1].z);	// glVertax(v)
		}
	}

	glEnd();


	if (wireFrameFlag == 1) {

		glColor3f(1.0f, 0.0f, 0.0f);

		glBegin(GL_LINES);
		for (long faceIDs = 0; faceIDs < nFaces; faceIDs++) {

			glVertex3f(Data.verts[Data.faces[faceIDs].vertIDs[0] - 1].x,
				Data.verts[Data.faces[faceIDs].vertIDs[0] - 1].y,
				Data.verts[Data.faces[faceIDs].vertIDs[0] - 1].z);

			glVertex3f(Data.verts[Data.faces[faceIDs].vertIDs[1] - 1].x,
				Data.verts[Data.faces[faceIDs].vertIDs[1] - 1].y,
				Data.verts[Data.faces[faceIDs].vertIDs[1] - 1].z);

			glVertex3f(Data.verts[Data.faces[faceIDs].vertIDs[1] - 1].x,
				Data.verts[Data.faces[faceIDs].vertIDs[1] - 1].y,
				Data.verts[Data.faces[faceIDs].vertIDs[1] - 1].z);

			glVertex3f(Data.verts[Data.faces[faceIDs].vertIDs[2] - 1].x,
				Data.verts[Data.faces[faceIDs].vertIDs[2] - 1].y,
				Data.verts[Data.faces[faceIDs].vertIDs[2] - 1].z);

			glVertex3f(Data.verts[Data.faces[faceIDs].vertIDs[2] - 1].x,
				Data.verts[Data.faces[faceIDs].vertIDs[2] - 1].y,
				Data.verts[Data.faces[faceIDs].vertIDs[2] - 1].z);

			glVertex3f(Data.verts[Data.faces[faceIDs].vertIDs[0] - 1].x,
				Data.verts[Data.faces[faceIDs].vertIDs[0] - 1].y,
				Data.verts[Data.faces[faceIDs].vertIDs[0] - 1].z);
		}

		glEnd();
	}

	glFlush();
}

void Reshape(int w, int h)
{
	float fov_y, aspect;

	if (h == 0) h = 1.;
	aspect = (float)w / (float)h;
	fov_y = 60.0f;

	glViewport(0, 0, w, h);

	//projection matrix setting 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//mode of projection
	//glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 100.0); //Orthogonal projection
	gluPerspective(fov_y, aspect, z_near, z_far);

}

void Key(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		g_x_rot -= 5.0f;
	if (key == GLUT_KEY_DOWN)
		g_x_rot += 5.0f;
	if (key == GLUT_KEY_LEFT)
		g_y_rot -= 5.0f;
	if (key == GLUT_KEY_RIGHT)
		g_y_rot += 5.0f;
	if (key > 365.0f)
		g_x_rot = 0.0f;
	if (key < 0.0f)
		g_x_rot = 365.0f;
	if (key > 365.0f)
		g_y_rot = 0.0f;
	if (key < 0.0f)
		g_y_rot = 365.0f;

	// Zoom key
	if (key == GLUT_KEY_F1) {
		cameraPos[0] -= dx;
		cameraPos[1] -= dy;
		cameraPos[2] -= dz;
	}
	if (key == GLUT_KEY_F2) {
		cameraPos[0] += dx;
		cameraPos[1] += dy;
		cameraPos[2] += dz;
	}

	if (key == GLUT_KEY_F3) {	//face normal, vertex normal 스위치
		if (tangentNormalFlag) tangentNormalFlag = 0;
		else tangentNormalFlag = 1;
	}

	if (key == GLUT_KEY_F4) {	//wireframe 모드 스위치
		if (wireFrameFlag) wireFrameFlag = 0;
		else wireFrameFlag = 1;
	}

	//refresh window
	glutPostRedisplay();
}

void SetupRender(void)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); // z-buffer check, front or back test(should be included in Line+Face..)
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	// setting material
	GLfloat mat_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 40);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	// setting light source
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
}