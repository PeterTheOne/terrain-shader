#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string.h>

#include <GL/glew.h>		// Include GLEW, which pulls in OpenGL headers as required
#include <GL/freeglut.h>

typedef struct Vertex {
	float position[4];
	float texCoords[2];
} Vertex;

void exitOnGLError(const char* error_message);
void validateShader(GLuint, const char*);
void validateProgram(GLuint);
GLuint loadShader(const char* filename, GLenum shader_type);

#endif
