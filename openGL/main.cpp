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
mat4 model_view;
mat4  projection;
const vec3 viewer_pos(0.0, 0.0, 1.0);
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

const int NumVertices = 856200;
int mode = 0; //the drawing mode
int Index = 0;
int xRotation = 0;
int yRotation = 0;

point4 points[NumVertices];
color4 colors[NumVertices];
vec3   normals[NumVertices];
vec2   texture[NumVertices];
std::vector<point4> offVertices; //the vector that holds vertices loaded from the .off file
std::vector<vec3> shapeNormal; //stores the normals of the shapeX
std::vector<vec3> shapeQuad; //stores the faces of shapeX
std::vector<vec2> texCoords; //stores the texture coordinates of shapeX

								 // Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5,  0.5, 1.0),
	point4(-0.5,  0.5,  0.5, 1.0),
	point4(0.5,  0.5,  0.5, 1.0),
	point4(0.5, -0.5,  0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5,  0.5, -0.5, 1.0),
	point4(0.5,  0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};



color4 main_colour = color4(0.0, 0.0, 0.0, 1.0);

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int  Axis = Yaxis;
bool expand = true;
bool wireframe = false;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
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
	// Initialize colors
	if (mode == 0) {
		vec4 u = vertices[b] - vertices[a];
		vec4 v = vertices[c] - vertices[b];

		vec3 normal = normalize(cross(u, v));
		normals[Index] = normal; colors[Index] = main_colour; points[Index] = vertices[a]; Index++;
		normals[Index] = normal; colors[Index] = main_colour; points[Index] = vertices[b]; Index++;
		normals[Index] = normal; colors[Index] = main_colour; points[Index] = vertices[c]; Index++;
		normals[Index] = normal; colors[Index] = main_colour; points[Index] = vertices[a]; Index++;
		normals[Index] = normal; colors[Index] = main_colour; points[Index] = vertices[c]; Index++;
		normals[Index] = normal; colors[Index] = main_colour; points[Index] = vertices[d]; Index++;

	}
	else if (mode == 1 || mode == 2 ) { //for .off files a more simplified version of quad is called, this generates one triangle instead of two, that is why it is called twice as often.
		vec4 u = offVertices[b] - offVertices[a];
		vec4 v = offVertices[c] - offVertices[b];

		vec3 normal = normalize(cross(u, v));
		normals[Index] = normal; colors[Index] = main_colour; points[Index] = offVertices[a]; Index++;
		normals[Index] = normal; colors[Index] = main_colour; points[Index] = offVertices[b]; Index++;
		normals[Index] = normal; colors[Index] = main_colour; points[Index] = offVertices[c]; Index++;
	}
	else if (mode == 3) { //for loading of shapeX texture coordinates and normals are also put in their respective arrays
		texture[Index] = texCoords[a]; normals[Index] = shapeNormal[a]; colors[Index] = main_colour; points[Index] = offVertices[a]; Index++;
		texture[Index] = texCoords[b]; normals[Index] = shapeNormal[b]; colors[Index] = main_colour; points[Index] = offVertices[b]; Index++;
		texture[Index] = texCoords[c]; normals[Index] = shapeNormal[c]; colors[Index] = main_colour; points[Index] = offVertices[c]; Index++;
	}
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void loadSphere() {//method for loading a sphere from the sphere.off file
	if (mode == 1) {
		std::ifstream infile("sphere.off");
		std::string line;
		int vertexCount = 0; //the number of vertices
		int facesCount = 0; //the number of faces
		char *b;
		int a = 1;
		int counter = 0;
		int indices[3];
		std::getline(infile, line);
		if (line.compare("OFF") == 0) { //check if it is an .off file
			std::getline(infile, line); //gets the second line
			char tab2[1024];
			strncpy(tab2, line.c_str(), sizeof(tab2));
			b = strtok(tab2, " ");//tokenizes the string and gets the first two integers and stores them in the variables mentioned above
			while (b != NULL)
			{
				int value = std::stoi(b);
				if (a == 1) {
					vertexCount = value;
					a++;
				}
				else if (a == 2) {
					facesCount = value;
					a++;
				}
				else
					a = 1;
				b = strtok(NULL, " ");
			}
			while (std::getline(infile, line)) {
				istringstream is(line);
				string part;
				char s1[10], s2[10], s3[10], s4[10];
				if (counter < vertexCount) {// ignores the white space and pushes the vertices into the vector
					is >> std::skipws;
					is >> s1 >> std::ws >> s2 >> std::ws >> s3;
					GLfloat x = atof(s1) / 2;
					GLfloat y = atof(s2) / 2;
					GLfloat z = atof(s3) / 2;
					offVertices.push_back(point4(x, y, z, 1.0));
					counter++;

				}
				else if (counter >= vertexCount) {//after all the vertices have been scaned it calls the quad function to generate the triangles based on the .off data
					is >> std::skipws;
					is >> s1 >> std::ws >> s2 >> std::ws >> s3 >> std::ws >> s4;
					int x = atoi(s2);
					int y = atoi(s3);
					int z = atoi(s4);
					quad(x, y, z, 0);
					counter++;
				}
			}
		}
	}
}

