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
	textureHandles[2] = { 0 }, 
	locTerrainScale, 
	locInterpolationScale,
	locHeightMap, 
	locHeightMap2, 
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
	projectionMatrix, 
	viewMatrix, 
	modelMatrix;

// ----- Plane -----

const int 
	PLANE_WIDTH = 100, 
	PLANE_HEIGHT = 100, 
	PLANE_POLYGONS_PER_TILE = 2, 
	PLANE_INDICES_PER_POLYGON = 3, 
	PLANE_INDICES_PER_TILE = PLANE_POLYGONS_PER_TILE * PLANE_INDICES_PER_POLYGON, 
	PLANE_NUM_INDICES = PLANE_WIDTH * PLANE_HEIGHT * PLANE_INDICES_PER_TILE;

const float
	PLANE_TILE_WIDTH = 0.5f, 
	PLANE_TILE_HEIGHT = 0.5f;

bool lineMode = false;
GLfloat terrainScale = 10;
GLfloat interpolationScale = 0;

// ----- Camera -----

const int left = 1;
const int right = 2;
const int forward = 4;
const int backward = 8;

const float movespeed = 10;

int move = 0;

glm::vec3 position;
glm::vec3 angles;
glm::vec3 lookat;

float
	mouseSensitivityX = 0.01f, 
	mouseSensitivityY = 0.01f;

// ----- Initialize Functions -----

void init(int, char**);
void initGlut(int, char**);
void initGlew();
void initDevIl();
void loadImages();

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


// ----- Main -----

int main(int argc, char** argv) {
	init(argc, argv);

	glutMainLoop();

	return EXIT_SUCCESS;
}

// ----- Initialize Functions -----

void init(int argc, char** argv) {

	// ----- Initialize Glut (Window) -----

	initGlut(argc, argv);

	// ----- Initialize Glew -----

	initGlew();

	// ----- Window and Projection Settings -----

	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::mat4(1.0f);
	viewMatrix = glm::mat4(1.0f);
	viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, -1.0f, -20.0f));

	// ----- OpenGL settings -----

	glEnable(GL_DEPTH_TEST);                                // Enable the depth buffer
	glDepthFunc(GL_LESS);                            // Set our depth function to overwrite if new value less than or equal to current value
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Ask for nicest perspective correction

	// ----- Initialize DevIL -----

	initDevIl();

	// ----- Compile and Link Shader -----

	shaderIds[0] = glCreateProgram();
	exitOnGLError("ERROR: Could not create the shader program");

	shaderIds[1] = loadShader("./src/shader.frag", GL_FRAGMENT_SHADER);
	shaderIds[2] = loadShader("./src/shader.vert", GL_VERTEX_SHADER);
	glAttachShader(shaderIds[0], shaderIds[1]);
	glAttachShader(shaderIds[0], shaderIds[2]);

	glLinkProgram(shaderIds[0]);
	validateProgram(shaderIds[0]);
	glGetError(); // ignore link error
	//exitOnGLError("ERROR: Could not link the shader program");

	// ----- glGetUniformLocation model, view, projection -----

	modelMatrixUniformLocation = glGetUniformLocation(shaderIds[0], "ModelMatrix");
	viewMatrixUniformLocation = glGetUniformLocation(shaderIds[0], "ViewMatrix");
	projectionMatrixUniformLocation = glGetUniformLocation(shaderIds[0], "ProjectionMatrix");
	exitOnGLError("ERROR: Could not get shader uniform locations");

	locHeightMap = glGetUniformLocation(shaderIds[0], "heightMap");
	locHeightMap2 = glGetUniformLocation(shaderIds[0], "heightMap2");
	locTerrainScale = glGetUniformLocation(shaderIds[0], "terrainScale");
	locInterpolationScale = glGetUniformLocation(shaderIds[0], "interpolationScale");
	locKa = glGetUniformLocation(shaderIds[0], "Ka");
	locKd = glGetUniformLocation(shaderIds[0], "Kd");
	locGlobalAmbient = glGetUniformLocation(shaderIds[0], "globalAmbient");
	locLightColor = glGetUniformLocation(shaderIds[0], "lightColor");
	locLightPosition = glGetUniformLocation(shaderIds[0], "lightPosition");
	exitOnGLError("ERROR: Could not get shader uniform locations 2");

	// ----- loadImage -----

	loadImages();

	// ----- createPlane -----

	createPlane();
}

