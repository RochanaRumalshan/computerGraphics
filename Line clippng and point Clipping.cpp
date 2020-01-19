#include <windows.h>
#include <GL/glut.h> 
#include <math.h>
#include<iostream>
#define PI 3.14159265f
#include <array>
using namespace std;

GLfloat windowW = 20;
GLfloat windowH = 20;

//area of intrest
int xVmin = -10;
int xVmax = 10;
int yVmin = -10;
int yVmax = 10;

struct Point {
	int x=0, y=0;
	int code[4] = { 0,0,0,0 };
};


Point p1 = { -14,-15 };
Point p2 = { -2, 7 };
Point p3 = { 2, -17 };
Point p4 = { -17, 5 };
Point p5 = { 17, 5 };
//Point p6 = { 9, -15 };
//Point p7 = { 7, 15 };


void Init() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1.0);
}

//point clipping
void pointClipping(int x, int y) {
	if (x > xVmin && x < xVmax) {
		if (y > yVmin && y < yVmax) {
			glPointSize(5.0f);
			glColor3f(1.0f, 1.0f, 0.0f);
			glBegin(GL_POINTS);
			glVertex2f(x, y);
			glEnd();
		}
	}
	glFlush();
}

Point assignRegionCode(Point p1){
	if (p1.x > xVmax) { //1st bit from right -- 
		p1.code[3] = 1;
	}
	if (p1.x < xVmin) {//2nd bit from right -- 
		p1.code[2] = 1;
	}
	if (p1.y < yVmin) {//3rd bit from right
		p1.code[1] = 1;
	}
	if (p1.y > yVmax) { //4th bit from right
		p1.code[0] = 1;
	}
	return p1;
}

boolean checkVisibility(Point p) {//returns true if the point is in the visible area
	boolean visibility = true;

	for (int i = 0; i < 4;i++) {
		if (p.code[i] == 1) {
			visibility = false;
			break;
		}
	}
	return visibility;
}

boolean checkInBetweenVisibility(Point p , Point p2) {//returns true if a part of the line is in the visible area
	boolean visibility = true;

	for (int i = 0; i < 4;i++) {
		if ((p.code[i] & p2.code[i]) == 1) {
			visibility = false;
			break;
		}
	}
	return visibility;
}

float calculateM(Point p1, Point p2) {
	float m = ((p2.y - p1.y) / (p2.x - p1.x));
	return m;
}


