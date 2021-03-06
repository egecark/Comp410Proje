﻿//
//  Display a rotating cube 
//

#include "Angel.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <Windows.h>
#include <mmsystem.h>
#include "TetrisObjects.h"
#include <time.h>
#include <stdlib.h>
#include <algorithm>

GLuint buffer;
using namespace std;

typedef vec4  color4;
typedef vec4  point4;


mat4 model_view_floor;
mat4  projection;

GLuint program;
point4 light_position2(-1.0, 0.0, 0.0, 0.0);
float  material_shininess = 5;
float  material_shininess2 = 500;
bool isPoint = true; //checks if light source is point
bool isMetal = true; //checks if shininess is metal
bool isOrtho = true; //checks if perspective is orthographic
GLuint type = 1; //type of object
GLuint textureFlag = 0; //enable texture mapping
GLuint shading = 1; //illumination model (sorry for the improper naming)
int cubeNumber = 0;
int t = 900;

//shader lighting parameters
point4 light_position(-1.0, 0.0, 0.0, 1.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

color4 material_ambient(1.0, 1.0, 1.0, 1.0);
color4 material_diffuse(1.0, 1.0, 1.0, 1.0);
color4 material_specular(1.0, 1.0, 1.0, 1.0);


color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;

const int NumVertices = 85620;
int Index = 0;
int yGridPosition = 0;

point4 points[NumVertices];
color4 colors[NumVertices];
std::vector<point4> pointsList;
vec3   normals[NumVertices];
vec2   texture[NumVertices];
std::vector<point4> offVertices; //the vector that holds vertices loaded from the .off file
std::vector<mat4> model_views;
std::vector<vec3> shapeNormal; //stores the normals of the shapeX
std::vector<vec3> shapeQuad; //stores the faces of shapeX
std::vector<vec2> texCoords; //stores the texture coordinates of shapeX
std::vector<vec3> movePos;

std::vector<int> demolishedCubes;
// Vertices of a unit cube centered at origin, sides aligned with axes
std::vector<vec3> Thetas;
std::vector<GLfloat> xGrid;
std::vector<GLfloat> yGrid;
std::vector<GLfloat> zGrid;
int shape = 0;
int counterX = 0;
int counterZ = 0;
								 // Vertices of a unit cube centered at origin, sides aligned with axes

//GLfloat cubeLengthHalf = 0.3;
//GLfloat groundLenghtHalf = 10 * cubeLengthHalf;
//GLfloat cubeStartingPosY = cubeLengthHalf * 10 * 2;
//GLfloat groundPosY = -cubeStartingPosY;

int gameSpace[10][20][10];	//10x10 ground tiles containing the elevation as y position. Will be initialized in init


vec3 viewer_pos(cubeLengthHalf, 0.0, cubeLengthHalf);	//Bu deðiþmeli

GLvoid *font_style1 = GLUT_BITMAP_HELVETICA_18;
int ch1, ch2, ch3, ch4, ch5;
int score = 0;
int chY, chO, chU, chSpace, chL, chS, chE;
void updateScoreBoard(int);
void failureMessageDisplay();

point4 vertices[8] = {
	point4(-cubeLengthHalf * 2 + 0.6, cubeStartingPosY - 0.6,  0, 1.0),
	point4(-cubeLengthHalf * 2 + 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf,  0, 1.0),
	point4(0 + 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf,  0, 1.0),
	point4(0 + 0.6, cubeStartingPosY - 0.6,  0, 1.0),
	point4(-cubeLengthHalf * 2 + 0.6, cubeStartingPosY - 0.6, -cubeLengthHalf * 2, 1.0),
	point4(-cubeLengthHalf * 2 + 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf * 2, 1.0),
	point4(0 + 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf * 2, 1.0),
	point4(0 + 0.6, cubeStartingPosY - 0.6, -cubeLengthHalf * 2, 1.0)
};
point4 groundVertices[4] = {
	point4(-groundLenghtHalf, groundPosY, -groundLenghtHalf, 1.0),
	point4(groundLenghtHalf, groundPosY, -groundLenghtHalf, 1.0),
	point4(groundLenghtHalf, groundPosY, groundLenghtHalf, 1.0),
	point4(-groundLenghtHalf, groundPosY, groundLenghtHalf, 1.0)
};

color4 colorTetrahedron[4] = {
	color4(1.0, 0.0, 0.0, 1.0),
	color4(0.0, 1.0, 0.0, 1.0),
	color4(1.0, 1.0, 0.0, 1.0),
	color4(0.0, 0.0, 1.0, 1.0),
};
color4 main_colour = color4(1.0, 0.0, 0.0, 1.0);
color4 ground_colour = color4(1.0, 0.0, 1.0, 1.0);

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int groundHeight;
int  Axis = Yaxis;
bool expand = true;
bool wireframe = false;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat  Theta2[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat  Beta = 1.0;
GLfloat speed = 0.01;

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices

void
quad(int a, int b, int c, int d, point4 vertices[8], int color)
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];

	vec3 normal = normalize(cross(u, v));
	normals[Index] = normal; colors[Index] = colorTetrahedron[color]; pointsList.push_back(vertices[a]); Index++;
	normals[Index] = normal; colors[Index] = colorTetrahedron[color]; pointsList.push_back(vertices[b]); Index++;
	normals[Index] = normal; colors[Index] = colorTetrahedron[color]; pointsList.push_back(vertices[c]); Index++;
	normals[Index] = normal; colors[Index] = colorTetrahedron[color]; pointsList.push_back(vertices[a]); Index++;
	normals[Index] = normal; colors[Index] = colorTetrahedron[color]; pointsList.push_back(vertices[c]); Index++;
	normals[Index] = normal; colors[Index] = colorTetrahedron[color]; pointsList.push_back(vertices[d]); Index++;
}

void
floorQuad(int a, int b, int c, int d)
{
	colors[Index] = ground_colour; pointsList.push_back(groundVertices[a]); Index++;
	colors[Index] = ground_colour; pointsList.push_back(groundVertices[b]); Index++;
	colors[Index] = ground_colour; pointsList.push_back(groundVertices[c]); Index++;
	colors[Index] = ground_colour; pointsList.push_back(groundVertices[a]); Index++;
	colors[Index] = ground_colour; pointsList.push_back(groundVertices[c]); Index++;
	colors[Index] = ground_colour; pointsList.push_back(groundVertices[d]); Index++;
}

//----------------------------------------------------------------------------

void initGroundTiles()
{
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 20; y++) {
			for (int z = 0; z < 10; z++) {
				gameSpace[x][y][z] = 0;
			}
		}
	}
}

GLfloat viewerposToCoordinates(GLfloat viewer_posY)
{
	return 6.0 - viewer_posY;
}

