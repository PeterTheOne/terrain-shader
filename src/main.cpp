#include "utils.h"


GLuint
	currentWidth = 800,
	currentHeight = 600,
	windowHandle = 0;

GLuint
	projectionMatrixUniformLocation, 
	viewMatrixUniformLocation, 
	modelMatrixUniformLocation, 
	bufferIds[3] = { 0 }, 
	shaderIds[3] = { 0 }, 
	textureHandle, 
	locColourMap,
	locTerrainScale;

Matrix
	projectionMatrix, 
	viewMatrix, 
	modelMatrix;

int 
	lastMousePosX = 0, 
	lastMousePosY = 0;

float
	mouseSensitivityX = 0.2f, 
	mouseSensitivityY = 0.2f;

const int 
	plane_width = 100, 
	plane_height = 100;

const float
	tile_width = 0.5f, 
	tile_height = 0.5f;

bool lineMode = false;
GLfloat terrainScale = 10;


// ----- Initialize Functions -----

void init(int, char**);
void initGlut(int, char**);
void initGlew();
void initDevIl();
void loadImage();

// ----- Create, Draw and Destroy Functions -----

void createPlane();
void drawPlane();
void destroyPlane();

// ----- Input Functions -----

void keyboardFunction(unsigned char, int, int);
void mouseButton(int, int, int, int);
void mouseDrag(int, int);

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

	modelMatrix = IDENTITY_MATRIX;
	projectionMatrix = IDENTITY_MATRIX;
	viewMatrix = IDENTITY_MATRIX;
	translateMatrix(&viewMatrix, 0, -1, -20);

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

	locColourMap = glGetUniformLocation(shaderIds[0], "colourMap");
	locTerrainScale = glGetUniformLocation(shaderIds[0], "terrainScale");
	exitOnGLError("ERROR: locColourMap");

	// ----- loadImage -----

	loadImage();

	// ----- createPlane -----

	createPlane();
}

void initGlut(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(currentWidth, currentHeight);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("terrain-shader");

	glutIdleFunc(idleFunction);
	glutDisplayFunc(drawPlane);
	glutKeyboardFunc(keyboardFunction);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseDrag);
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

void loadImage() {
	glActiveTexture(GL_TEXTURE0);
	ILstring imageFilename = "./assets/heightmap.png";
	textureHandle = ilutGLLoadImage(imageFilename);

	// Output last image loaded properties
	// Available properties list is at: http://www-f9.ijs.si/~matevz/docs/DevIL/il/f00027.htm
	std::cout << "Image width         : " << ilGetInteger(IL_IMAGE_WIDTH)          << std::endl;
	std::cout << "Image height        : " << ilGetInteger(IL_IMAGE_HEIGHT)         << std::endl;
	std::cout << "Image bits per pixel: " << ilGetInteger(IL_IMAGE_BITS_PER_PIXEL) << std::endl;
}

// ----- Create, Draw and Destroy Functions -----

