#include <stdio.h>
#include <windows.h>	   // Standard header for MS Windows applications
#include <opengl/GL/gl.h>		   // Open Graphics Library (OpenGL) header
#include <opengl/GL/glu.h>		   // Open Graphics Library (OpenGL) header
#include <opengl/GL/glut.h>	   // The GL Utility Toolkit (GLUT) Header
#include <time.h>

#define KEY_ESCAPE 27

#include "raytracer.h"

typedef struct {
    int width;
	int height;
	char* title;

	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;

glutWindow win;

float g_rotation = 0;
float g_rotation_speed = 0.2f;

long g_fps;
time_t g_time;

#define top_left_near     { 1,-1, 1}
#define top_right_near    {-1,-1, 1}
#define bottom_left_near  { 1, 1, 1}
#define bottom_right_near {-1, 1, 1}
#define top_left_far      { 1,-1,-1}
#define top_right_far     {-1,-1,-1}
#define bottom_left_far   { 1, 1,-1}
#define bottom_right_far  {-1, 1,-1}

polygon_t p[] = {
	// front side
	{ top_left_near, top_right_near, bottom_right_near },
	{ bottom_right_near, bottom_left_near, top_left_near },
	// right side
	{ top_right_near, top_right_far, bottom_right_far },
	{ bottom_right_far, bottom_right_near, top_right_near },
	// back side
	{ top_right_far, top_left_far, bottom_left_far },
	{ bottom_left_far, bottom_right_far, top_right_far },
	// left side
	{ top_left_far, top_left_near, bottom_left_near },
	{ bottom_left_near, bottom_left_far, top_left_far },
	// top side
	{ top_right_far, top_left_near, top_left_far },
	{ top_left_near, top_right_near, top_right_far },
	// bottom side
	{ bottom_left_near, bottom_right_near, bottom_right_far },
	{ bottom_right_far, bottom_left_far, bottom_left_near },
};

void display() 
{
	glClearColor(.3, .3, .3, 0);
 	glClear(GL_COLOR_BUFFER_BIT);

        int x, y, i;
        vector_t camPosition = { 0,0,-3 };
        light_t light = { {cos(g_rotation), 0, sin(g_rotation)}, {1,1,1} };
	vector_t spherePosition = { 0,0,1 };
        float radius = 1.5;

	glBegin(GL_POINTS);
        for(y=0; y<win.height; y++) {
                for(x=0; x<win.width; x++) {
        		vector_t rayDirection = { (x-win.width/2.0)/(win.width/2.0),(y-win.height/2.0)/(win.height/2.0), 1 };
                        color_t pixel = rayCastRaySphere(camPosition, rayDirection, spherePosition, radius, light);
			for(i=0; i<12; i++) {
                        	color_t pixel2 = rayCastRayPolygon(camPosition, rayDirection, p[i], light);
				pixel.r += pixel2.r;
				pixel.g += pixel2.g;
				pixel.b += pixel2.b;
			}
			glColor3f(pixel.r, pixel.g, pixel.b);
			glVertex2f(x + 0.5, y + 0.5);
                }
        }
 	glEnd();

	glBegin(GL_POINTS);
	glVertex2f(50 + 0.5, 50 + 0.5);
 	glEnd();

	glutSwapBuffers();

	g_rotation += g_rotation_speed;
	g_fps++;

	time_t t = time(NULL);
	if( t != g_time ) {
		printf("FPS: %d\n", g_fps);
		g_time = t;
		g_fps = 0;
	}
}

void initialize () 
{
	glMatrixMode(GL_PROJECTION);												
	glViewport(0, 0, win.width, win.height);									
	glMatrixMode(GL_PROJECTION);												
	glLoadIdentity();
	GLfloat aspect = (GLfloat) win.width / win.height;
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);		

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (0, win.width, win.height, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	// Displacement trick for exact pixelization
	glTranslatef(0.375, 0.375, 0);
}


void keyboard ( unsigned char key, int mousePositionX, int mousePositionY )		
{ 
	switch ( key ) 
	{
		case KEY_ESCAPE:        
			exit ( 0 );   
			break;      

		default:
			break;
	}
}

void special ( int key, int mousePositionX, int mousePositionY )
{
	switch( key )
	{
		case GLUT_KEY_LEFT:
			g_rotation_speed = 0.0;
			g_rotation -= 0.2;
			break;

		case GLUT_KEY_RIGHT:
			g_rotation_speed = 0.0;
			g_rotation += 0.2;
			break;

		default:      
			break;
	}
}

int main(int argc, char **argv) 
{
	// set window values
	win.width = 200;
	win.height = 200;
	win.title = "RayTracer";
	win.field_of_view_angle = 45;
	win.z_near = 1.0f;
	win.z_far = 500.0f;

	if( argc == 3 ) {
		win.width = atoi(argv[1]);
		win.height= atoi(argv[2]);
	}

	// initialize and run program
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize(win.width,win.height);
	glutCreateWindow(win.title);
	glutDisplayFunc(display);
	glutIdleFunc(display);
    	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	initialize();
	glutMainLoop();	

	return EXIT_SUCCESS;
}

