#include <iostream>
using std::cerr;
using std::endl;
#include <stdlib.h>
//using std::exit;
#include <GL/glut.h> // GLUT stuff, includes OpenGL headers as well 
#include <windows.h>
#include <math.h>
#include <gl/Gl.h>
#include <gl/Glu.h>


int SCREEN_HEIGHT = 480;
// Keep track of times clicked, on 3 clicks draw.
int NUMPOINTS = 0;

// Point class to keep it a little cleaner.
class Point {
public:
	float x, y, z;
	void setxy(float x2, float y2) { x = x2; y = y2; }

};

Point points[4];

void myInit() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(4.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);

}

void drawDot(int x, int y) {
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
	glFlush();
}

void drawLine(Point p1, Point p2) {
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);

	glEnd();
	glFlush();
}


Point Casteljau(int i, int j, float t) {
	if (i == 0) {
		return points[j];
	}
	Point P;

	P.x = (1 - t)*Casteljau(i - 1, j, t).x + t*Casteljau(i - 1, j + 1, t).x;
	P.y = (1 - t)*Casteljau(i - 1, j, t).y + t*Casteljau(i - 1, j + 1, t).y;

	return P;
}

void myMouse(int button, int state, int x, int y) {
	// If left button was clicked
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Store where the user clicked, note Y is backwards.
		points[NUMPOINTS].setxy((float)x, (float)(SCREEN_HEIGHT - y));
		NUMPOINTS++;

		// Draw the red  dot.
		drawDot(x, SCREEN_HEIGHT - y);

		// If 3 points are drawn do the curve.
		if (NUMPOINTS > 3) {
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();

			glColor3f(1.0, 1.0, 1.0);
			for (int i = 0; i < NUMPOINTS - 1; i++) {
				drawLine(points[i], points[i + 1]);
			}

			for (int i = 0; i < NUMPOINTS; i++) {
				printf("pont j000 = %d valor x = %f valor y = %f \n", i, points[i].x, points[i].y);
			}
			// Draw two legs of the triangle


			//drawLine(abc[3], abc[4]);
			Point POld = points[0];
			/* Draw each segment of the curve.  Make t increment in
			smaller amounts for a more detailed curve. */
			for (double t = 0.0; t <= 1.0; t += 0.01) {
				Point P = Casteljau(NUMPOINTS - 1, 0, t);
				drawLine(POld, P);
				POld = P;
			}
			glColor3f(1.0, 0.0, 0.0);

		}
	}
}

void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Bezier Curve");

	glutMouseFunc(myMouse);
	glutDisplayFunc(myDisplay);

	myInit();
	glutMainLoop();

	return 0;
}