#include "utils.h"
#define WINDOW_TITLE_PREFIX "terrain-shader"

int CurrentWidth = 800,
	CurrentHeight = 600,
	WindowHandle = 0;

unsigned FrameCount = 0;

GLuint
	ProjectionMatrixUniformLocation,
	ViewMatrixUniformLocation,
	ModelMatrixUniformLocation,
	BufferIds[3] = { 0 },
	ShaderIds[3] = { 0 };

Matrix
	ProjectionMatrix,
	ViewMatrix,
	ModelMatrix;

float CubeRotation = 0;
clock_t LastTime = 0;

int 
	lastMousePosX = 0,
	lastMousePosY = 0;

float
	mouseSensitivityX = 0.2,
	mouseSensitivityY = 0.2;

void initialize(int, char*[]);
void initWindow(int, char*[]);
void resizeFunction(int, int);
void renderFunction(void);
void timerFunction(int);
void idleFunction(void);
void createCube(void);
void destroyCube(void);
void drawCube(void);
void keyboardFunction(unsigned char, int, int);
void mouseButton(int, int, int, int);
void mouseDrag(int, int);

int main(int argc, char* argv[]) {
	initialize(argc, argv);

	glutMainLoop();
	
	exit(EXIT_SUCCESS);
}

void initialize(int argc, char* argv[]) {
	GLenum GlewInitResult;

	initWindow(argc, argv);
	
	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		fprintf(
			stderr,
			"ERROR: %s\n",
			glewGetErrorString(GlewInitResult)
		);
		exit(EXIT_FAILURE);
	}
	
	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);

	glGetError();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	exitOnGLError("ERROR: Could not set OpenGL depth testing options");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	exitOnGLError("ERROR: Could not set OpenGL culling options");

	ModelMatrix = IDENTITY_MATRIX;
	ProjectionMatrix = IDENTITY_MATRIX;
	ViewMatrix = IDENTITY_MATRIX;
	translateMatrix(&ViewMatrix, 0, 0, -2);

	createCube();
}

void initWindow(int argc, char* argv[]) {
	glutInit(&argc, argv);
	
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);
	
	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

	if(WindowHandle < 1) {
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
		);
		exit(EXIT_FAILURE);
	}
	
	glutReshapeFunc(resizeFunction);
	glutDisplayFunc(renderFunction);
	glutIdleFunc(idleFunction);
	glutTimerFunc(0, timerFunction, 0);
	glutCloseFunc(destroyCube);
	glutKeyboardFunc(keyboardFunction);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseDrag);
}

void keyboardFunction(unsigned char Key, int X, int Y)
{
	switch (Key) {
	case 'w':
		translateMatrix(&ViewMatrix, 0, 0, 0.1);
		break;
	case 's':
		translateMatrix(&ViewMatrix, 0, 0, -0.1);
		break;
	case 'a':
		translateMatrix(&ViewMatrix, 0.1, 0, 0);
		break;
	case 'd':
		translateMatrix(&ViewMatrix, -0.1, 0, 0);
		break;
	default:
		break;
	}
}

/* MOUSE: check on mouse button pressed to save the position of the mouse when a button was pressed */
void mouseButton(int button, int state, int x, int y){
	if(state == GLUT_DOWN){
		lastMousePosX = x;
		lastMousePosY = y;
	}
}

/* MOUSE: calculate mouse movement and set global variables */
void mouseDrag(int x, int y){
	// calculate deltas (= how far has the mouse moved since the last call)
	int deltaX = x - lastMousePosX;
	int deltaY = y - lastMousePosY;

	// x and y are swapped because 2d screen x, y are different from 3d x, y
	rotateAboutX(&ViewMatrix, degreesToRadians(-deltaY * mouseSensitivityY));
	rotateAboutY(&ViewMatrix, degreesToRadians(deltaX * mouseSensitivityX));

	// set lastMousePos to the current values
	lastMousePosX = x;
	lastMousePosY = y;
}

void resizeFunction(int Width, int Height) {
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
	ProjectionMatrix =
		CreateProjectionMatrix(
			60,
			(float)CurrentWidth / CurrentHeight,
			1.0f,
			100.0f
		);

	glUseProgram(ShaderIds[0]);
	glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, ProjectionMatrix.m);
	glUseProgram(0);
}

void renderFunction(void) {
	++FrameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawCube();
	
	glutSwapBuffers();
	glutPostRedisplay();
}

void idleFunction(void) {
	glutPostRedisplay();
}