void loadCar() { //works the same as the loadSphere function
	if (mode == 2) {
		std::ifstream infile2("car.off");
		std::string line2;
		int vertexCount2 = 0;
		int facesCount2 = 0;
		GLfloat max = 0;
		char *b2;
		int a2 = 1;
		int counter2 = 0;
		std::getline(infile2, line2);
		if (line2.compare("OFF") == 0) {
			std::getline(infile2, line2);
			char tab2[1024];
			strncpy(tab2, line2.c_str(), sizeof(tab2));
			b2 = strtok(tab2, " ");
			while (b2 != NULL)
			{
				int value = std::stoi(b2);
				if (a2 == 1) {
					vertexCount2 = value;
					a2++;
				}
				else if (a2 == 2) {
					facesCount2 = value;
					a2++;
				}
				else
					a2 = 1;
				b2 = strtok(NULL, " ");
			}
			while (std::getline(infile2, line2)) {
				istringstream is(line2);
				string part;
				char s1[10], s2[10], s3[10], s4[10];
				if (counter2 < vertexCount2) {
					is >> std::skipws;
					is >> s1 >> std::ws >> s2 >> std::ws >> s3;
					GLfloat x = atof(s1) / (77 * 2);
					GLfloat y = -0.2 + atof(s2) / (77 * 2);
					GLfloat z = atof(s3) / (77 * 2);
					if (abs(x) > max)
						max = abs(x);
					if (abs(y) > max)
						max = abs(y);
					if (abs(z) > max)
						max = abs(z);
					offVertices.push_back(point4(x, y, z, 1.0));
					counter2++;
				}

				//cancelled function to determine the farthest vertex and normalize the model. It was buggy
				/*else if(counter2 == vertexCount2) {
				for (int i = 0; i < offVertices.size(); i++) {
				offVertices[i].x = offVertices[i].x / (max * 2);
				offVertices[i].y = -0.2 + offVertices[i].y / (max *2);
				offVertices[i].z = offVertices[i].z / (max * 2);
				printf("%f %f %f \n", offVertices[i].x, offVertices[i].y, offVertices[i].z);
				}
				counter2++;
				}*/
				else if (counter2 >= vertexCount2) {
					is >> std::skipws;
					is >> s1 >> std::ws >> s2 >> std::ws >> s3 >> std::ws >> s4;
					int x = atoi(s2);
					int y = atoi(s3);
					int z = atoi(s4);
					quad(x, y, z, 0);
					counter2++;
				}
			}
		}
	}
}