void initGlut(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(currentWidth, currentHeight);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("terrain-shader");

	glutSetCursor(GLUT_CURSOR_NONE);

	glutIdleFunc(idleFunction);
	glutDisplayFunc(drawPlane);
	glutKeyboardFunc(keyboardFunction);
	glutKeyboardUpFunc(keyboardUpFunction);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(mouseMove);
	glutReshapeFunc(resizeFunction);
}

void initGlew() {
	GLenum err = glewInit();
	if (GLEW_OK != err)	{
		std::cout << "GLEW initialisation error: " << glewGetErrorString(err) << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "GLEW intialised successfully. Using GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
}

void initDevIl() {
	// DevIL sanity check
	if ((iluGetInteger(IL_VERSION_NUM) < IL_VERSION) || 
			(iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION) || 
			(ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)) {
		std::cout << "DevIL versions are different... Exiting." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Initialise all DevIL functionality
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);	// Tell DevIL that we're using OpenGL for our rendering
	glGetError();				// ignore error
}

void loadImages() {
	glActiveTexture(GL_TEXTURE0);
	ILstring imageFilename = "./assets/heightmap.png";
	textureHandles[0] = ilutGLLoadImage(imageFilename);

	glActiveTexture(GL_TEXTURE1);
	ILstring imageFilename2 = "./assets/heightmap2.png";
	textureHandles[1] = ilutGLLoadImage(imageFilename2);

	// Output last image loaded properties
	// Available properties list is at: http://www-f9.ijs.si/~matevz/docs/DevIL/il/f00027.htm
	std::cout << "Image width         : " << ilGetInteger(IL_IMAGE_WIDTH)          << std::endl;
	std::cout << "Image height        : " << ilGetInteger(IL_IMAGE_HEIGHT)         << std::endl;
	std::cout << "Image bits per pixel: " << ilGetInteger(IL_IMAGE_BITS_PER_PIXEL) << std::endl;
}

// ----- Create, Draw and Destroy Functions -----

void createPlane() {
	// ----- Create Vertex Array -----

	Vertex vertices[(PLANE_WIDTH + 1) * (PLANE_HEIGHT + 1)];
	for (int i = 0; i <= PLANE_WIDTH; i++) {
		for (int j = 0; j <= PLANE_HEIGHT; j++) {
			float Position[4] = {i * PLANE_TILE_WIDTH, 0.0f, j * PLANE_TILE_HEIGHT, 1.0f};
			float TexCoords[2] = {i / (float) PLANE_WIDTH, j / (float) PLANE_HEIGHT};

			for (int k = 0; k < 4; k++) {
				vertices[i * (PLANE_HEIGHT + 1) + j].position[k] = Position[k];
			}
			for (int k = 0; k < 2; k++) {
				vertices[i * (PLANE_HEIGHT + 1) + j].texCoords[k] = TexCoords[k];
			}
		}
	}

	// ----- Create Index Array -----

	GLuint indices[PLANE_NUM_INDICES];
	for (int i = 0; i < PLANE_WIDTH; i++) {
		for (int j = 0; j < PLANE_HEIGHT; j++) {
			// first tri
			indices[(i * PLANE_HEIGHT + j) * PLANE_INDICES_PER_TILE + 0] = i * (PLANE_HEIGHT + 1) + j;				// bottom left
			indices[(i * PLANE_HEIGHT + j) * PLANE_INDICES_PER_TILE + 1] = i * (PLANE_HEIGHT + 1) + j + 1;			// bottom right
			indices[(i * PLANE_HEIGHT + j) * PLANE_INDICES_PER_TILE + 2] = (i + 1) * (PLANE_HEIGHT + 1) + j;		// top left

			// second tri
			indices[(i * PLANE_HEIGHT + j) * PLANE_INDICES_PER_TILE + 3] = (i + 1) * (PLANE_HEIGHT + 1) + j;		// top left
			indices[(i * PLANE_HEIGHT + j) * PLANE_INDICES_PER_TILE + 4] = i * (PLANE_HEIGHT + 1) + j + 1;			// bottom right
			indices[(i * PLANE_HEIGHT + j) * PLANE_INDICES_PER_TILE + 5] = (i + 1) * (PLANE_HEIGHT + 1) + j + 1;	// top right
		}
	}

	// ----- gen and bind VertexArray (Object) -----

	glGenVertexArrays(1, &bufferIds[0]);
	exitOnGLError("ERROR: Could not generate the VAO");
	glBindVertexArray(bufferIds[0]);
	exitOnGLError("ERROR: Could not bind the VAO");

	// ----- enable VertexAttribArrays -----

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	exitOnGLError("ERROR: Could not enable vertex attributes");

	// ----- gen Vertex Buffer Object and bind to Vertex Array Object -----

	glGenBuffers(2, &bufferIds[1]);
	exitOnGLError("ERROR: Could not generate the buffer objects");

	glBindBuffer(GL_ARRAY_BUFFER, bufferIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	exitOnGLError("ERROR: Could not bind the VBO to the VAO");

	// ----- set Vertex Array Object Attributes -----

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)sizeof(vertices[0].position));
	exitOnGLError("ERROR: Could not set VAO attributes");

	// ----- gen Index Buffer Object and bind to Vertex Array Object -----

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	exitOnGLError("ERROR: Could not bind the IBO to the VAO");

	// ----- unbind Vertex Array? -----

	glBindVertexArray(0);
}