void demolishRow()
{
	bool isEqual = true;
	for (int y = 0; y < 20; y++) {
		for (int x = 5; x < 9; x++) {
			for (int z = 4; z < 8; z++) {
				isEqual = isEqual && gameSpace[x][y][z] != 0;
			}
		}
		if (isEqual) {
			for (int x = 5; x < 9; x++) {
				for (int z = 4; z < 8; z++) {
					demolishedCubes.push_back(gameSpace[x][y][z]);
					gameSpace[x][y][z] = 0;
					for (int y2 = y+1; y2 < 20; y2++) {
						if (gameSpace[x][y2][z] != 0) {
							movePos[gameSpace[x][y2][z]].y += 0.6;
							yGrid[gameSpace[x][y2][z]] -= 1;
							gameSpace[x][y2 - 1][z] = gameSpace[x][y2][z];
							gameSpace[x][y2][z] = 0;
						}
					}
				}
			}
		}
	}
	//if all the entries in groundTileY are equal and not groundPosY, demolish row
	//row is all squares having the same resting position
	int pointsEarned = 0;
	updateScoreBoard(pointsEarned);

}

void populatePoints() {
	for (int i = 0; i < pointsList.size(); i++) {
		points[i] = pointsList[i];
	}
}


// generate 12 triangles: 36 vertices and 36 colors
void
colorcube(point4 v[8], int x, int y, int z, int color)
{
	quad(1, 0, 3, 2, v, color);
	quad(2, 3, 7, 6, v, color);
	quad(3, 0, 4, 7, v, color);
	quad(6, 5, 1, 2, v, color);
	quad(4, 5, 6, 7, v, color);
	quad(5, 4, 0, 1, v, color);
	float yOffset = 12 - 0.6*y;
	vec3 moveVector = (0.0, 0.0 + yOffset, 0.0);
	vec3 rotateVector = (0.0, 0.0, 0.0);
	movePos.push_back(moveVector);
	Thetas.push_back(rotateVector);
	xGrid.push_back(x);
	yGrid.push_back(y);
	zGrid.push_back(z);
	cubeNumber++;
	//movePos[cubeNumber - 1].y += yOffset;
	gameSpace[x][y][z] = cubeNumber;
	model_views.push_back(NULL);
}

void newBlock(point4 v[8], int x, int y, int z, int color) {
	colorcube(v, x, y, z, color);
	populatePoints();
}

void newStick() {
	newBlock(vertices, 5, 18, 5, 1);
	newBlock(vertices_pos1X, 4, 18, 5, 1);
	newBlock(vertices_neg1X, 6, 18, 5, 1);
	newBlock(vertices_neg2X, 7, 18, 5, 1);
	groundHeight = 19;
	shape = 1;
	counterX = 0;
	counterZ = 0;
}

void newLetterT() {
	newBlock(vertices, 5, 18, 5, 2);
	newBlock(vertices_pos1X, 4, 18, 5, 2);
	newBlock(vertices_neg1X, 6, 18, 5, 2);
	newBlock(vertices_pos1Y, 5, 19, 5, 2);
	groundHeight = 19;
	shape = 2;
	counterX = 0;
	counterZ = 0;
}

void newLetterS() {
	newBlock(vertices, 5, 18, 5, 3);
	newBlock(vertices_pos1X1Y, 4, 19, 5, 3);
	newBlock(vertices_pos1Y, 5, 19, 5, 3);
	newBlock(vertices_neg1X, 6, 18, 5, 3);
	groundHeight = 19;
	shape = 3;
	counterX = 0;
	counterZ = 0;
}

void newLetterZ() {
	newBlock(vertices, 5, 18, 5, 0);
	newBlock(vertices_neg1X1Y, 6, 17, 5, 0);
	newBlock(vertices_pos1Y, 5, 19, 5, 0);
	newBlock(vertices_neg1X, 6, 18, 5, 0);
	groundHeight = 18;
	shape = 4;
	counterX = 0;
	counterZ = 0;
}



void newLetterL() {
	newBlock(vertices, 5, 18, 5, 0);
	newBlock(vertices_pos1X1Y, 4, 19, 5, 0);
	newBlock(vertices_pos1X, 4, 18, 5, 0);
	newBlock(vertices_neg1X, 6, 18, 5, 0);
	groundHeight = 19;
	shape = 5;
	counterX = 0;
	counterZ = 0;
}

void createRandomObject() {
	srand(time(NULL));
	int r = rand() % ((5 - 1) + 1) + 1;
	//printf("%d\n", r);
	switch (r) {
	case 1:
		newStick();
		break;
	case 2:
		newLetterL();
		break;
	case 3:
		newLetterL();
		break;
	case 4:
		newLetterT();
		break;
	case 5:
		newLetterT();
		break;
	}
}

//----------------------------------------------------------------------------

void initFailureMessage();
void initScoreBoard();

// OpenGL initialization
void
init()
{
	floorQuad(1, 0, 3, 2);
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 20; y++) {
			for (int z = 0; z < 4; z++) {
				gameSpace[x][y][z] = 0;
			}
		}
	}
	newStick();

	ch1 = ch2 = ch3 = ch4 = ch5 = 48;
	chY = chO = chU = chSpace = chL = chS = chE = 0;

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object


	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals) + sizeof(texture), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), sizeof(texture), texture);


	// Load shaders and use the resulting shader program
	program = InitShader("vshader.glsl", "fshader.glsl");


	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors) + sizeof(normals)));

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
		1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, specular_product);
	if (isPoint)
		glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	else
		glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position2);
	if (isMetal)
		glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
	else
		glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess2);

	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");

	glUniform1i(glGetUniformLocation(program, "type"), type);
	glUniform1i(glGetUniformLocation(program, "TextureFlag"), textureFlag);
	glUniform1i(glGetUniformLocation(program, "shading"), shading);

	// Set current program object
	glUseProgram(program);

	if (isOrtho)
		// Set projection matrix
		projection = Ortho(-8.0, 8.0, -8.0, 8.0, -8.0, 8.0); // Ortho(): user-defined function in mat.h
															 //projection = Perspective( 45.0, 1.0, -1.0, 1.0 );
	else
		projection = Perspective(45.0, 1.0, 0.5, 6.0);
	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

	// Enable hiddden surface removal
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Set state variable "clear color" to clear buffer with.
	//glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------