void loadShapeX() {//method for loading a sphere from the sphere.off file
	if (mode == 3) {
		std::ifstream infile("shapeX.offx");
		std::string line;
		int vertexCount = 0; //the number of vertices
		int facesCount = 0; //the number of faces
		char *b;
		int a = 1;
		int counter = 0;
		int log = 0;
		int indices[3];
		std::getline(infile, line);
		if (line.compare("OFFX") == 0) { //check if it is an .off file
			std::getline(infile, line); //gets the second line
			char tab2[1024];
			strncpy(tab2, line.c_str(), sizeof(tab2));
			b = strtok(tab2, " ");//tokenizes the string and gets the first two integers and stores them in the variables mentioned above
			while (b != NULL)
			{
				int value = std::stoi(b);
				if (a == 1) {
					vertexCount = value;
					a++;
				}
				else if (a == 2) {
					facesCount = value;
					a++;
				}
				else
					a = 1;
				b = strtok(NULL, " ");
			}
			while (std::getline(infile, line)) {
				istringstream is(line);
				string part;
				char s1[10], s2[10], s3[10], s4[10];
				if (counter < vertexCount) {// ignores the white space and pushes the vertices into the vector
					is >> std::skipws;
					is >> s1 >> std::ws >> s2 >> std::ws >> s3;
					GLfloat x = atof(s1) ;
					GLfloat y = atof(s2) ;
					GLfloat z = atof(s3) ;
					offVertices.push_back(point4(x, y, z, 1.0));
					counter++;

				}
				else if (counter >= vertexCount && counter < facesCount+vertexCount) {//after all the vertices have been scaned it calls the quad function to generate the triangles based on the .off data
					is >> std::skipws;
					is >> s1 >> std::ws >> s2 >> std::ws >> s3 >> std::ws >> s4;
					int x = atoi(s2);
					int y = atoi(s3);
					int z = atoi(s4);
					shapeQuad.push_back(vec3(x, y, z));
					counter++;
				}
				else if(counter >= facesCount + vertexCount) {
					is >> std::skipws;
					is >> s1 >> std::ws >> s2 >> std::ws >> s3 >> std::ws >> s4;
					char compare[] = "vn";
					char compare2[] = "vt";
					if (std::strcmp(s1, compare) == 0) {//if the beginning of the line is vn then the floats go to the shapeNormal vector
						float x = atof(s2);
						float y = atof(s3);
						float z = atof(s4);
						counter++;
						shapeNormal.push_back(vec3(x, y, z));
					}
					else if (std::strcmp(s1, compare2) == 0) {//if the beginning of the line is vt then the floats go to the texCoords vector
						GLfloat s = atof(s2);
						GLfloat t = atof(s3);
						counter++;
						log++;
						texCoords.push_back(vec2(s,1-t));
					}
				}
				
			}
		}
		for (int i = 0; i < facesCount; i++) { //quad is called as much as the face count
			quad(shapeQuad[i].x, shapeQuad[i].y, shapeQuad[i].z, 0);
		}
	}
}


//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	if (mode == 1) { //selects the model to load when initalized
		loadSphere();
		Index = 0;
	}
	else if (mode == 2) {
		loadCar();
		Index = 0;
	}
	else if (mode == 0) {
		colorcube();
		Index = 0;
	}
	else if (mode == 3) {
		loadShapeX();
		Index = 0;
		int k, n, m;//the functions required to read a ppm file
		char c, b[100];
		FILE *fd = fopen("texture.ppm", "r");
		fscanf(fd, "%[^\n] ", b);
		fscanf(fd, "%c", &c);
		while (c == '#') {
			fscanf(fd, "%[^\n] ", b);
			fscanf(fd, "%c", &c);
		}
		ungetc(c, fd);
		fscanf(fd, "%d %d %d", &n, &m, &k);
		GLubyte *image = (GLubyte *)malloc(3 * sizeof(GLubyte) * n * m);
		int red, green, blue;
		for (unsigned int i = n * m; i > 0; i--) {
			fscanf(fd, "%d %d %d", &red, &green, &blue);
			image[3 * n * m - 3 * i] = red;
			image[3 * n * m - 3 * i + 1] = green;
			image[3 * n * m - 3 * i + 2] = blue;
		}
		GLuint textures[1];
		glGenTextures(1, textures);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, n, m, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

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
		projection = Ortho(-1.0, 1.0, -1.0, 1.0, -2.0, 2.0); // Ortho(): user-defined function in mat.h
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
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	glutSwapBuffers();

}

//----------------------------------------------------------------------------


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
	}
}