void drawPlane() {
	// ----- Clear Screen and buffers ------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ----- Activate ------

	// activate shader
	glUseProgram(shaderIds[0]);
	exitOnGLError("ERROR: Could not use the shader program");

	// Reset the matrix to identity
	glMatrixMode(GL_MODELVIEW);
	modelMatrix = glm::mat4(1.0f);

	// set shader uniforms
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	exitOnGLError("ERROR: Could not set the shader uniforms");

	// activate terrainScale
	glUniform1f(locTerrainScale, terrainScale);
	glUniform1f(locInterpolationScale, interpolationScale);
	glUniform3fv(locKa, 1, glm::value_ptr(Ka));
	glUniform3fv(locKd, 1, glm::value_ptr(Kd));
	glUniform3fv(locGlobalAmbient, 1, glm::value_ptr(globalAmbient));
	glUniform3fv(locLightColor, 1, glm::value_ptr(lightColor));
	glUniform3fv(locLightPosition, 1, glm::value_ptr(lightPosition));
	exitOnGLError("ERROR: Could not set the shader uniforms2");

	// activate textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandles[0]);
	glUniform1i(locHeightMap, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureHandles[1]);
	glUniform1i(locHeightMap2, 1);
	
	// activate buffer
	glBindVertexArray(bufferIds[0]);
	exitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

	// polygon mode
	if (lineMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// ----- Draw ------

	glDrawElements(GL_TRIANGLES, PLANE_NUM_INDICES, GL_UNSIGNED_INT, (GLvoid*)0);
	exitOnGLError("ERROR: Could not draw");

	// ----- Deactivate ------

	glBindVertexArray(0);
	glUseProgram(0);

	// ----- Display ------
	glutSwapBuffers();
	glutPostRedisplay();
}

void destroyPlane() {
	glDetachShader(shaderIds[0], shaderIds[1]);
	glDetachShader(shaderIds[0], shaderIds[2]);
	glDeleteShader(shaderIds[1]);
	glDeleteShader(shaderIds[2]);
	glDeleteProgram(shaderIds[0]);
	exitOnGLError("ERROR: Could not destroy the shaders");

	glDeleteBuffers(2, &bufferIds[1]);
	glDeleteVertexArrays(1, &bufferIds[0]);
	exitOnGLError("ERROR: Could not destroy the buffer objects");
}

// ----- Input Functions -----

void keyboardFunction(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESCAPE key
		exit (0);
		break;
	case 'l':
		lineMode = !lineMode;
		break;
	case 'n':
		++terrainScale;
		break;
	case 'm':
		--terrainScale;
		break;
	case 'i': 
		if (interpolationScale < 1.0f) {
			interpolationScale += 0.1f;
			std::cout << "interpolationScale: " << interpolationScale << std::endl;
		}
		break;
	case 'o': 
		if (interpolationScale >= 0.05f) {
			interpolationScale -= 0.1f;
			std::cout << "interpolationScale: " << interpolationScale << std::endl;
		}
		break;
	case 'w': // forward
		move |= forward;
		break;
	case 's': // backward
		move |= backward;
		break;
	case 'a': // left
		move |= left;
		break;
	case 'd': // right
		move |= right;
		break;
	default:
		break;
	}

	// Redraw Scene
	glutPostRedisplay();
}