void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//ScoreBoard and messages
	initScoreBoard();
	initFailureMessage();

	//failureMessageDisplay();

	/*
	int scoreTemp = score;
	int ch = scoreTemp % 10;
	scoreTemp = scoreTemp / 10;
	ch1 = 48 + ch;
	ch2 = 48 + ((scoreTemp) % 10);
	scoreTemp = scoreTemp / 10;
	ch3 = 48 + (scoreTemp) % 10;
	scoreTemp = scoreTemp / 10;
	ch4 = 48 + (scoreTemp) % 10;
	scoreTemp = scoreTemp / 10;
	ch5 = 48 + (scoreTemp) % 10;
	*/

	for (int i = 0; i < cubeNumber; i++) {
			model_views[cubeNumber - 1] = ((Translate(-movePos[i]) * //modelview of the object
				RotateX(Theta[Xaxis]) *
				RotateY(Theta[Yaxis]) *
				RotateZ(Theta[Zaxis]) *
				Scale(Beta, Beta, Beta)));
			//for (int y = 0; y < demolishedCubes.size(); y++) {
				//if (i != demolishedCubes[y]) {
			if (!(std::find(demolishedCubes.begin(), demolishedCubes.end(), i) != demolishedCubes.end())) {
				glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_views[cubeNumber - 1]);
				glDrawArrays(GL_TRIANGLES, 6 + 36 * i, 36);
			}
				//}
			//}
	}

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view_floor);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glutSwapBuffers();

}

//----------------------------------------------------------------------------

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);


	if (w <= h) {
		projection = Ortho(-8.0, 8.0, -8.0 * (GLfloat)h / (GLfloat)w,
			8.0 * (GLfloat)h / (GLfloat)w, -8.0, 8.0);
		projection = (Scale(1.0, 1.0, 1.0) * //in order to rotate the cube as a whole, instead of rotating all 27 subcubes I rotate the camera
			RotateX(Theta2[Xaxis]) *
			RotateY(Theta2[Yaxis]) *
			RotateZ(Theta2[Zaxis])) *projection;
	}
	else  projection = Ortho(-8.0* (GLfloat)w / (GLfloat)h, 8.0 *
		(GLfloat)w / (GLfloat)h, -8.0, 8.0, -8.0, 8.0);

	glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

}

void initScoreBoard()
{
	glRasterPos3f(0.8, 0.8, 0.0);
	glutBitmapCharacter(font_style1, ch1);
	glRasterPos3f(0.75, 0.8, 0.0);
	glutBitmapCharacter(font_style1, ch2);
	glRasterPos3f(0.70, 0.8, 0.0);
	glutBitmapCharacter(font_style1, ch3);
	glRasterPos3f(0.65, 0.8, 0.0);
	glutBitmapCharacter(font_style1, ch4);
	glRasterPos3f(0.60, 0.8, 0.0);
	glutBitmapCharacter(font_style1, ch5);
}

void initFailureMessage()
{
	glRasterPos3f(-0.6, 0.0, 0.0);
	glutBitmapCharacter(font_style1, chY);
	glRasterPos3f(-0.4, 0.0, 0.0);
	glutBitmapCharacter(font_style1, chO);
	glRasterPos3f(-0.2, 0.0, 0.0);
	glutBitmapCharacter(font_style1, chU);
	glRasterPos3f(0.0, 0.0, 0.0);
	glutBitmapCharacter(font_style1, chSpace);
	glRasterPos3f(0.2, 0.0, 0.0);
	glutBitmapCharacter(font_style1, chL);
	glRasterPos3f(0.4, 0.0, 0.0);
	glutBitmapCharacter(font_style1, chO);
	glRasterPos3f(0.6, 0.0, 0.0);
	glutBitmapCharacter(font_style1, chS);
	glRasterPos3f(0.8, 0.0, 0.0);
	glutBitmapCharacter(font_style1, chE);
}
void
idle(void)
{

	/*Theta[Axis] += 0.05;
	if (Theta[Axis] > 360.0) {
	Theta[Axis] -= 360.0;
	}*/

	glutPostRedisplay();
}
bool checkRotateCollision() {
	bool answer = true;

	int xLast = xGrid[cubeNumber - 1];
	int yLast = yGrid[cubeNumber - 1];
	int zLast = zGrid[cubeNumber - 1];
	bool answer1 = gameSpace[zLast][yLast][-xLast] == 0;

	int xLast2 = xGrid[cubeNumber - 2];
	int yLast2 = yGrid[cubeNumber - 2];
	int zLast2 = zGrid[cubeNumber - 2];
	bool answer2 = gameSpace[zLast2][yLast2][-xLast2] == 0;

	int xLast3 = xGrid[cubeNumber - 3];
	int yLast3 = yGrid[cubeNumber - 3];
	int zLast3 = zGrid[cubeNumber - 3];
	bool answer3 = gameSpace[zLast3][yLast3][-xLast3] == 0;

	int xLast4 = xGrid[cubeNumber - 4];
	int yLast4 = yGrid[cubeNumber - 4];
	int zLast4 = zGrid[cubeNumber - 4];
	bool answer4 = gameSpace[zLast4][yLast4][-xLast4] == 0;

	answer = answer1 && answer2 && answer3 && answer4;
	return answer;
}