void processSpecialKeys(int key, int x, int y) { //controls the speed

	switch (key) {
	case GLUT_KEY_UP:

		Theta[0] += 1;

		if (Theta[0] > 360.0) {
			Theta[0] -= 360.0;
		}
		break;
	case GLUT_KEY_DOWN:
		Theta[0] -= 1;

		if (Theta[0] > 360.0) {
			Theta[0] -= 360.0;
		}
		break;
	case GLUT_KEY_RIGHT:
		Theta[1] += 1;

		if (Theta[1] > 360.0) {
			Theta[1] -= 360.0;
		}
		break;
	case GLUT_KEY_LEFT:
		Theta[1] -= 1;

		if (Theta[1] > 360.0) {
			Theta[1] -= 360.0;
		}
		break;

	}
}


	

void GoMenu(int value) { //controls the wireframe
	switch (value) {
	case 1:
		if (!wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframe = true;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe = false;
		}
		break;


	}
	glutPostRedisplay();
}

void c_menu(int value) { //controls the colour of the object
	switch (value) {
	case 1:
		main_colour = color4(1.0, 0.0, 0.0, 1.0);
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 2:
		main_colour = color4(0.0, 1.0, 0.0, 1.0);
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 3:
		main_colour = color4(0.0, 0.0, 1.0, 1.0);
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 4:
		main_colour = color4(0.0, 1.0, 1.0, 1.0);
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 5:
		main_colour = color4(1.0, 0.0, 1.0, 1.0);
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 6:
		main_colour = color4(1.0, 1.0, 0.0, 1.0);
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 7:
		main_colour = color4(1.0, 1.0, 1.0, 1.0);
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 8:
		main_colour = color4(0.0, 0.0, 0.0, 1.0);
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	}
}
void s_menu(int value) { //clears the vector and points array and chooses the new object
	switch (value) {
	case 1:
		mode = 0;
		offVertices.clear();
		for (int x = 0; x < NumVertices; x++) {
			points[x] = NULL;
			normals[x] = NULL;
		}
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 2:
		mode = 1;
		offVertices.clear();
		for (int x = 0; x < NumVertices; x++) {
			points[x] = NULL;
			normals[x] = NULL;
		}
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 3:
		mode = 2;
		offVertices.clear();
		for (int x = 0; x < NumVertices; x++) {
			points[x] = NULL;
			normals[x] = NULL;
		}
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	case 4:
		mode = 3;
		offVertices.clear();
		for (int x = 0; x < NumVertices; x++) {
			points[x] = NULL;
			normals[x] = NULL;
		}
		init();
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
		glutSwapBuffers();

		break;
	}
}

void b_menu(int value) { //changes the background colour
	switch (value) {
	case 1:
		glClearColor(0.0, 0.0, 0.0, 1.0);
		break;
	case 2:
		glClearColor(0.5, 0.5, 0.5, 1.0);
		break;
	case 3:
		glClearColor(1.0, 1.0, 1.0, 1.0);
		break;
	}
}

void p_menu(int value) { //changes the projection type
	switch (value) {
	case 1:
		isOrtho = true;
		projection = Ortho(-1.0, 1.0, -1.0, 1.0, -2.0, 2.0); 
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
		break;
	case 2:
		isOrtho = false;
		projection = Perspective( 45.0, 1.0, 0.5, 6.0 );
		glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
		break;
	}
}

void l_menu(int value) { //changes the light type
	switch (value) {
	case 1:
		glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
		isPoint = true;
		break;
	case 2:
		
		glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position2);
		isPoint = false;
		break;
	}
}

void sh_menu(int value) { //changes the shininess of the object
	switch (value) {
	case 1:
		glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
		isMetal = true;
		break;
	case 2:
		glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess2);
		isMetal = false;
		break;
	}
}

