//
//  Display a rotating cube 
//

#include "Angel.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include "TetrisObjects.h"
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
std::vector<GLfloat> xGrid;
std::vector<GLfloat> yGrid;
std::vector<GLfloat> zGrid;
								 // Vertices of a unit cube centered at origin, sides aligned with axes
//GLfloat cubeLengthHalf = 0.3;
//GLfloat groundLenghtHalf = 10 * cubeLengthHalf;
//GLfloat cubeStartingPosY = cubeLengthHalf * 10 * 2;
//GLfloat groundPosY = -cubeStartingPosY;

GLfloat gameSpace[10][20][10];	//10x10 ground tiles containing the elevation as y position. Will be initialized in init


vec3 viewer_pos(cubeLengthHalf, 0.0, cubeLengthHalf);	//Bu deðiþmeli

point4 vertices[8] = {
	point4(-cubeLengthHalf*2, cubeStartingPosY,  0, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY + 2 * cubeLengthHalf,  0, 1.0),
	point4(0,  cubeStartingPosY + 2 * cubeLengthHalf,  0, 1.0),
	point4(0, cubeStartingPosY,  0, 1.0),
	point4(-cubeLengthHalf*2, cubeStartingPosY, -cubeLengthHalf*2, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4(0,  cubeStartingPosY + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4(0, cubeStartingPosY, -cubeLengthHalf*2, 1.0)
};
point4 groundVertices[4] = {
	point4(-groundLenghtHalf, groundPosY, -groundLenghtHalf, 1.0),
	point4(groundLenghtHalf, groundPosY, -groundLenghtHalf, 1.0),
	point4(groundLenghtHalf, groundPosY, groundLenghtHalf, 1.0),
	point4(-groundLenghtHalf, groundPosY, groundLenghtHalf, 1.0)	
};


color4 main_colour = color4(1.0, 0.0, 0.0, 1.0);
color4 ground_colour = color4(1.0, 0.0, 1.0, 1.0);

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
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
quad(int a, int b, int c, int d, point4 vertices[8])
{
		vec4 u = vertices[b] - vertices[a];
		vec4 v = vertices[c] - vertices[b];

		vec3 normal = normalize(cross(u, v));
		normals[Index] = normal; colors[Index] = main_colour; pointsList.push_back(vertices[a]); Index++;
		normals[Index] = normal; colors[Index] = main_colour; pointsList.push_back(vertices[b]); Index++;
		normals[Index] = normal; colors[Index] = main_colour; pointsList.push_back(vertices[c]); Index++;
		normals[Index] = normal; colors[Index] = main_colour; pointsList.push_back(vertices[a]); Index++;
		normals[Index] = normal; colors[Index] = main_colour; pointsList.push_back(vertices[c]); Index++;
		normals[Index] = normal; colors[Index] = main_colour; pointsList.push_back(vertices[d]); Index++;
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
		for (int y = 0; y < 10; y++) {
			for (int z = 0; z < 20; z++) {
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
	bool isEqual = false;
	//if all the entries in groundTileY are equal and not groundPosY, demolish row
	//row is all squares having the same resting position
	
}

void populatePoints() {
	for (int i = 0; i < pointsList.size(); i++) {
		points[i] = pointsList[i];
	}
}




// generate 12 triangles: 36 vertices and 36 colors
void
colorcube(point4 v[8])
{
	quad(1, 0, 3, 2, v);
	quad(2, 3, 7, 6, v);
	quad(3, 0, 4, 7, v);
	quad(6, 5, 1, 2, v);
	quad(4, 5, 6, 7, v);
	quad(5, 4, 0, 1, v);
	movePos.push_back((0.0, 0.0, 0.0));
	xGrid.push_back(5);
	yGrid.push_back(19);
	zGrid.push_back(5);
	cubeNumber++;
	gameSpace[5][19][5] = 1;
	model_views.push_back(NULL);
}

void newBlock(point4 v[8]) {
	colorcube(v);
	populatePoints();
}

void newStick() {
	newBlock(vertices);
	newBlock(vertices_pos1X);
	newBlock(vertices_neg1X);
	newBlock(vertices_neg2X);
}

void newLetterT() {
	newBlock(vertices);
	newBlock(vertices_pos1X);
	newBlock(vertices_neg1X);
	newBlock(vertices_pos1Y);
}

void newLetterS() {
	newBlock(vertices);
	newBlock(vertices_pos1X1Y);
	newBlock(vertices_pos1Y);
	newBlock(vertices_neg1X);
}

void newLetterZ() {
	newBlock(vertices);
	newBlock(vertices_neg1X1Y);
	newBlock(vertices_pos1Y);
	newBlock(vertices_neg1X);
}

void newLetterL() {
	newBlock(vertices);
	newBlock(vertices_pos1X1Y);
	newBlock(vertices_pos1X);
	newBlock(vertices_neg1X);

}

//----------------------------------------------------------------------------



// OpenGL initialization
void
init()
{
	floorQuad(1, 0, 3, 2);
	initGroundTiles();
	newLetterL();
	populatePoints();
	

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object

	GLuint buffer;
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
	if(isPoint)
		glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	else
		glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position2);
	if(isMetal)
		glUniform1f(glGetUniformLocation(program, "Shininess"),material_shininess);
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

	if(isOrtho)
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
	
	

	for (int i = 0; i < cubeNumber; i++) {
		model_views[cubeNumber - 1] = ((Translate(-movePos[cubeNumber-1]) * //modelview of the object
			RotateX(Theta[Xaxis]) *
			RotateY(Theta[Yaxis]) *
			RotateZ(Theta[Zaxis]) *
			Scale(Beta, Beta, Beta)));
		glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_views[i]);
		glDrawArrays(GL_TRIANGLES, 6 , NumVertices);
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

void
idle(void)
{

	/*Theta[Axis] += 0.05;

	if (Theta[Axis] > 360.0) {
	Theta[Axis] -= 360.0;
	}*/

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	if (key == 'Q' | key == 'q')
		exit(0);
	if (key == 'I' | key == 'i') {// initalizes the values
		//printf("%i\n", ((cubeNumber - 1) * 36));
		printf("%i\n", cubeNumber);
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
}

void processSpecialKeys(int key, int x, int y) { //controls the speed

	switch (key) {
		int xLast;
		int yLast;
		int zLast;
	case GLUT_KEY_UP:
		xLast = xGrid.back();
		yLast = yGrid.back();
		zLast = zGrid.back();
		if (gameSpace[xLast][yLast][zLast - 1] == 0) {
			movePos[cubeNumber - 1].z -= 0.6;
			gameSpace[xLast][yLast][zLast] = 0;
			zGrid[cubeNumber - 1] -= 1;
			zLast -= 1;
			gameSpace[xLast][yLast][zLast] = 1;
			printf("%i, %i, %i\n", xLast, yLast, zLast);
		}
		break;
	case GLUT_KEY_DOWN:
		xLast = xGrid.back();
		yLast = yGrid.back();
		zLast = zGrid.back();
		if (gameSpace[xLast][yLast][zLast + 1] == 0) {
			movePos[cubeNumber - 1].z += 0.6;
			gameSpace[xLast][yLast][zLast] = 0;
			zGrid[cubeNumber - 1] += 1;
			zLast += 1;
			gameSpace[xLast][yLast][zLast] = 1;
			//printf("%i, %i, %i\n", xLast, yLast, zLast);
		}
		break;
	case GLUT_KEY_RIGHT:
		xLast = xGrid.back();
		yLast = yGrid.back();
		zLast = zGrid.back();
		if (gameSpace[xLast - 1][yLast][zLast] == 0) {
			movePos[cubeNumber - 1].x -= 0.6;
			gameSpace[xLast][yLast][zLast] = 0;
			xGrid[cubeNumber - 1] -= 1;
			xLast -= 1;
			gameSpace[xLast][yLast][zLast] = 1;
			//printf("%i, %i, %i\n", xLast, yLast, zLast);
		}
		break;
	case GLUT_KEY_LEFT:
		xLast = xGrid.back();
		yLast = yGrid.back();
		zLast = zGrid.back();
		if (gameSpace[xLast + 1][yLast][zLast] == 0) {
			movePos[cubeNumber - 1].x += 0.6;
			gameSpace[xLast][yLast][zLast] = 0;
			xGrid[cubeNumber - 1] += 1;
			xLast += 1;
			gameSpace[xLast][yLast][zLast] = 1;
			//printf("%i, %i, %i\n", xLast, yLast, zLast);
		}
		break;

	}
}


bool checkCollision() {
	int xLast = xGrid.back();
	int yLast = yGrid.back() - 1;
	int zLast = zGrid.back();

	return gameSpace[xLast][yLast][zLast] == 0;
}

void timer(int p)
{

	/*
	if(yGridPosition < 20)
		viewer_pos.y += 0.6;
	yGridPosition++;
	*/
	if (-movePos[cubeNumber - 1].y >= groundPosY*2 && checkCollision()) {
		int xLast = xGrid.back();
		int yLast = yGrid.back();
		int zLast = zGrid.back();
		gameSpace[xLast][yLast][zLast] = 0;
		yGrid[cubeNumber-1] -= 1;
		printf("%i, %i, %i\n", xLast, yLast, zLast);
		yLast -= 1;
		gameSpace[xLast][yLast][zLast] = 1;
		movePos[cubeNumber-1].y += 0.6;
	}
	else {
		viewer_pos.y = 0;
		newLetterL();
	}
	glutPostRedisplay();

	glutTimerFunc(1000, timer, 0);
}


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
	glutTimerFunc(5, timer, 0);

	glutDisplayFunc(display); // set display callback function
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);

	glutMainLoop();
	return 0;
}