//----------------------------------------------------------------------------
void
keyboard(unsigned char key, int x, int y)
{
	if (key == 'Q' | key == 'q')
		exit(0);
	if (key == 'E' | key == 'e')
		t = 100;
	if (key == 'I' | key == 'i') {
		for (int y = 20; y > 0; y--) {
			for (int x = 10; x > 0; x--) {
				printf("%i ", gameSpace[x][y][5]);
			}
			printf("\n");
		}
		printf("\n");
	}
	if (key == 'H' | key == 'h') {
		printf("To open the menu, right click on anywhere on the window. To turn the object around, use the arrow keys and M and N keys for z-axis rotation. Press z for zooming in and Z for zooming out. To initalize the object, press the I key and to quit the program press the Q key. In order to see a textured object more clearly, turn off the lights from the menu. You can later turn them back on to a light type of your desire.");
	}
	if (key == 'Z') {//zoom in and out
		Beta -= speed;
		glutPostRedisplay();
	}
	if (key == 'z') {
		Beta += speed;
		glutPostRedisplay();
	}
	if (key == 'M' | key == 'm') {//z-axis rotation
		Theta[2] += 1;

		if (Theta[2] > 360.0) {
			Theta[2] -= 360.0;
		}
	}
	if (key == 'N' | key == 'n') {
		Theta[2] -= 1;

		if (Theta[2] > 360.0) {
			Theta[2] -= 360.0;
		}
	}if (key == 'W' | key == 'w') {
		projection = RotateX(Theta2[Xaxis]) * RotateY(Theta2[Yaxis]) * RotateZ(Theta2[Zaxis]) * projection;
		Axis = Xaxis;
		Theta2[Axis] += 2;

		if (Theta2[Axis] > 360.0) {
			Theta2[Axis] -= 360.0;
		}
		reshape(512, 512); //reshape function is necessary for refreshing the view with a different camera perspective
	}
	if (key == 'A' | key == 'a') {
		projection = RotateX(Theta2[Xaxis]) * RotateY(Theta2[Yaxis]) * RotateZ(Theta2[Zaxis]) * projection;
		Axis = Yaxis;
		Theta2[Axis] -= 2;

		if (Theta2[Axis] > 360.0) {
			Theta2[Axis] -= 360.0;
		}
		reshape(512, 512);//reshape function is necessary for refreshing the view with a different camera perspective
	}
	if (key == 'S' | key == 's') {
		projection = RotateX(Theta2[Xaxis]) * RotateY(Theta2[Yaxis]) * RotateZ(Theta2[Zaxis]) * projection;
		Axis = Xaxis;
		Theta2[Axis] -= 2;

		if (Theta2[Axis] > 360.0) {
			Theta2[Axis] -= 360.0;
		}
		reshape(512, 512);//reshape function is necessary for refreshing the view with a different camera perspective
	}
	if (key == 'D' | key == 'd') {
		projection = RotateX(Theta2[Xaxis]) * RotateY(Theta2[Yaxis]) * RotateZ(Theta2[Zaxis]) * projection;
		Axis = Yaxis;
		Theta2[Axis] += 2;

		if (Theta2[Axis] > 360.0) {
			Theta2[Axis] -= 360.0;
		}
		reshape(512, 512);//reshape function is necessary for refreshing the view with a different camera perspective
	}
	if (key == 'R' | key == 'r') {
		if (checkRotateCollision()) {
			int xLast = xGrid[cubeNumber - 1];
			int yLast = yGrid[cubeNumber - 1];
			int zLast = zGrid[cubeNumber - 1];
			gameSpace[xLast][yLast][zLast] = 0;

			int xLast2 = xGrid[cubeNumber - 2];
			int yLast2 = yGrid[cubeNumber - 2];
			int zLast2 = zGrid[cubeNumber - 2];
			gameSpace[xLast2][yLast2][zLast2] = 0;

			int xLast3 = xGrid[cubeNumber - 3];
			int yLast3 = yGrid[cubeNumber - 3];
			int zLast3 = zGrid[cubeNumber - 3];
			gameSpace[xLast3][yLast3][zLast3] = 0;

			int xLast4 = xGrid[cubeNumber - 4];
			int yLast4 = yGrid[cubeNumber - 4];
			int zLast4 = zGrid[cubeNumber - 4];
			gameSpace[xLast4][yLast4][zLast4] = 0;

			gameSpace[zLast4][yLast4][-xLast4] = cubeNumber - 1;
			gameSpace[zLast4][yLast4][-xLast4] = cubeNumber - 2;
			gameSpace[zLast4][yLast4][-xLast4] = cubeNumber - 3;
			gameSpace[zLast4][yLast4][-xLast4] = cubeNumber - 4;
			Thetas[cubeNumber - 1].y += 90;
			Thetas[cubeNumber - 2].y += 90;
			Thetas[cubeNumber - 3].y += 90;
			Thetas[cubeNumber - 4].y += 90;
	}
	}
}