void off_menu(int value) { //turns the light off or lets the user to turn on a light
	switch (value) {
	case 1:
		ambient_product = (0.0, 0.0, 0.0, 0.0);
		diffuse_product = (0.0, 0.0, 0.0, 0.0);
		specular_product = (0.0, 0.0, 0.0, 0.0);

		glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
		glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
		glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
		break;
	case 2:
		ambient_product = light_ambient * material_ambient;
		diffuse_product = light_diffuse * material_diffuse; 
		specular_product = light_specular * material_specular;
		isPoint = true;

		glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
		glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
		glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
		glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
		break;
	case 3:
		ambient_product = light_ambient * material_ambient;
		diffuse_product = light_diffuse * material_diffuse; 
		specular_product = light_specular * material_specular;
		isPoint = false;

		glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
		glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
		glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
		glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position2);
		break;
	}
}

void t_menu(int value) { //changes the shading type
	switch (value) {
	case 1:
		type = 1;
		glUniform1i(glGetUniformLocation(program, "type"), type);
		break;
	case 2:
		type = 0;
		glUniform1i(glGetUniformLocation(program, "type"), type);
		break;
	}
}

void i_menu(int value) { //changes the illumination type
	switch (value) {
	case 1:
		shading = 0;
		glUniform1i(glGetUniformLocation(program, "shading"), shading);
		break;
	case 2:
		shading = 1;
		glUniform1i(glGetUniformLocation(program, "shading"), shading);
		break;
	}
}

void d_menu(int value) { //changes the display type to wireframe, shading or texture
	switch (value) {
	case 1:
		textureFlag = 0;
		glUniform1i(glGetUniformLocation(program, "TextureFlag"), textureFlag);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);		
		break;
	case 2:
		textureFlag = 0;
		glUniform1i(glGetUniformLocation(program, "TextureFlag"), textureFlag);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 3:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		textureFlag = 1;
		glUniform1i(glGetUniformLocation(program, "TextureFlag"), textureFlag);
		break;
	}
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


	glutDisplayFunc(display); // set display callback function
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(processSpecialKeys);

	int colour = glutCreateMenu(c_menu);

	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Magenta", 5);
	glutAddMenuEntry("Yellow", 6);
	glutAddMenuEntry("White", 7);
	glutAddMenuEntry("Black", 8);



	int shape = glutCreateMenu(s_menu);

	glutAddMenuEntry("Cube", 1);
	glutAddMenuEntry("Sphere", 2);
	glutAddMenuEntry("Car", 3);
	glutAddMenuEntry("ShapeX", 4);

	int proj = glutCreateMenu(p_menu);

	glutAddMenuEntry("Orthographic", 1);
	glutAddMenuEntry("Perspective", 2);

	int background = glutCreateMenu(b_menu);

	glutAddMenuEntry("Black", 1);
	glutAddMenuEntry("Grey", 2);
	glutAddMenuEntry("White", 3);

	int light = glutCreateMenu(l_menu);

	glutAddMenuEntry("Point", 1);
	glutAddMenuEntry("Directional", 2);

	int shiny = glutCreateMenu(sh_menu);

	glutAddMenuEntry("Metal", 1);
	glutAddMenuEntry("Plastic", 2);

	int offon = glutCreateMenu(off_menu);

	glutAddMenuEntry("Lights Off", 1);
	glutAddMenuEntry("Lights On / point", 2);
	glutAddMenuEntry("Lights On / directional", 3);

	int type = glutCreateMenu(t_menu);

	glutAddMenuEntry("Phong", 1);
	glutAddMenuEntry("Gouraud", 2);

	int display = glutCreateMenu(d_menu);

	glutAddMenuEntry("Wireframe", 1);
	glutAddMenuEntry("Shading", 2);
	glutAddMenuEntry("Texture", 3);

	int illumination = glutCreateMenu(i_menu);

	glutAddMenuEntry("Phong", 1);
	glutAddMenuEntry("Modified Phong", 2);

	glutCreateMenu(GoMenu);
	glutAddSubMenu("Shape", shape);
	glutAddSubMenu("Colour", colour);
	glutAddSubMenu("Background Colour", background);
	glutAddSubMenu("Projection", proj);
	glutAddSubMenu("Light Type", light);
	glutAddSubMenu("Material Shininess", shiny);
	glutAddSubMenu("Lights Off/On", offon);
	glutAddSubMenu("Shading Option", type);
	glutAddSubMenu("Display Mode", display);
	glutAddSubMenu("Illumination Model", illumination);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}