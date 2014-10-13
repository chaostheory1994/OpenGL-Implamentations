#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "GL.h"

#define min(a,b) ((a) < (b)? a:b)
#define FALSE 0 
#define TRUE  1

#define COUNTERCLOCKWISE  1
#define CLOCKWISE -1

#define DELTA_TIME 100
#define DELTA_DEG  5

/* define index constants into the colors array */
/* colors */
#define BLACK   0
#define RED     1
#define YELLOW  2
#define ORANGE  3
#define GREEN   4
#define BLUE    5
#define WHITE   6
#define MAGENTA 7
#define CYAN    8
#define GREY    9

GLfloat colors [][3] = {
		{ 0.0, 0.0, 0.0 },  /* black   */
		{ 1.0, 0.0, 0.0 },  /* red     */
		{ 1.0, 1.0, 0.0 },  /* yellow  */
		{ 1.0, 0.5, 0.0 },  /* orange  */
		{ 0.0, 1.0, 0.0 },  /* green   */
		{ 0.0, 0.0, 1.0 },  /* blue    */
		{ 1.0, 1.0, 1.0 },   /* white   */
		{ 1.0, 0.0, 1.0 },  /* magenta */
		{ 0.0, 1.0, 1.0 },  /* cyan    */
		{ 0.5, 0.5, 0.5 }  /* 50%grey */
};

GLfloat vertices [][3] = {
		{ -1, 1, 1 }, { 1, 1, 1 }, { 1, 1, -1 }, { -1, 1, -1 },
		{ -1, -1, 1 }, { 1, -1, 1 }, { 1, -1, -1 }, { -1, -1, -1 }
};

void glut_setup(void);
void gl_setup(void);
void my_setup(void);
void my_display(void);
void my_reshape(int w, int h);
void my_keyboard(unsigned char key, int x, int y);
void my_timer(int val);

int theta;
int bGrid;
int iRotY;
bool lookat_switch = false;
float scale;

int main(int argc, char **argv) {

	/* General initialization for GLUT and OpenGL
	Must be called first */
	glutInit(&argc, argv);

	/* we define these setup procedures */
	glut_setup();
	gl_setup();
	my_setup();

	/* go into the main event loop */
	glutMainLoop();

	return(0);
}

/* This function sets up the windowing related glut calls */
void glut_setup(void) {

	/* specify display mode -- here we ask for a double buffer and RGB coloring */
	/* NEW: tell display we care about depth now */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	/* make a 400x400 window with the title of "GLUT Skeleton" placed at the top left corner */
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Cube in Perspective View");

	/*initialize callback functions */
	glutDisplayFunc(my_display);
	glutReshapeFunc(my_reshape);
	glutKeyboardFunc(my_keyboard);
	return;
}

/* This function sets up the initial states of OpenGL related enivornment */
void gl_setup(void) {

	/* specifies a background color: black in this case */
	glClearColor(0, 0, 0, 0);

	/* NEW: now we have to enable depth handling (z-buffer) */
	glEnable(GL_DEPTH_TEST);

	/* NEW: setup for 3d projection */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// perspective view:
	// fovy = 20 degrees
	// aspect ratio is 1 (width = height
	// near plane of the view volume is at 1
	//  far plane of the view volume is at 100
	gluPerspective(20.0, 1.0, 1.0, 100.0);
	/* NEW: safe practice: initialize modelview matrix */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return;
}

void my_setup(void) {
	theta = 0;
	bGrid = FALSE;
	iRotY = 0;
	scale = 1;
	return;
}

void my_reshape(int w, int h) {

	/* define viewport -- x, y, (origin is at lower left corner) width, height */
	glViewport(0, 0, min(w, h), min(w, h));
	return;
}

void my_keyboard(unsigned char key, int x, int y) {
	float mat[16];
	switch (key) {
	case 'o':
	case 'O':
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (lookat_switch) my_ortho(-3, 3, -3, 3, 1, 100);
		else glOrtho(-3, 3, -3, 3, 1, 100);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		break;
	case 'p':
	case 'P':
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (lookat_switch) my_perspective(20.0, 1.0, 1.0, 100.0);
		else gluPerspective(20.0, 1.0, 1.0, 100.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		break;
	case 'f':
	case 'F':
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (lookat_switch) my_frustum(-3, 3, -3, 3, 1, 100);
		else glFrustum(-3, 3, -3, 3, 1, 100);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		break;
	case 'd':
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		print_mat(mat, 4, 4);
		break;
	case 'D':
		glGetFloatv(GL_PROJECTION_MATRIX, mat);
		print_mat(mat, 4, 4);
		break;
	case 'l':
	case 'L':
		lookat_switch = !lookat_switch;
		break;
	case 's':
		scale -= 0.1;
		break;
	case 'S':
		scale += 0.1;
		break;
	case 'y':
		if (iRotY == COUNTERCLOCKWISE) // turn off if already rotating counter clockwise
			iRotY = 0;
		else iRotY = COUNTERCLOCKWISE;
		break;
	case 'Y':
		if (iRotY == CLOCKWISE)
			iRotY = 0;
		else iRotY = CLOCKWISE;
		break;
	case 'q':
	case 'Q':
		exit(0);
	default: break;
	}
	glutTimerFunc(DELTA_TIME, my_timer, 0);
	return;
}

/* helper drawing function. Input are 4 indices into the vertices
array to define a face and an index into the color array.  For the
normal to work out, follow left-hand-rule (i.e., counter clock
wise) */
void make_quad(int iv1, int iv2, int iv3, int iv4, int ic) {
	glBegin(GL_POLYGON);
	{
		glColor3fv(colors[ic]);
		glVertex3fv(vertices[iv1]);
		glVertex3fv(vertices[iv2]);
		glVertex3fv(vertices[iv3]);
		glVertex3fv(vertices[iv4]);
	}
	glEnd();
}

/* defines a hollow cube (no top or bottom) */
void make_cube()
{
	make_quad(4, 5, 1, 0, BLUE);
	make_quad(5, 6, 2, 1, CYAN);
	make_quad(3, 2, 6, 7, YELLOW);
	make_quad(0, 3, 7, 4, MAGENTA);
}

void my_display(void) {
	float mat[16];
	/* clear the buffer */
	/* NEW: now we have to clear depth as well */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (lookat_switch){
		my_lookat(0.0, 5.0, 15.0,  // x,y,z coord of the camera 
			0.0, 0.0, 0.0,  // x,y,z coord of the origin
			0.0, 1.0, 0.0); // the direction of up (default is y-axis)

	}
	else{
		gluLookAt(0.0, 5.0, 15.0,  // x,y,z coord of the camera 
			0.0, 0.0, 0.0,  // x,y,z coord of the origin
			0.0, 1.0, 0.0); // the direction of up (default is y-axis)
		
	}
	if (lookat_switch) my_rotate(theta, 0, 1, 0);
	else glRotatef(theta, 0, 1, 0); // auto-rotation
	my_scale(scale, scale, scale);
	make_cube();

	/* buffer is ready */
	glutSwapBuffers();

	return;
}

void my_timer(int val) {
	int i;

	if (iRotY == COUNTERCLOCKWISE) {
		theta = (theta + DELTA_DEG) % 360;
		glutTimerFunc(DELTA_TIME, my_timer, 0);
	}
	else if (iRotY == CLOCKWISE) {
		theta = (theta - DELTA_DEG) % 360;
		glutTimerFunc(DELTA_TIME, my_timer, 0);
	}

	glutPostRedisplay();
	return;
}