void keyboardUpFunction(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': // forward
		move &= ~forward;
		break;
	case 's': // backward
		move &= ~backward;
		break;
	case 'a': // left
		move &= ~left;
		break;
	case 'd': // right
		move &= ~right;
		break;
	default:
		break;
	}
}

void mouseMove(int x, int y) {
	static bool wrap = false;

	if(!wrap) {
		int ww = currentWidth;
		int wh = currentHeight;

		int dx = x - ww / 2;
		int dy = y - wh / 2;

		// Do something with dx and dy here
		angles.x -= dx * mouseSensitivityX;
		angles.y -= dy * mouseSensitivityY;

		if(angles.x < -glm::pi<float>())
			angles.x += glm::pi<float>() * 2;
		else if(angles.x > glm::pi<float>())
			angles.x -= glm::pi<float>() * 2;

		if(angles.y < -glm::half_pi<float>())
			angles.y = -glm::half_pi<float>();
		if(angles.y > glm::half_pi<float>())
			angles.y = glm::half_pi<float>();

		// TODO: move some stuff to idle func

		lookat.x = sinf(angles.x) * cosf(angles.y);
		lookat.y = sinf(angles.y);
		lookat.z = cosf(angles.x) * cosf(angles.y);

		viewMatrix = glm::lookAt(position, position + lookat, glm::vec3(0, 1, 0));

		// move mouse pointer back to the center of the window
		wrap = true;
		glutWarpPointer(ww / 2, wh / 2);
	} else {
		wrap = false;
	}

	// Redraw Scene
	glutPostRedisplay();
}

// ----- Other Functions -----

void idleFunction(void) {
	static int pt = 0;

	// Calculate time since last call to idle()
	int t = glutGet(GLUT_ELAPSED_TIME);
	float dt = (t - pt) * 1.0e-3;
	pt = t;

	// Calculate movement vectors
	glm::vec3 forward_dir = lookat;
	glm::vec3 right_dir = glm::vec3(-forward_dir.z, 0, forward_dir.x);


	// Update camera position
	glm::vec3 velocity = glm::vec3(0.0f);
	if(move & left)
		velocity -= right_dir;
	if(move & right)
		velocity += right_dir;
	if(move & forward)
		velocity += forward_dir;
	if(move & backward)
		velocity -= forward_dir;

	//TODO: normalize(velocity), not work?
	position += velocity * movespeed * dt;

	viewMatrix = glm::lookAt(position, position + lookat, glm::vec3(0, 1, 0));

	// Redraw Scene
	glutPostRedisplay();
}

void resizeFunction(int width, int height) {
	currentWidth = width;
	currentHeight = height;

	projectionMatrix = glm::perspective(
		60.0f, 
		(float) currentWidth / currentHeight, 
		0.1f, 
		100.0f);

	glUseProgram(shaderIds[0]);
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUseProgram(0);
}