void processSpecialKeys(int key, int x, int y) {
	bool if1 = true;
	bool if2 = true;
	bool if3 = true;
	bool if4 = true;
	bool if5 = true;
	bool if6 = true;
	bool if7 = true;
	bool if8 = true;
	bool if9 = true;
	bool if10 = true;
	bool if11 = true;
	bool if12 = true;
	bool if13 = true;
	bool if14 = true;
	bool if15 = true;
	bool if16 = true;
	int xLast1;
	int yLast1;
	int zLast1;

	int xLast2;
	int yLast2;
	int zLast2;

	int xLast3;
	int yLast3;
	int zLast3;

	int xLast4;
	int yLast4;
	int zLast4;
	//controls the speed

	switch (key) {
	case GLUT_KEY_UP:
		if (counterZ >= -2 && counterZ < 1) {
			xLast1 = xGrid[cubeNumber - 1];
			yLast1 = yGrid[cubeNumber - 1];
			zLast1 = zGrid[cubeNumber - 1];

			xLast2 = xGrid[cubeNumber - 2];
			yLast2 = yGrid[cubeNumber - 2];
			zLast2 = zGrid[cubeNumber - 2];

			xLast3 = xGrid[cubeNumber - 3];
			yLast3 = yGrid[cubeNumber - 3];
			zLast3 = zGrid[cubeNumber - 3];

			xLast4 = xGrid[cubeNumber - 4];
			yLast4 = yGrid[cubeNumber - 4];
			zLast4 = zGrid[cubeNumber - 4];

			if1 = gameSpace[xLast1][yLast1][zLast1 - 1] == cubeNumber - 2 || gameSpace[xLast1][yLast1][zLast1 - 1] == cubeNumber - 3 ||
				gameSpace[xLast1][yLast1][zLast1 - 1] == cubeNumber - 4 || gameSpace[xLast1][yLast1][zLast1 - 1] == 0;

			if2 = gameSpace[xLast2][yLast2][zLast2 - 1] == cubeNumber - 2 || gameSpace[xLast2][yLast2][zLast2 - 1] == cubeNumber - 3 ||
				gameSpace[xLast2][yLast2][zLast2 - 1] == cubeNumber - 4 || gameSpace[xLast2][yLast2][zLast2 - 1] == 0;

			if3 = gameSpace[xLast3][yLast3][zLast3 - 1] == cubeNumber - 2 || gameSpace[xLast3][yLast3][zLast3 - 1] == cubeNumber - 3 ||
				gameSpace[xLast3][yLast3][zLast3 - 1] == cubeNumber - 4 || gameSpace[xLast3][yLast3][zLast3 - 1] == 0;

			if4 = gameSpace[xLast4][yLast4][zLast4 - 1] == cubeNumber - 2 || gameSpace[xLast4][yLast4][zLast1 - 4] == cubeNumber - 3 ||
				gameSpace[xLast4][yLast4][zLast4 - 1] == cubeNumber - 4 || gameSpace[xLast4][yLast4][zLast1 - 4] == 0;
			if (if1 && if2 && if3 && if4) {

				//if (gameSpace[xLast1][yLast1][zLast1 - 1] == cubeNumber - 2 || gameSpace[xLast1][yLast1][zLast1 - 1] == cubeNumber - 3 || gameSpace[xLast1][yLast1][zLast1 - 1] == cubeNumber - 4 || gameSpace[xLast1][yLast1][zLast1 - 1] == 0) {
				movePos[cubeNumber - 1].z += 0.6;
				gameSpace[xLast1][yLast1][zLast1] = 0;
				zGrid[cubeNumber - 1] -= 1;
				zLast1 -= 1;
				gameSpace[xLast1][yLast1][zLast1] = cubeNumber;
				//printf("%i, %i, %i\n", xLast1, yLast1, zLast1);
				//}

				//if (gameSpace[xLast2][yLast2][zLast2 - 1] == cubeNumber - 2 || gameSpace[xLast2][yLast2][zLast2 - 1] == cubeNumber - 3 ||	gameSpace[xLast2][yLast2][zLast2 - 1] == cubeNumber - 4 || gameSpace[xLast2][yLast2][zLast2 - 1] == 0) {
				movePos[cubeNumber - 2].z += 0.6;
				gameSpace[xLast2][yLast2][zLast2] = 0;
				zGrid[cubeNumber - 2] -= 1;
				zLast2 -= 1;
				gameSpace[xLast2][yLast2][zLast2] = cubeNumber - 1;
				//printf("%i, %i, %i\n", xLast2, yLast2, zLast2);
				//}

				//if (gameSpace[xLast3][yLast3][zLast3 - 1] == cubeNumber - 2 || gameSpace[xLast3][yLast3][zLast3 - 1] == cubeNumber - 3 ||	gameSpace[xLast3][yLast3][zLast3 - 1] == cubeNumber - 4 || gameSpace[xLast3][yLast3][zLast3 - 1] == 0) {
				movePos[cubeNumber - 3].z += 0.6;
				gameSpace[xLast3][yLast3][zLast3] = 0;
				zGrid[cubeNumber - 3] -= 1;
				zLast3 -= 1;
				gameSpace[xLast3][yLast3][zLast3] = cubeNumber - 2;
				//printf("%i, %i, %i\n", xLast3, yLast3, zLast3);
				//}

				//if (gameSpace[xLast4][yLast4][zLast4 - 1] == cubeNumber - 2 || gameSpace[xLast4][yLast4][zLast1 - 4] == cubeNumber - 3 ||	gameSpace[xLast4][yLast4][zLast4 - 1] == cubeNumber - 4 || gameSpace[xLast4][yLast4][zLast1 - 4] == 0) {
				movePos[cubeNumber - 4].z += 0.6;
				gameSpace[xLast4][yLast4][zLast4] = 0;
				zGrid[cubeNumber - 4] -= 1;
				zLast4 -= 1;
				gameSpace[xLast4][yLast4][zLast4] = cubeNumber - 3;
				//printf("%i, %i, %i\n", xLast4, yLast4, zLast4);
				//}
			}
			counterZ++;
		}
		break;
	case GLUT_KEY_DOWN:
		if (counterZ > -2 && counterZ <= 1) {
			xLast1 = xGrid[cubeNumber - 1];
			yLast1 = yGrid[cubeNumber - 1];
			zLast1 = zGrid[cubeNumber - 1];

			xLast2 = xGrid[cubeNumber - 2];
			yLast2 = yGrid[cubeNumber - 2];
			zLast2 = zGrid[cubeNumber - 2];

			xLast3 = xGrid[cubeNumber - 3];
			yLast3 = yGrid[cubeNumber - 3];
			zLast3 = zGrid[cubeNumber - 3];

			xLast4 = xGrid[cubeNumber - 4];
			yLast4 = yGrid[cubeNumber - 4];
			zLast4 = zGrid[cubeNumber - 4];

			if5 = gameSpace[xLast1][yLast1][zLast1 + 1] == cubeNumber - 2 || gameSpace[xLast1][yLast1][zLast1 + 1] == cubeNumber - 3 ||
				gameSpace[xLast1][yLast1][zLast1 + 1] == cubeNumber - 4 || gameSpace[xLast1][yLast1][zLast1 + 1] == 0;

			if6 = gameSpace[xLast2][yLast2][zLast2 + 1] == cubeNumber - 1 || gameSpace[xLast2][yLast2][zLast2 + 1] == cubeNumber - 3 ||
				gameSpace[xLast2][yLast2][zLast2 + 1] == cubeNumber - 4 || gameSpace[xLast2][yLast2][zLast2 + 1] == 0;

			if7 = gameSpace[xLast3][yLast3][zLast3 + 1] == cubeNumber - 1 || gameSpace[xLast3][yLast3][zLast3 + 1] == cubeNumber - 2 ||
				gameSpace[xLast3][yLast3][zLast3 + 1] == cubeNumber - 4 || gameSpace[xLast3][yLast3][zLast3 + 1] == 0;

			if8 = gameSpace[xLast4][yLast4][zLast1 + 4] == cubeNumber - 1 || gameSpace[xLast4][yLast4][zLast4 + 1] == cubeNumber - 2 ||
				gameSpace[xLast4][yLast4][zLast1 + 4] == cubeNumber - 3 || gameSpace[xLast4][yLast4][zLast4 + 1] == 0;

			if (if1 && if2 && if3 && if4) {


				//if (gameSpace[xLast1][yLast1][zLast1 + 1] == cubeNumber - 2 || gameSpace[xLast1][yLast1][zLast1 + 1] == cubeNumber - 3 ||	gameSpace[xLast1][yLast1][zLast1 + 1] == cubeNumber - 4 || gameSpace[xLast1][yLast1][zLast1 + 1] == 0) {
				movePos[cubeNumber - 1].z -= 0.6;
				gameSpace[xLast1][yLast1][zLast1] = 0;
				zGrid[cubeNumber - 1] += 1;
				zLast1 += 1;
				gameSpace[xLast1][yLast1][zLast1] = cubeNumber;
				//printf("%i, %i, %i\n", xLast, yLast, zLast);
				//}

				//if (gameSpace[xLast2][yLast2][zLast2 + 1] == cubeNumber - 1 || gameSpace[xLast2][yLast2][zLast2 + 1] == cubeNumber - 3 || gameSpace[xLast2][yLast2][zLast2 + 1] == cubeNumber - 4 || gameSpace[xLast2][yLast2][zLast2 + 1] == 0) {
				movePos[cubeNumber - 2].z -= 0.6;
				gameSpace[xLast2][yLast2][zLast2] = 0;
				zGrid[cubeNumber - 2] += 1;
				zLast2 += 1;
				gameSpace[xLast2][yLast2][zLast2] = cubeNumber - 1;
				//printf("%i, %i, %i\n", xLast, yLast, zLast);
				//}

				//if (gameSpace[xLast3][yLast3][zLast3 + 1] == cubeNumber - 1 || gameSpace[xLast3][yLast3][zLast3 + 1] == cubeNumber - 2 || gameSpace[xLast3][yLast3][zLast3 + 1] == cubeNumber - 4 || gameSpace[xLast3][yLast3][zLast3 + 1] == 0) {
				movePos[cubeNumber - 3].z -= 0.6;
				gameSpace[xLast3][yLast3][zLast3] = 0;
				zGrid[cubeNumber - 3] += 1;
				zLast3 += 1;
				gameSpace[xLast3][yLast3][zLast3] = cubeNumber - 2;
				//printf("%i, %i, %i\n", xLast, yLast, zLast);
				//}

				//if (gameSpace[xLast4][yLast4][zLast1 + 4] == cubeNumber - 1 || gameSpace[xLast4][yLast4][zLast4 + 1] == cubeNumber - 2 || 	gameSpace[xLast4][yLast4][zLast1 + 4] == cubeNumber - 3 || gameSpace[xLast4][yLast4][zLast4 + 1] == 0) {
				movePos[cubeNumber - 4].z -= 0.6;
				gameSpace[xLast4][yLast4][zLast4] = 0;
				zGrid[cubeNumber - 4] += 1;
				zLast4 += 1;
				gameSpace[xLast4][yLast4][zLast4] = cubeNumber - 3;
				//printf("%i, %i, %i\n", xLast, yLast, zLast);
				//}
			}
			counterZ--;
		}
		break;
	case GLUT_KEY_RIGHT:
		if (shape == 1) {

		}
		else if(counterX < 0 && counterX >= -1){
			xLast1 = xGrid[cubeNumber - 1];
			yLast1 = yGrid[cubeNumber - 1];
			zLast1 = zGrid[cubeNumber - 1];

			xLast2 = xGrid[cubeNumber - 2];
			yLast2 = yGrid[cubeNumber - 2];
			zLast2 = zGrid[cubeNumber - 2];

			xLast3 = xGrid[cubeNumber - 3];
			yLast3 = yGrid[cubeNumber - 3];
			zLast3 = zGrid[cubeNumber - 3];

			xLast4 = xGrid[cubeNumber - 4];
			yLast4 = yGrid[cubeNumber - 4];
			zLast4 = zGrid[cubeNumber - 4];

			if9 = gameSpace[xLast1 + 1][yLast1][zLast1] == cubeNumber - 2 || gameSpace[xLast1 + 1][yLast1][zLast1] == cubeNumber - 3 ||
				gameSpace[xLast1 + 1][yLast1][zLast1] == cubeNumber - 4 || gameSpace[xLast1 + 1][yLast1][zLast1] == 0;

			if10 = gameSpace[xLast2 + 1][yLast2][zLast2] == cubeNumber - 1 || gameSpace[xLast2 + 1][yLast2][zLast2] == cubeNumber - 3 ||
				gameSpace[xLast2 + 1][yLast2][zLast2] == cubeNumber - 4 || gameSpace[xLast2 + 1][yLast2][zLast2] == 0;

			if11 = gameSpace[xLast3 + 1][yLast3][zLast3] == cubeNumber - 1 || gameSpace[xLast1 + 3][yLast3][zLast3] == cubeNumber - 2 ||
				gameSpace[xLast3 + 1][yLast3][zLast3] == cubeNumber - 4 || gameSpace[xLast1 + 3][yLast3][zLast3] == 0;

			if12 = gameSpace[xLast4 + 1][yLast4][zLast4] == cubeNumber - 1 || gameSpace[xLast4 + 1][yLast4][zLast4] == cubeNumber - 2 ||
				gameSpace[xLast4 + 1][yLast4][zLast4] == cubeNumber - 3 || gameSpace[xLast4 + 1][yLast4][zLast4] == 0;
			if (if1 && if2 && if3 && if4) {
				//if (gameSpace[xLast1 + 1][yLast1][zLast1] == cubeNumber - 2 || gameSpace[xLast1 + 1][yLast1][zLast1] == cubeNumber - 3 || gameSpace[xLast1 + 1][yLast1][zLast1] == cubeNumber - 4 || gameSpace[xLast1 + 1][yLast1][zLast1] == 0) {
				movePos[cubeNumber - 1].x -= 0.6;
				gameSpace[xLast1][yLast1][zLast1] = 0;
				xGrid[cubeNumber - 1] += 1;
				xLast1 += 1;
				gameSpace[xLast1][yLast1][zLast1] = cubeNumber;
				//printf("%i, %i, %i\n", xLast1, yLast1, zLast1);
				//}

				//if (gameSpace[xLast2 + 1][yLast2][zLast2] == cubeNumber - 1 || gameSpace[xLast2 + 1][yLast2][zLast2] == cubeNumber - 3 || gameSpace[xLast2 + 1][yLast2][zLast2] == cubeNumber - 4 || gameSpace[xLast2 + 1][yLast2][zLast2] == 0) {
				movePos[cubeNumber - 2].x -= 0.6;
				gameSpace[xLast2][yLast2][zLast2] = 0;
				xGrid[cubeNumber - 2] += 1;
				xLast2 += 1;
				gameSpace[xLast2][yLast2][zLast2] = cubeNumber - 1;
				//printf("%i, %i, %i\n", xLast2, yLast2, zLast2);
				//}

				//if (gameSpace[xLast3 + 1][yLast3][zLast3] == cubeNumber - 1 || gameSpace[xLast1 + 3][yLast3][zLast3] == cubeNumber - 2 || gameSpace[xLast3 + 1][yLast3][zLast3] == cubeNumber - 4 || gameSpace[xLast1 + 3][yLast3][zLast3] == 0) {
				movePos[cubeNumber - 3].x -= 0.6;
				gameSpace[xLast3][yLast3][zLast3] = 0;
				xGrid[cubeNumber - 3] += 1;
				xLast3 += 1;
				gameSpace[xLast3][yLast3][zLast3] = cubeNumber - 2;
				//printf("%i, %i, %i\n", xLast3, yLast3, zLast3);
				//}

				//if (gameSpace[xLast4 + 1][yLast4][zLast4] == cubeNumber - 1 || gameSpace[xLast4 + 1][yLast4][zLast4] == cubeNumber - 2 || gameSpace[xLast4 + 1][yLast4][zLast4] == cubeNumber - 3 || gameSpace[xLast4 + 1][yLast4][zLast4] == 0) {
				movePos[cubeNumber - 4].x -= 0.6;
				gameSpace[xLast4][yLast4][zLast4] = 0;
				xGrid[cubeNumber - 4] += 1;
				xLast4 += 1;
				gameSpace[xLast4][yLast4][zLast4] = cubeNumber - 3;
				//printf("%i, %i, %i\n", xLast4, yLast4, zLast4);
				//}
			}
			counterX++;
		}
		break;
	case GLUT_KEY_LEFT:
		if (shape == 1) {

		}
		else if (counterX <= 0 && counterX > -1) {
			xLast1 = xGrid[cubeNumber - 1];
			yLast1 = yGrid[cubeNumber - 1];
			zLast1 = zGrid[cubeNumber - 1];

			xLast2 = xGrid[cubeNumber - 2];
			yLast2 = yGrid[cubeNumber - 2];
			zLast2 = zGrid[cubeNumber - 2];

			xLast3 = xGrid[cubeNumber - 3];
			yLast3 = yGrid[cubeNumber - 3];
			zLast3 = zGrid[cubeNumber - 3];

			xLast4 = xGrid[cubeNumber - 4];
			yLast4 = yGrid[cubeNumber - 4];
			zLast4 = zGrid[cubeNumber - 4];

			if13 = gameSpace[xLast1 - 1][yLast1][zLast1] == cubeNumber - 2 || gameSpace[xLast1 - 1][yLast1][zLast1] == cubeNumber - 3 ||
				gameSpace[xLast1 - 1][yLast1][zLast1] == cubeNumber - 4 || gameSpace[xLast1 - 1][yLast1][zLast1] == 0;

			if14 = gameSpace[xLast2 - 1][yLast2][zLast2] == cubeNumber - 1 || gameSpace[xLast2 - 1][yLast2][zLast2] == cubeNumber - 3 ||
				gameSpace[xLast2 - 1][yLast2][zLast2] == cubeNumber - 4 || gameSpace[xLast2 - 1][yLast2][zLast2] == 0;

			if15 = gameSpace[xLast3 - 1][yLast3][zLast3] == cubeNumber - 1 || gameSpace[xLast1 - 3][yLast3][zLast3] == cubeNumber - 2 ||
				gameSpace[xLast3 - 1][yLast3][zLast3] == cubeNumber - 4 || gameSpace[xLast1 - 3][yLast3][zLast3] == 0;

			if16 = gameSpace[xLast4 - 1][yLast4][zLast4] == cubeNumber - 1 || gameSpace[xLast4 - 1][yLast4][zLast4] == cubeNumber - 2 ||
				gameSpace[xLast4 - 1][yLast4][zLast4] == cubeNumber - 3 || gameSpace[xLast4 - 1][yLast4][zLast4] == 0;
			if (if1 && if2 && if3 && if4) {
				//if (gameSpace[xLast1 - 1][yLast1][zLast1] == cubeNumber - 2 || gameSpace[xLast1 - 1][yLast1][zLast1] == cubeNumber - 3 || gameSpace[xLast1 - 1][yLast1][zLast1] == cubeNumber - 4 || gameSpace[xLast1 - 1][yLast1][zLast1] == 0) {
				movePos[cubeNumber - 1].x += 0.6;
				gameSpace[xLast1][yLast1][zLast1] = 0;
				xGrid[cubeNumber - 1] -= 1;
				xLast1 -= 1;
				gameSpace[xLast1][yLast1][zLast1] = cubeNumber;
				//printf("%i, %i, %i\n", xLast1, yLast1, zLast1);
				//}

				//if (gameSpace[xLast2 - 1][yLast2][zLast2] == cubeNumber - 1 || gameSpace[xLast2 - 1][yLast2][zLast2] == cubeNumber - 3 || gameSpace[xLast2 - 1][yLast2][zLast2] == cubeNumber - 4 || gameSpace[xLast2 - 1][yLast2][zLast2] == 0) {
				movePos[cubeNumber - 2].x += 0.6;
				gameSpace[xLast2][yLast2][zLast2] = 0;
				xGrid[cubeNumber - 2] -= 1;
				xLast2 -= 1;
				gameSpace[xLast2][yLast2][zLast2] = cubeNumber - 1;
				//printf("%i, %i, %i\n", xLast, yLast, zLast);
				//}

				//if (gameSpace[xLast3 - 1][yLast3][zLast3] == cubeNumber - 1 || gameSpace[xLast1 - 3][yLast3][zLast3] == cubeNumber - 2 || gameSpace[xLast3 - 1][yLast3][zLast3] == cubeNumber - 4 || gameSpace[xLast1 - 3][yLast3][zLast3] == 0) {
				movePos[cubeNumber - 3].x += 0.6;
				gameSpace[xLast3][yLast3][zLast3] = 0;
				xGrid[cubeNumber - 3] -= 1;
				xLast3 -= 1;
				gameSpace[xLast3][yLast3][zLast3] = cubeNumber - 2;
				//printf("%i, %i, %i\n", xLast, yLast, zLast);
				//}

				//if (gameSpace[xLast4 - 1][yLast4][zLast4] == cubeNumber - 1 || gameSpace[xLast4 - 1][yLast4][zLast4] == cubeNumber - 2 ||gameSpace[xLast4 - 1][yLast4][zLast4] == cubeNumber - 3 || gameSpace[xLast4 - 1][yLast4][zLast4] == 0) {
				movePos[cubeNumber - 4].x += 0.6;
				gameSpace[xLast4][yLast4][zLast4] = 0;
				xGrid[cubeNumber - 4] -= 1;
				xLast4 -= 1;
				gameSpace[xLast4][yLast4][zLast4] = cubeNumber - 3;
				//printf("%i, %i, %i\n", xLast, yLast, zLast);
				//}
			}
			counterX--;
		}
		break;

	}
}