void timerFunction(int Value) {
	if (0 != Value) {
		char* TempString = (char*)
			malloc(512 + strlen(WINDOW_TITLE_PREFIX));

		sprintf(
			TempString,
			"%s: %d Frames Per Second @ %d x %d",
			WINDOW_TITLE_PREFIX,
			FrameCount * 4,
			CurrentWidth,
			CurrentHeight
		);

		glutSetWindowTitle(TempString);
		free(TempString);
	}

	FrameCount = 0;
	glutTimerFunc(250, timerFunction, 1);
}

void createCube() {
	const Vertex VERTICES[8] =
	{
		{ { -.5f, -.5f,  .5f, 1 }, { 0, 0, 1, 1 } },
		{ { -.5f,  .5f,  .5f, 1 }, { 1, 0, 0, 1 } },
		{ {  .5f,  .5f,  .5f, 1 }, { 0, 1, 0, 1 } },
		{ {  .5f, -.5f,  .5f, 1 }, { 1, 1, 0, 1 } },
		{ { -.5f, -.5f, -.5f, 1 }, { 1, 1, 1, 1 } },
		{ { -.5f,  .5f, -.5f, 1 }, { 1, 0, 0, 1 } },
		{ {  .5f,  .5f, -.5f, 1 }, { 1, 0, 1, 1 } },
		{ {  .5f, -.5f, -.5f, 1 }, { 0, 0, 1, 1 } }
	};

	const GLuint INDICES[36] =
	{
		0,2,1,  0,3,2,
		4,3,0,  4,7,3,
		4,1,5,  4,0,1,
		3,6,2,  3,7,6,
		1,6,5,  1,2,6,
		7,5,6,  7,4,5
	};

	ShaderIds[0] = glCreateProgram();
	exitOnGLError("ERROR: Could not create the shader program");
	{
		ShaderIds[1] = loadShader("./src/shader.frag", GL_FRAGMENT_SHADER);
		ShaderIds[2] = loadShader("./src/shader.vert", GL_VERTEX_SHADER);
		glAttachShader(ShaderIds[0], ShaderIds[1]);
		glAttachShader(ShaderIds[0], ShaderIds[2]);
	}
	glLinkProgram(ShaderIds[0]);
	exitOnGLError("ERROR: Could not link the shader program");

	ModelMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
	exitOnGLError("ERROR: Could not get shader uniform locations");

	glGenVertexArrays(1, &BufferIds[0]);
	exitOnGLError("ERROR: Could not generate the VAO");
	glBindVertexArray(BufferIds[0]);
	exitOnGLError("ERROR: Could not bind the VAO");

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	exitOnGLError("ERROR: Could not enable vertex attributes");

	glGenBuffers(2, &BufferIds[1]);
	exitOnGLError("ERROR: Could not generate the buffer objects");

	glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
	exitOnGLError("ERROR: Could not bind the VBO to the VAO");

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)sizeof(VERTICES[0].Position));
	exitOnGLError("ERROR: Could not set VAO attributes");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
	exitOnGLError("ERROR: Could not bind the IBO to the VAO");

	glBindVertexArray(0);
}

void destroyCube() {
	glDetachShader(ShaderIds[0], ShaderIds[1]);
	glDetachShader(ShaderIds[0], ShaderIds[2]);
	glDeleteShader(ShaderIds[1]);
	glDeleteShader(ShaderIds[2]);
	glDeleteProgram(ShaderIds[0]);
	exitOnGLError("ERROR: Could not destroy the shaders");

	glDeleteBuffers(2, &BufferIds[1]);
	glDeleteVertexArrays(1, &BufferIds[0]);
	exitOnGLError("ERROR: Could not destroy the buffer objects");
}

void drawCube(void) {
	float CubeAngle;
	clock_t Now = clock();

	if (LastTime == 0)
		LastTime = Now;

	CubeRotation += 45.0f * ((float)(Now - LastTime) / CLOCKS_PER_SEC);
	CubeAngle = degreesToRadians(CubeRotation);
	LastTime = Now;

	ModelMatrix = IDENTITY_MATRIX;
	rotateAboutY(&ModelMatrix, CubeAngle);
	rotateAboutX(&ModelMatrix, CubeAngle);

	glUseProgram(ShaderIds[0]);
	exitOnGLError("ERROR: Could not use the shader program");

	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
	glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, ViewMatrix.m);
	exitOnGLError("ERROR: Could not set the shader uniforms");

	glBindVertexArray(BufferIds[0]);
	exitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
	exitOnGLError("ERROR: Could not draw the cube");

	glBindVertexArray(0);
	glUseProgram(0);
}