void createPlane() {
	// ----- Create Vertex Array -----

	Vertex vertices[(plane_width + 1) * (plane_height + 1)];
	for (int i = 0; i <= plane_width; i++) {
		for (int j = 0; j <= plane_height; j++) {
			float Position[4] = {i * tile_width, 0.0f, j * tile_height, 1.0f};
			float TexCoords[2] = {i / (float) plane_width, j / (float) plane_height};

			for (int k = 0; k < 4; k++) {
				vertices[i * (plane_height + 1) + j].position[k] = Position[k];
			}
			for (int k = 0; k < 2; k++) {
				vertices[i * (plane_height + 1) + j].texCoords[k] = TexCoords[k];
			}
		}
	}

	// ----- Create Index Array -----

	GLuint indices[plane_width * plane_height * 12];
	for (int i = 0; i < plane_width; i++) {
		for (int j = 0; j < plane_height; j++) {
			// first tri
			indices[(i * plane_height + j) * 12 + 0] = i * (plane_height + 1) + j;			// bottom left
			indices[(i * plane_height + j) * 12 + 1] = i * (plane_height + 1) + j + 1;		// bottom right
			indices[(i * plane_height + j) * 12 + 2] = (i + 1) * (plane_height + 1) + j;		// top left

			indices[(i * plane_height + j) * 12 + 3] = 0;
			indices[(i * plane_height + j) * 12 + 4] = 0;
			indices[(i * plane_height + j) * 12 + 5] = 0;

			// second tri
			indices[(i * plane_height + j) * 12 + 6] = (i + 1) * (plane_height + 1) + j;		// top left
			indices[(i * plane_height + j) * 12 + 7] = i * (plane_height + 1) + j + 1;		// bottom right
			indices[(i * plane_height + j) * 12 + 8] = (i + 1) * (plane_height + 1) + j + 1;	// top right

			indices[(i * plane_height + j) * 12 + 9] = 0;
			indices[(i * plane_height + j) * 12 + 10] = 0;
			indices[(i * plane_height + j) * 12 + 11] = 0;
		}
	}

	// ----- glGetUniformLocation model, view, projection -----

	// this is already done in init
	// 	modelMatrixUniformLocation = glGetUniformLocation(shaderIds[0], "ModelMatrix");
	// 	viewMatrixUniformLocation = glGetUniformLocation(shaderIds[0], "ViewMatrix");
	// 	projectionMatrixUniformLocation = glGetUniformLocation(shaderIds[0], "ProjectionMatrix");
	// 	exitOnGLError("ERROR: Could not get shader uniform locations");

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
	modelMatrix = IDENTITY_MATRIX;

	// set shader uniforms
	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, modelMatrix.m);
	glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, viewMatrix.m);
	exitOnGLError("ERROR: Could not set the shader uniforms");

	// activate terrainScale
	glUniform1f(locTerrainScale, terrainScale);

	// activate texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glUniform1i(locColourMap, 0);
	
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

	glDrawElements(GL_TRIANGLES, plane_width * plane_height * 12, GL_UNSIGNED_INT, (GLvoid*)0);
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

void keyboardFunction(unsigned char Key, int X, int Y) {
	switch (Key) {
	case 'l':
		lineMode = !lineMode;
		break;
	case 'n':
		++terrainScale;
		break;
	case 'm':
		--terrainScale;
		break;
	case 'w':
		translateMatrix(&viewMatrix,  0.0f,  0.0f,  0.1f);
		break;
	case 's':
		translateMatrix(&viewMatrix,  0.0f,  0.0f, -0.1f);
		break;
	case 'a':
		translateMatrix(&viewMatrix,  0.1f,  0.0f,  0.0f);
		break;
	case 'd':
		translateMatrix(&viewMatrix, -0.1f,  0.0f,  0.0f);
		break;
	default:
		break;
	}
}

void mouseButton(int button, int state, int x, int y) {
	if(state == GLUT_DOWN){
		lastMousePosX = x;
		lastMousePosY = y;
	}
}

void mouseDrag(int x, int y) {
	// calculate deltas (= how far has the mouse moved since the last call)
	int deltaX = x - lastMousePosX;
	int deltaY = y - lastMousePosY;

	// x and y are swapped because 2d screen x, y are different from 3d x, y
	rotateAboutX(&viewMatrix, degreesToRadians(-deltaY * mouseSensitivityY));
	rotateAboutY(&viewMatrix, degreesToRadians(deltaX * mouseSensitivityX));

	// set lastMousePos to the current values
	lastMousePosX = x;
	lastMousePosY = y;
}

// ----- Other Functions -----

void idleFunction(void) {
	glutPostRedisplay();
}

void resizeFunction(int width, int height) {
	currentWidth = width;
	currentHeight = height;

	projectionMatrix =
		CreateProjectionMatrix(
		60,
		(float)currentWidth / currentHeight,
		1.0f,
		100.0f
		);

	glUseProgram(shaderIds[0]);
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, projectionMatrix.m);
	glUseProgram(0);
}