void updateScoreBoard(int pointsEarned)
{
	score += pointsEarned;
	int scoreTemp = score;
	int ch = scoreTemp % 10;
	scoreTemp = scoreTemp / 10;
	ch1 = 48 + ch;
	ch2 = 48 + ((scoreTemp) % 10);
	scoreTemp = scoreTemp / 10;
	ch3 = 48 + (scoreTemp) % 10;
	scoreTemp = scoreTemp / 10;
	ch4 = 48 + (scoreTemp) % 10;
	scoreTemp = scoreTemp / 10;
	ch5 = 48 + (scoreTemp) % 10;
}

void failureMessageDisplay()
{
	chY = 89;
	chO = 79;
	chU = 85;
	chSpace = 32;
	chL = 76;
	chS = 83;
	chE = 69;
}

bool checkCollision() {
	bool answer = true;

	int xLast = xGrid[cubeNumber - 1];
	int yLast = yGrid[cubeNumber - 1] - 1;
	int zLast = zGrid[cubeNumber - 1];
	bool answer1 = gameSpace[xLast][yLast][zLast] == cubeNumber - 2 || gameSpace[xLast][yLast][zLast] == cubeNumber - 3 || gameSpace[xLast][yLast][zLast] == cubeNumber - 4 || gameSpace[xLast][yLast][zLast] == 0;

	xLast = xGrid[cubeNumber - 2];
	yLast = yGrid[cubeNumber - 2] - 1;
	zLast = zGrid[cubeNumber - 2];
	bool answer2 = gameSpace[xLast][yLast][zLast] == cubeNumber - 1 || gameSpace[xLast][yLast][zLast] == cubeNumber - 3 || gameSpace[xLast][yLast][zLast] == cubeNumber - 4 || gameSpace[xLast][yLast][zLast] == 0;

	xLast = xGrid[cubeNumber - 3];
	yLast = yGrid[cubeNumber - 3] - 1;
	zLast = zGrid[cubeNumber - 3];
	bool answer3 = gameSpace[xLast][yLast][zLast] == cubeNumber - 1 || gameSpace[xLast][yLast][zLast] == cubeNumber - 2 || gameSpace[xLast][yLast][zLast] == cubeNumber - 4 || gameSpace[xLast][yLast][zLast] == 0;

	xLast = xGrid[cubeNumber - 4];
	yLast = yGrid[cubeNumber - 4] - 1;
	zLast = zGrid[cubeNumber - 4];
	bool answer4 = gameSpace[xLast][yLast][zLast] == cubeNumber - 1 || gameSpace[xLast][yLast][zLast] == cubeNumber - 2 || gameSpace[xLast][yLast][zLast] == cubeNumber - 3 || gameSpace[xLast][yLast][zLast] == 0;

	answer = answer1 && answer2 && answer3 && answer4;
	return answer;
}