void drawLine(Point p1, Point p2) {
	glPointSize(5.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
	glEnd();
	
	glFlush();
}


//line clipping
void lineClipping(Point p1, Point p2) {
	Point ep1 = assignRegionCode(p1); //end point 1 with code assigned
	Point ep2 = assignRegionCode(p2); //end point 2 with code assigned

	cout << "Point 1 :" << endl;
	cout << ep1.x << "   " << ep1.y << "---" << "\n" << endl;
	cout << ep1.code[0] << "  " << ep1.code[1] << "  " << ep1.code[2] << "  " << ep1.code[3] << "\n\n" <<endl;

	cout << "Point 2 :" << endl;
	cout << ep2.x << "   " << ep2.y << "---" << "\n" << endl;
	cout << ep2.code[0] << "  " << ep2.code[1] << "  " << ep2.code[2] << "  " << ep2.code[3];

	
	if (checkVisibility(ep1) == true && checkVisibility(ep2) == true) { //dekama inside nam
		drawLine(ep1, ep2);
		cout << "Both inside" << endl;
	}
	else if (checkVisibility(ep1) == true || checkVisibility(ep2) == true) {  // 2n ekak 0000 nam.... 
		Point newEndPoint;
		float m = calculateM(ep1, ep2);
		if (checkVisibility(ep1) == true) { // ep1 0000 nam.... 
		
			int c1 = (int)(ep1.y - (m * ep1.x));

			if ((ep2.code[0] == 1) || (ep2.code[1] == 1)) {
				if (ep2.code[0] == 1) {
					newEndPoint.y = yVmax;
					newEndPoint.x = int((yVmax - c1) / m);
				}
				else {
					newEndPoint.y = yVmin;
					newEndPoint.x = int((yVmin - c1) / m);
				}
			}else {
				if (ep2.code[2] == 1) {
					newEndPoint.y = int((m*xVmin) + c1);
					newEndPoint.x = xVmin;
				}
				else {
					newEndPoint.y = int((m * xVmax) + c1);
					newEndPoint.x = xVmax;
				}
			}
			cout << "\n\nonly ep1 inside" << endl;
			drawLine(ep1, newEndPoint);
		}
		else { // ep2 0000 nam
			
			int c = (int)(ep2.y - (m * ep2.x));

			if ((ep1.code[2] == 1) || (ep1.code[3] == 1)) {
				if (ep1.code[2] == 1) {
					newEndPoint.y = int((m*xVmin)+c);
					newEndPoint.x = xVmin;
				}
				else {
					newEndPoint.y = int((m * xVmax)+c);
					newEndPoint.x = xVmin;
				}
			}
			else {
				if (ep1.code[3] == 1) {
					newEndPoint.y = int((m * xVmin) + c);
					newEndPoint.x = xVmax;
				}
				else {
					newEndPoint.y = int((m * xVmax) + c);
					newEndPoint.x = xVmax;
				}
			}
			cout << "\n\nonly ep2 inside" << endl;
			drawLine(ep2, newEndPoint);
		}
	}
	else if(checkInBetweenVisibility(ep1, ep2)==true){
		float m = calculateM(ep1, ep2);
		Point newEp1, newEp2;

		int c2 = (int)(ep2.y - (m * ep2.x));

		//ep2 out of range
		if ((ep2.code[0] == 1) || (ep2.code[1] == 1)) {
			if (ep2.code[0] == 1) {
				newEp2.y = yVmax;
				newEp2.x = int((yVmax - c2) / m);
			}
			else {
				newEp2.y = yVmin;
				newEp2.x = int((yVmin - c2) / m);
			}
		}
		else {
			if (ep2.code[2] == 1) {
				newEp2.y = int((m * xVmin) + c2);
				newEp2.x = xVmin;
			}
			else {
				newEp2.y = int((m * xVmax) + c2);
				newEp2.x = xVmax;
			}
		}

		//ep1 out of range
		int c1 = (int)(ep2.y - (m * ep2.x));
		if ((ep1.code[2] == 1) || (ep1.code[3] == 1)) {
			if (ep1.code[2] == 1) {
				newEp1.y = int((m * xVmin) + c1);
				newEp1.x = xVmin;
			}
			else {
				newEp1.y = int((m * xVmax) + c1);
				newEp1.x = xVmin;
			}
		}
		else {
			if (ep1.code[3] == 1) {
				newEp1.y = int((m * xVmin) + c1);
				newEp1.x = xVmax;
			}
			else {
				newEp1.y = int((m * xVmax) + c1);
				newEp1.x = xVmax;
			}
		}
		drawLine(newEp1, newEp2);
		cout << "\n\nBoth ep1,ep2 are outside but middle range visible" << endl;
	
	}else{
		//line is not visible
		cout << "Line is not visible - both points are at the outside" << " " << endl;
	}

	

}

//drawing the border around the area of intrest
void drawBorder() {
	
			glLineWidth(2.0f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glBegin(GL_LINE_LOOP);
			glVertex2f(xVmax, yVmax);
			glVertex2f(xVmin, yVmax);
			glVertex2f(xVmin, yVmin);
			glVertex2f(xVmax, yVmin);
			glEnd();

	glFlush();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	//define the boundary for clipping
	drawBorder();

	//point clipping example
	glPointSize(5.0f);
	glPushMatrix();
		pointClipping(4,7);
		pointClipping(18,7);
		pointClipping(19,7);
	glPopMatrix();
	
	//line clipping example
	glPushMatrix();
		lineClipping(p1, p2);
		lineClipping(p2, p3);
		lineClipping(p3, p1);
		lineClipping(p4, p5);
		//lineClipping(p6, p7);
	glPopMatrix();

}


void changesize(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	GLfloat aspect_ratio = h == 0 ? w / 1 : (GLfloat)w / (GLfloat)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
		glOrtho(-windowW, windowW, -windowH / aspect_ratio, windowH / aspect_ratio, 20.0, -20.0);
	else
		glOrtho(-windowW * aspect_ratio, windowW * aspect_ratio, -windowH, windowH, 20.0, -20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
											   // Compute aspect ratio of the new window
	if (height == 0) height = 1;                // To prevent divide by 0
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping area to match the viewport
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset the projection matrix
	if (width >= height) {
		// aspect >= 1, set the height from -1 to 1, with larger width
		gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
	}
	else {
		// aspect < 1, set the width to -1 to 1, with larger height
		gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
	}
}


int main(int argc, char* argv[]) {
	
	
	

	/*glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA |GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 10);
	glutCreateWindow("Window");
	glutDisplayFunc(display);
	glutReshapeFunc(changesize);
	glutTimerFunc(30, Timer, 2);
	Init();
	glutMainLoop();
	return 0;

*/

// Use a single buffered window in RGB mode (as opposed to a double-buffered
// window or color-index mode).
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	// Position window at (80,80)-(480,380) and give it a title.
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Window");

	//To make the cartesian coordinate range from -1 -- -1 to  -20 -- 20 
	glutReshapeFunc(changesize);
	// Tell GLUT that whenever the main window needs to be repainted that it
	// should call the function display().
	glutDisplayFunc(display);

	// Tell GLUT to start reading and processing events.  This function
	// never returns; the program only exits when the user closes the main
	// window or kills the process.
	glutMainLoop();

	return 0;
}
