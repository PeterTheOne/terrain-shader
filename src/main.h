#ifndef MAIN_H
#define MAIN_H

// ----- Includes -----

#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/constants.hpp>

#include <GL/glew.h>		// Include GLEW, which pulls in OpenGL headers as required
#include <GL/freeglut.h>

#define ILUT_USE_OPENGL		// This MUST be defined before calling the DevIL headers or we don't get OpenGL functionality
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "utils.h"

// ----- Functions -----

// ----- Initialize Functions -----

void init(int, char**);
void initGlut(int, char**);
void initGlew();
void setOpenGLSettings();
void initShader();
void setShaderUniformLocations();
void initDevIl();
void loadImage();

// ----- Create, Draw and Destroy Functions -----

void createPlane();
void drawPlane();
void destroyPlane();

// ----- Input Functions -----

void keyboardFunction(unsigned char, int, int);
void keyboardUpFunction(unsigned char, int, int);
void mouseMove(int, int);

// ----- Other Functions -----

void idleFunction();
void resizeFunction(int, int);

// ----- Variables -----

// ----- Window -----

GLuint
	currentWidth = 800,
	currentHeight = 600,
	windowHandle = 0;

// ----- Shader -----

GLuint
	projectionMatrixUniformLocation, 
	viewMatrixUniformLocation, 
	modelMatrixUniformLocation, 
	bufferIds[3] = { 0 }, 
	shaderIds[3] = { 0 }, 
	textureHandle, 
	locHeightMap, 
	locTerrainScale, 
	locKa, 
	locKd, 
	locGlobalAmbient, 
	locLightColor, 
	locLightPosition;

glm::vec3
	Ka = glm::vec3(1.0f, 1.0f, 1.0f), 
	Kd = glm::vec3(0.5f, 0.5f, 0.5f), 
	globalAmbient = glm::vec3(0.0f, 0.0f, 0.0f), 
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f), 
	lightPosition = glm::vec3();

glm::mat4 
	modelMatrix = glm::mat4(1.0f), 
	projectionMatrix = glm::mat4(1.0f), 
	viewMatrix = glm::mat4(1.0f);
	
// ----- Plane -----

const int 
	PLANE_WIDTH = 100, 
	PLANE_HEIGHT = 100, 
	PLANE_POLYGONS_PER_TILE = 2, 
	PLANE_INDICES_PER_POLYGON = 3, 
	PLANE_INDICES_PER_TILE = PLANE_POLYGONS_PER_TILE * PLANE_INDICES_PER_POLYGON, 
	PLANE_NUM_INDICES = PLANE_WIDTH * PLANE_HEIGHT * PLANE_INDICES_PER_TILE;

const GLfloat
	PLANE_TILE_WIDTH = 0.5f, 
	PLANE_TILE_HEIGHT = 0.5f;

bool lineMode = false;
GLfloat terrainScale = 10;

// ----- Camera -----

const int left = 1;
const int right = 2;
const int forward = 4;
const int backward = 8;

const GLfloat movespeed = 10;

int move = 0;

glm::vec3 
	position = glm::vec3(0.0f, 20.0f, 0.0f), 
	angles = glm::vec3(
		glm::quarter_pi<float>(), 
		-glm::quarter_pi<float>() / 2, 
		0), 
	lookat;

GLfloat
	mouseSensitivityX = 0.01f, 
	mouseSensitivityY = 0.01f;

#endif