void timer(int p)
{
	//t = 900;
	/*
	if(yGridPosition < 20)
	viewer_pos.y += 0.6;
	yGridPosition++;
	*/
	if (yGridPosition < groundHeight && checkCollision()) {
		//if (-movePos[cubeNumber - 1].y > groundPosY * 2 && -movePos[cubeNumber - 2].y > groundPosY * 2 && -movePos[cubeNumber - 3].y > groundPosY * 2 && -movePos[cubeNumber - 4].y > groundPosY * 2 && checkCollision()) {


		int xLast1 = xGrid[cubeNumber - 1];
		int yLast1 = yGrid[cubeNumber - 1];
		int zLast1 = zGrid[cubeNumber - 1];
		gameSpace[xLast1][yLast1][zLast1] = 0;
		movePos[cubeNumber - 1].y += 0.6;
		yGrid[cubeNumber - 1] -= 1;
		//printf("%i, %i, %i\n", xLast1, yLast1, zLast1);
		yLast1 -= 1;
		gameSpace[xLast1][yLast1][zLast1] = cubeNumber;
		//printf("%i\n", gameSpace[xLast1][yLast1][zLast1]);

		int xLast2 = xGrid[cubeNumber - 2];
		int yLast2 = yGrid[cubeNumber - 2];
		int zLast2 = zGrid[cubeNumber - 2];
		gameSpace[xLast2][yLast2][zLast2] = 0;
		movePos[cubeNumber - 2].y += 0.6;
		yGrid[cubeNumber - 2] -= 1;
		//printf("%i, %i, %i\n", xLast2, yLast2, zLast2);
		yLast2 -= 1;
		gameSpace[xLast2][yLast2][zLast2] = cubeNumber - 1;
		//printf("%i\n", gameSpace[xLast2][yLast2][zLast2]);

		int xLast3 = xGrid[cubeNumber - 3];
		int yLast3 = yGrid[cubeNumber - 3];
		int zLast3 = zGrid[cubeNumber - 3];
		gameSpace[xLast3][yLast3][zLast3] = 0;
		movePos[cubeNumber - 3].y += 0.6;
		yGrid[cubeNumber - 3] -= 1;
		//printf("%i, %i, %i\n", xLast3, yLast3, zLast3);
		yLast3 -= 1;
		gameSpace[xLast3][yLast3][zLast3] = cubeNumber - 2;
		//printf("%i\n", gameSpace[xLast3][yLast3][zLast3]);

		int xLast4 = xGrid[cubeNumber - 4];
		int yLast4 = yGrid[cubeNumber - 4];
		int zLast4 = zGrid[cubeNumber - 4];
		gameSpace[xLast4][yLast4][zLast4] = 0;
		movePos[cubeNumber - 4].y += 0.6;
		yGrid[cubeNumber - 4] -= 1;
		//printf("%i, %i, %i\n", xLast4, yLast4, zLast4);
		yLast4 -= 1;
		gameSpace[xLast4][yLast4][zLast4] = cubeNumber - 3;
		//printf("%i\n", gameSpace[xLast4][yLast4][zLast4]);


		yGridPosition++;
		//printf("%f, %f, %f, %f\n", movePos[cubeNumber - 1].y, movePos[cubeNumber - 2].y, movePos[cubeNumber - 3].y, movePos[cubeNumber - 4].y);
	}
	else {
		yGridPosition = 0;

		int x1 = xGrid[cubeNumber - 1];
		int y1 = yGrid[cubeNumber - 1];
		int z1 = zGrid[cubeNumber - 1];

		int x2 = xGrid[cubeNumber - 2];
		int y2 = yGrid[cubeNumber - 2];
		int z2 = zGrid[cubeNumber - 2];

		int x3 = xGrid[cubeNumber - 3];
		int y3 = yGrid[cubeNumber - 3];
		int z3 = zGrid[cubeNumber - 3];

		int x4 = xGrid[cubeNumber - 4];
		int y4 = yGrid[cubeNumber - 4];
		int z4 = zGrid[cubeNumber - 4];

		gameSpace[x1][y1][z1] = cubeNumber - 1;
		gameSpace[x2][y2][z2] = cubeNumber - 2;
		gameSpace[x3][y3][z3] = cubeNumber - 3;
		gameSpace[x4][y4][z4] = cubeNumber - 4;

		//newStick();
		createRandomObject();

		glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals) + sizeof(texture), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), sizeof(texture), texture);
	}
	score++;
	if (score == 99999) {
		score = 0;
	}
	demolishRow();
	glutPostRedisplay();

	glutTimerFunc(t, timer, 0);

}

