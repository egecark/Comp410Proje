#include "Angel.h"
typedef vec4  color4;
typedef vec4  point4;


GLfloat cubeLengthHalf = 0.3;
GLfloat groundLenghtHalf = 10 * cubeLengthHalf;
GLfloat cubeStartingPosY = cubeLengthHalf * 10 * 2;
GLfloat groundPosY = -cubeStartingPosY;

point4 vertices_pos1X[8] = {
	point4(-cubeLengthHalf*2 + 0.6, cubeStartingPosY - 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2 + 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf,  0, 1.0),
	point4(0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf,  0, 1.0),
	point4(0.6, cubeStartingPosY - 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2 + 0.6, cubeStartingPosY - 0.6, -cubeLengthHalf*2, 1.0),
	point4(-cubeLengthHalf*2 + 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4(0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4(0.6, cubeStartingPosY - 0.6, -cubeLengthHalf*2, 1.0)
};

point4 vertices_pos2X[8] = {
	point4(-cubeLengthHalf*2 + 1.2, cubeStartingPosY - 0.6, 0, 1.0),
	point4(-cubeLengthHalf*2 + 1.2, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, 0, 1.0),
	point4(1.2, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, 0, 1.0),
	point4(1.2, cubeStartingPosY - 0.6, 0, 1.0),
	point4(-cubeLengthHalf*2 + 1.2, cubeStartingPosY - 0.6, -cubeLengthHalf*2, 1.0),
	point4(-cubeLengthHalf*2 + 1.2, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4(1.2, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4(1.2, cubeStartingPosY - 0.6, -cubeLengthHalf*2, 1.0)
};

point4 vertices_neg1X[8] = {
	point4(-cubeLengthHalf*2 - 0.6, cubeStartingPosY - 0.6, 0, 1.0),
	point4(-cubeLengthHalf*2 - 0.6, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, 0, 1.0),
	point4(- 0.6, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, 0, 1.0),
	point4( - 0.6, cubeStartingPosY - 0.6, 0, 1.0),
	point4(-cubeLengthHalf*2 - 0.6, cubeStartingPosY - 0.6, -cubeLengthHalf*2, 1.0),
	point4(-cubeLengthHalf*2 - 0.6, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4( - 0.6, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4( - 0.6, cubeStartingPosY - 0.6, -cubeLengthHalf*2, 1.0)
};

point4 vertices_neg2X[8] = {
	point4(-cubeLengthHalf*2 - 1.2, cubeStartingPosY - 0.6, 0, 1.0),
	point4(-cubeLengthHalf*2 - 1.2, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, 0, 1.0),
	point4( - 1.2, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, 0, 1.0),
	point4( - 1.2, cubeStartingPosY - 0.6, 0, 1.0),
	point4(-cubeLengthHalf*2 - 1.2, cubeStartingPosY - 0.6, -cubeLengthHalf*2, 1.0),
	point4(-cubeLengthHalf*2 - 1.2, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4( - 1.2, cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2, 1.0),
	point4( - 1.2, cubeStartingPosY - 0.6, -cubeLengthHalf*2, 1.0)
};

point4 vertices_pos1Y[8] = {
	point4(-cubeLengthHalf*2, cubeStartingPosY - 0.6 + 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf + 0.6,  0, 1.0),
	point4(0,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf + 0.6,  0, 1.0),
	point4(0, cubeStartingPosY - 0.6 + 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2, cubeStartingPosY - 0.6 + 0.6, -cubeLengthHalf*2, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf + 0.6, -cubeLengthHalf*2, 1.0),
	point4(0,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf + 0.6, -cubeLengthHalf*2, 1.0),
	point4(0, cubeStartingPosY - 0.6 + 0.6, -cubeLengthHalf*2, 1.0)
};

point4 vertices_neg1Y[8] = {
	point4(-cubeLengthHalf*2, cubeStartingPosY - 0.6 - 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf - 0.6,  0, 1.0),
	point4(0,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf - 0.6,  0, 1.0),
	point4(0, cubeStartingPosY - 0.6 - 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2, cubeStartingPosY - 0.6 - 0.6, -cubeLengthHalf*2, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf - 0.6, -cubeLengthHalf*2, 1.0),
	point4(0,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf - 0.6, -cubeLengthHalf*2, 1.0),
	point4(0, cubeStartingPosY - 0.6 - 0.6, -cubeLengthHalf*2, 1.0)
};

point4 vertices_pos1X1Y[8] = {
	point4(-cubeLengthHalf*2 + 0.6, cubeStartingPosY - 0.6 + 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2 + 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf + 0.6,  0, 1.0),
	point4(0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf + 0.6,  0, 1.0),
	point4(0.6, cubeStartingPosY - 0.6 + 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2 + 0.6, cubeStartingPosY - 0.6 + 0.6, -cubeLengthHalf*2, 1.0),
	point4(-cubeLengthHalf*2 + 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf + 0.6, -cubeLengthHalf*2, 1.0),
	point4(0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf + 0.6, -cubeLengthHalf*2, 1.0),
	point4(0.6, cubeStartingPosY - 0.6 + 0.6, -cubeLengthHalf*2, 1.0)
};

point4 vertices_neg1X1Y[8] = {
	point4(-cubeLengthHalf*2 - 0.6, cubeStartingPosY - 0.6 - 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2 - 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf - 0.6,  0, 1.0),
	point4( - 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf - 0.6,  0, 1.0),
	point4( - 0.6, cubeStartingPosY - 0.6 - 0.6,  0, 1.0),
	point4(-cubeLengthHalf*2 - 0.6, cubeStartingPosY - 0.6 - 0.6, -cubeLengthHalf*2, 1.0),
	point4(-cubeLengthHalf*2 - 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf - 0.6, -cubeLengthHalf*2, 1.0),
	point4( - 0.6,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf - 0.6, -cubeLengthHalf*2, 1.0),
	point4( - 0.6, cubeStartingPosY - 0.6 - 0.6, -cubeLengthHalf*2, 1.0)
};

point4 vertices_neg1Z[8] = {
	point4(-cubeLengthHalf*2, cubeStartingPosY - 0.6,   -0.6, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf,   - 0.6, 1.0),
	point4(0,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf,   - 0.6, 1.0),
	point4(0, cubeStartingPosY - 0.6,   - 0.6, 1.0),
	point4(-cubeLengthHalf*2, cubeStartingPosY - 0.6, -cubeLengthHalf*2 - 0.6, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2 - 0.6, 1.0),
	point4(0,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2 - 0.6, 1.0),
	point4(0, cubeStartingPosY - 0.6, -cubeLengthHalf*2 - 0.6, 1.0)
};

point4 vertices_pos1Z[8] = {
	point4(-cubeLengthHalf*2, cubeStartingPosY - 0.6,  0 + 0.6, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf,  0 + 0.6, 1.0),
	point4(0,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf,  0 + 0.6, 1.0),
	point4(0, cubeStartingPosY - 0.6,  0 + 0.6, 1.0),
	point4(-cubeLengthHalf*2, cubeStartingPosY - 0.6, -cubeLengthHalf*2 + 0.6, 1.0),
	point4(-cubeLengthHalf*2,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2 + 0.6, 1.0),
	point4(0,  cubeStartingPosY - 0.6 + 2 * cubeLengthHalf, -cubeLengthHalf*2 + 0.6, 1.0),
	point4(0, cubeStartingPosY - 0.6, -cubeLengthHalf*2 + 0.6, 1.0)
};