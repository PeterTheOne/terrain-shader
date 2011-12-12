#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "shader.h"

Shader shader;

GLfloat angle = 0.0;

void init() {
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	shader.init("src//shader.vert", "src//shader.frag");
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	shader.bind();
	glRotatef(angle, 1.0, 0.0, 0.0);
	glRotatef(angle, 0.0, 1.0, 0.0);
	glRotatef(angle, 0.0, 0.0, 1.0);
	glColor3f(1.0, 0.0, 0.0);
	glutSolidCube(2);
	glutSolidSphere(1.2, 16, 16);
	shader.unbind();

	glutSwapBuffers();
	angle++;
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat) w / (GLfloat) h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
}

void idle() {
	//TODO: logging

	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Terrain Shader");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	init();
	glutMainLoop();

	return 0;
}