void keyUp(unsigned char key, int x, int y) {
	if (key == 'E' || key == 'e') { 
		t = 900;
	}
}

//-----------------------------------------------------menu actions.
void quit(int value)
{
	switch (value)
	{
	case 0:
		exit(EXIT_SUCCESS);
		break;
	}
}

void musicMenuAction(int i)
{
	switch (i)
	{
	case 0:
		PlaySound("classic.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
		break;
	case 1:
		PlaySound("traditional.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
		break;
	case 2:
		PlaySound("techno.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
		break;
	case 3:
		PlaySound("hiphop.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
		break;
	}
}

void createMenu();
//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Color Cube");
	glewExperimental = GL_TRUE;
	glewInit();
	init();
	createMenu();
	//PlaySound("classic.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	glutTimerFunc(5, timer, 0);

	glutDisplayFunc(display); // set display callback function
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(processSpecialKeys);

	glutMainLoop();
	return 0;
}

void createMenu()
{
	int musicMenu = glutCreateMenu(musicMenuAction); // object type.
	glutAddMenuEntry("Classic", 0);
	glutAddMenuEntry("Traditional", 1);
	glutAddMenuEntry("Techno", 2);
	glutAddMenuEntry("HipHop", 3);

	glutCreateMenu(quit);
	glutAddSubMenu("Select Music", musicMenu);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}