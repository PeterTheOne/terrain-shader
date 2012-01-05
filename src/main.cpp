#include "utils.h"

GLuint textureHandle;           // Create a uint to store the handle to our texture

int CurrentWidth = 800,
	CurrentHeight = 600,
	WindowHandle = 0;

GLuint
	ProjectionMatrixUniformLocation,
	ViewMatrixUniformLocation,
	ModelMatrixUniformLocation,
	BufferIds[3] = { 0 },
	ShaderIds[3] = { 0 },
	locColourMap;

Matrix
	ProjectionMatrix,
	ViewMatrix,
	ModelMatrix;

int 
	lastMousePosX = 0,
	lastMousePosY = 0;

float
	mouseSensitivityX = 0.2,
	mouseSensitivityY = 0.2;

const int 
	plane_width = 100, 
	plane_height = 100;

const float
	tile_width = 0.5f,
	tile_height = 0.5f;


void idleFunction(void);
void createPlane(void);
void destroyPlane(void);
void keyboardFunction(unsigned char, int, int);
void mouseButton(int, int, int, int);
void mouseDrag(int, int);
void resizeFunction(int, int);

void init(int width, int height)
{
	//  ----- Initialise GLEW -----
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "GLEW initialisation error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}
	std::cout << "GLEW intialised successfully. Using GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

	// ----- Window and Projection Settings -----

	ModelMatrix = IDENTITY_MATRIX;
	ProjectionMatrix = IDENTITY_MATRIX;
	ViewMatrix = IDENTITY_MATRIX;
	translateMatrix(&ViewMatrix, 0, 0, -20);

	// ----- OpenGL settings -----

	glEnable(GL_DEPTH);                                // Enable the depth buffer
	glDepthFunc(GL_LEQUAL);                            // Set our depth function to overwrite if new value less than or equal to current value
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Ask for nicest perspective correction
	glEnable(GL_TEXTURE_2D);                           // Enable 2D textures

	// ----- Initialize DevIL libraries -----

	// DevIL sanity check
	if ( (iluGetInteger(IL_VERSION_NUM) < IL_VERSION) || (iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION) || (ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) )
	{
		std::cout << "DevIL versions are different... Exiting." << std::endl;
		exit(-1);
	}

	// Initialise all DevIL functionality
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);	// Tell DevIL that we're using OpenGL for our rendering
	glGetError();

	// shader
	ShaderIds[0] = glCreateProgram();
	exitOnGLError("ERROR: Could not create the shader program");
	{
		ShaderIds[1] = loadShader("./src/shader.frag", GL_FRAGMENT_SHADER);
		ShaderIds[2] = loadShader("./src/shader.vert", GL_VERTEX_SHADER);
		glAttachShader(ShaderIds[0], ShaderIds[1]);
		glAttachShader(ShaderIds[0], ShaderIds[2]);
	}
	glLinkProgram(ShaderIds[0]);
	validateProgram(ShaderIds[0]);
	glGetError();
	exitOnGLError("ERROR: Could not link the shader program");

	ModelMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
	exitOnGLError("ERROR: Could not get shader uniform locations");

	locColourMap = glGetUniformLocation(ShaderIds[0], "colourMap"); // The texture map
	exitOnGLError("ERROR: locColourMap");

	createPlane();
}

void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen and depth buffer

	// Reset the matrix to identity
	glMatrixMode(GL_MODELVIEW);

	ModelMatrix = IDENTITY_MATRIX;

	glUseProgram(ShaderIds[0]);
	exitOnGLError("ERROR: Could not use the shader program");

	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
	glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, ViewMatrix.m);
	exitOnGLError("ERROR: Could not set the shader uniforms");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandle); // Select the texture to use and bind to it
	glUniform1i(locColourMap, 0);

	// Draw our textured geometry (just a rectangle in this instance)
	glEnable(GL_TEXTURE_2D);
	



	glBindVertexArray(BufferIds[0]);
	exitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glEnable(GL_TEXTURE_2D);
	glDrawElements(GL_TRIANGLES, plane_width * plane_height * 12, GL_UNSIGNED_INT, (GLvoid*)0);
	glDisable(GL_TEXTURE_2D);
	exitOnGLError("ERROR: Could not draw the cube");

	glBindVertexArray(0);
	glUseProgram(0);




	glDisable(GL_TEXTURE_2D);

	// ----- Stop Drawing Stuff! ------
	glutSwapBuffers();
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(CurrentWidth, CurrentHeight);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutCreateWindow("Simple Reflection");

	glutIdleFunc(idleFunction);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(keyboardFunction);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseDrag);
	glutReshapeFunc(resizeFunction);

	init(CurrentWidth, CurrentHeight);

	ILstring imageFilename = "./assets/heightmap.png";  // Specify filename

	glActiveTexture(GL_TEXTURE0);
	textureHandle = ilutGLLoadImage(imageFilename); // Load image directly to texture

	// Output last image loaded properties
	// Available properties list is at: http://www-f9.ijs.si/~matevz/docs/DevIL/il/f00027.htm
	std::cout << "Image width         : " << ilGetInteger(IL_IMAGE_WIDTH)          << std::endl;
	std::cout << "Image height        : " << ilGetInteger(IL_IMAGE_HEIGHT)         << std::endl;
	std::cout << "Image bits per pixel: " << ilGetInteger(IL_IMAGE_BITS_PER_PIXEL) << std::endl;

	glutMainLoop();

	return 0;
}

// new stuff

void idleFunction(void) {
	glutPostRedisplay();
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

void createPlane() {
	Vertex vertices[(plane_width + 1) * (plane_height + 1)];
	for (int i = 0; i <= plane_width; i++) {
		for (int j = 0; j <= plane_height; j++) {
			float Position[4] = {i * tile_width, 0.0f, j * tile_height, 1.0f};
			float Color[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
			//float TexCoords[2] = {i / (float) plane_width, j / (float) plane_height};

			for (int k = 0; k < 4; k++) {
				vertices[i * (plane_height + 1) + j].Position[k] = Position[k];
				vertices[i * (plane_height + 1) + j].Color[k] = Color[k];
				//vertices[i * (plane_height + 1) + j].TexCoords[k] = TexCoords[k];
			}
		}
	}

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
	//glEnableVertexAttribArray(2);
	exitOnGLError("ERROR: Could not enable vertex attributes");

	glGenBuffers(2, &BufferIds[1]);
	exitOnGLError("ERROR: Could not generate the buffer objects");

	glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	exitOnGLError("ERROR: Could not bind the VBO to the VAO");

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)sizeof(vertices[0].Position));
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)(sizeof(vertices[0].Position) + sizeof(vertices[0].Color)));
	exitOnGLError("ERROR: Could not set VAO attributes");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	exitOnGLError("ERROR: Could not bind the IBO to the VAO");

	glBindVertexArray(0);
}

void destroyPlane() {
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


