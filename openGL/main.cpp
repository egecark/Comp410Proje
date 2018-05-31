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
using namespace std; 

typedef vec4  color4;
typedef vec4  point4;
std::vector<mat4> model_views;
mat4 model_view;
mat4 model_view_floor;
mat4  projection;
vec3 viewer_pos(0.0, 0.0, 1.0);
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

// Initialize shader lighting parameters
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
std::vector<vec3> shapeNormal; //stores the normals of the shapeX
std::vector<vec3> shapeQuad; //stores the faces of shapeX
std::vector<vec2> texCoords; //stores the texture coordinates of shapeX

								 // Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.25, 6.0,  0.25, 1.0),
	point4(-0.25,  6.6,  0.25, 1.0),
	point4(0.25,  6.6,  0.25, 1.0),
	point4(0.25, 6.0,  0.25, 1.0),
	point4(-0.25, 6.0, -0.25, 1.0),
	point4(-0.25,  6.6, -0.25, 1.0),
	point4(0.25,  6.6, -0.25, 1.0),
	point4(0.25, 6.0, -0.25, 1.0)
};
point4 groundVertices[4] = {
	point4(-6.0, -6.0, -6.0, 1.0),
	point4(6.0, -6.0, -6.0, 1.0),
	point4(6.0, -6.0, 6.0, 1.0),
	point4(-6.0, -6.0, 6.0, 1.0)
	
};


color4 main_colour = color4(1.0, 0.0, 0.0, 1.0);

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
quad(int a, int b, int c, int d)
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
	colors[Index] = main_colour; pointsList.push_back(groundVertices[a]); Index++;
	colors[Index] = main_colour; pointsList.push_back(groundVertices[b]); Index++;
	colors[Index] = main_colour; pointsList.push_back(groundVertices[c]); Index++;
	colors[Index] = main_colour; pointsList.push_back(groundVertices[a]); Index++;
	colors[Index] = main_colour; pointsList.push_back(groundVertices[c]); Index++;
	colors[Index] = main_colour; pointsList.push_back(groundVertices[d]); Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	floorQuad(1, 0, 3, 2);
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
//----------------------------------------------------------------------------

void createNew() {

}

void populatePoints() {
	for (int i = 0; i < pointsList.size(); i++) {
		points[i] = pointsList[i];
	}
}

// OpenGL initialization
void
init()
{
	colorcube();
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
	
	model_view = (Translate(-viewer_pos) * //modelview of the object
		RotateX(Theta[Xaxis]) *
		RotateY(Theta[Yaxis]) *
		RotateZ(Theta[Zaxis]) *
		Scale(Beta, Beta, Beta));

	glUniformMatrix4fv(ModelView, 1, GL_TRUE, model_view);
	glDrawArrays(GL_TRIANGLES, 6, NumVertices-6);
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
		Beta = 1.0;
		//model_view = Scale(Beta, Beta, Beta) *  RotateX(0) * RotateY(0) * RotateZ(0);
		Theta[0] = 0;
		Theta[1] = 0;
		Theta[2] = 0;
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
	case GLUT_KEY_UP:

		viewer_pos.z -= 0.6;
		break;
	case GLUT_KEY_DOWN:
		viewer_pos.z += 0.6;
		break;
	case GLUT_KEY_RIGHT:
		viewer_pos.x += 0.6;
		break;
	case GLUT_KEY_LEFT:
		viewer_pos.x -= 0.6;
		break;

	}
}

void timer(int p)
{
	if(yGridPosition < 20)
		viewer_pos.y += 0.6;
	yGridPosition++;
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