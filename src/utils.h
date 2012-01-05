#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string.h>
#include <math.h>
#include <time.h>

#include <GL/glew.h>		// Include GLEW, which pulls in OpenGL headers as required
#include <GL/freeglut.h>

#define ILUT_USE_OPENGL		// This MUST be defined before calling the DevIL headers or we don't get OpenGL functionality
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

static const double PI = 3.14159265358979323846;

typedef struct Vertex {
	float Position[4];
	float Color[4];
	//float TexCoords[2];
} Vertex;

typedef struct Matrix {
	float m[16];
} Matrix;

static const Matrix IDENTITY_MATRIX = { {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
} };

float cotangent(float angle);
float degreesToRadians(float degrees);
float radiansToDegrees(float radians);

Matrix multiplyMatrices(const Matrix* m1, const Matrix* m2);
void rotateAboutX(Matrix* m, float angle);
void rotateAboutY(Matrix* m, float angle);
void rotateAboutZ(Matrix* m, float angle);
void scaleMatrix(Matrix* m, float x, float y, float z);
void translateMatrix(Matrix* m, float x, float y, float z);

Matrix CreateProjectionMatrix(
	float fovy,
	float aspect_ratio,
	float near_plane,
	float far_plane
);

void exitOnGLError(const char* error_message);
void validateShader(GLuint, const char*);
void validateProgram(GLuint);
GLuint loadShader(const char* filename, GLenum shader_type);

#endif
