#include <stdlib.h>
#include "GL/glut.h"

float x = 0.0;
float y = 0.0;
float z = 0.0;

float h1 = 0.5;
float l1 = 2;
float w1 = 1;

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
}

void display(void)
{
	
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
////////////////// Code Here //////////////
	
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_QUADS);
	glVertex3f(-l1,0.0,h1);
	glVertex3f(0.0,w1,h1);
	glVertex3f(l1,0.0,h1);
	glVertex3f(0.0,-w1,h1);
	glEnd();

	glPushMatrix();
		glTranslatef(x, y, z);
		glColor3f (0.0, 0.0, 1.0);
		glutSolidSphere(0.2, 20, 20);// create sphere

		glTranslatef(0.2, 0.0, 0.0);
		glColor3f (1.0,0.0,0.0);
		glutSolidSphere(0.15,10,20); //2nd sphere

		glTranslatef(0.2, 0.0, 0.0);
		glColor3f (0.0,1.0,0.0);
		glutSolidSphere(0.1,10,20); //3rd sphere
	glPopMatrix();




///////////////

	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0,0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, (GLfloat)w/(GLfloat)h, 1.0, 20.0);
	gluLookAt(0.0, 0.0, 3.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void keyboard (unsigned char key, int k, int l)
{   
 
	switch (key) {
    
    case 'w':
	     x+=0.1*(0-x);
		 y+=0.1*(w1-y);
		 z+=0.1*(h1-z);
		 break;
    case 's':
		 x+=0.1*(0-x);
		 y+=0.1*(-w1-y);
		 z+=0.1*(h1-z);
		break;
    case 'a':
		 x = x + 0.1*(-l1 - x);
		 y = y + 0.1*(0-y);
		 z = z + 0.1*(h1-z);
         break;
     case 'd':
	    x = x + 0.1*(l1-x);
		y = y+ 0.1*(0-y);
		z = z + 0.1*(h1-z);
         break;
     case 'q':
	 	 z+=0.1;
         break;
     case 'e':
	 	z= z-0.1;
         break;
    default:
         break;
	 }
	 glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(250, 250);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Platform");
	init();
	
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);   
	glutReshapeFunc(reshape);
	
	glutMainLoop();
	return 0;
}