#include <stdlib.h>
#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# include "GL/glut.h"
#endif

struct planet{
	int year = 0;
	int day  = 0;
	int year_offset =0;
	int day_offset = 0;
	float dist = 0.0;
	float radius = 0.0;
	float r;float g;float b;

	//constructor
	planet(){}
	planet(int y,int d,int y_o,int d_o,float dis,float rad,float red,float green,float blue){
		year = y;
		day = d;
		year_offset=y_o;
		day_offset=d_o;
		dist = dis;
		radius = rad;
		r = red;
		g = green;
		b = blue;
	}
};
int year = 0;
int day = 0;
planet planets[8]; 
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	GLfloat position[] = { 0.5f, 0.5f, 0.5f, 1.0f }; 
	glLightfv(GL_LIGHT0, GL_POSITION, position); 
}
void bigbang(planet planets[]){
	planets[0] = planet(0,0,2,2,1,0.1,0.5,0.5,0.5); //mercury
	planets[1] = planet(0,0,3,5,1.5,0.15,1.0,0.69,0); //venus
	planets[2] = planet(0,0,5,6,1.75,0.25,0.0,0.0,1); //earth
	planets[3] = planet(0,0,4,9,1.55,0.2,1.0,0.0,0.0); //mars
	planets[4] = planet(0,0,7,3,1.65,0.45,0.25,0.20,0.30); //jupyter
	planets[5] = planet(0,0,6,7,1.75,0.35,0.5,0.6,1.0); //saturn
	planets[6] = planet(0,0,1,4,2,0.37,0.1,0.5,0.3); //Uranus
	planets[7] = planet(0,0,8,6,2.5,0.4,0.5,0.5,0.3); //neptune
	
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
		glColor3f (1.0, 1.0, 0.0);
		glutSolidSphere(0.5, 20, 20);// create sun
	glPopMatrix();

	for(int i=0;i<8;i++)
	{
	glPushMatrix();
		glRotatef((GLfloat) planets[i].year, 0.0, 1.0, 0.0);
		glTranslatef(planets[i].dist, 0.0, 0.0);
		glRotatef((GLfloat) planets[i].day, 0.0, 1.0, 0.0);
		glColor3f(planets[i].r, planets[i].g, planets[i].b);
		glutSolidSphere(planets[i].radius, 20, 20);
		if(i == 2){
			
		glRotatef((GLfloat) planets[i].year, 0.0, 1.0, 0.0);
		glTranslatef(0.5, 0.0, 0.0);
		glRotatef((GLfloat) planets[i].day, 0.0, 1.0, 0.0);
		glColor3f(1, 1, 1);
		glutSolidSphere(0.1, 20, 20);// create moon
		}
	glPopMatrix();
	}
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


void timer(int i)
{
	planets[i].year = (planets[i].year + planets[i].year_offset) % 360;
	planets[i].day = (planets[i].day + planets[i].day_offset) % 360;
	glutPostRedisplay();
	glutTimerFunc(50, timer, i);
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(1200, 1200);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Solar System");
	glEnable(GL_TEXTURE_2D); 
	bigbang(planets);
	init();
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	for(int i=0;i<8;i++)glutTimerFunc(50, timer,i);
	glutMainLoop();
	return 0;
}