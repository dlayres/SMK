/*
 *  CSCI 441, Computer Graphics, Fall 2018
 *
 *  Project: SMK
 *  File: main.cpp
 *
 *	Author: David Ayres (and others)
 *
 *  Description:
 *      Watch some heroes race around a track
 *
 */

// include the OpenGL library header
#ifdef __APPLE__					// if compiling on Mac OS
	#include <OpenGL/gl.h>
#else										// if compiling on Linux or Windows OS
	#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>	// include GLFW framework header

#include <CSCI441/objects.hpp> // for our 3D objects

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>				// for cos(), sin() functionality
#include <stdio.h>			// for printf functionality
#include <stdlib.h>			// for exit functionality
#include <time.h>			  // for time() functionality
#include <fstream>			// for file I/O
#include <vector>				// for vectors
#include <iostream>

using namespace std;

//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
int windowWidth = 640, windowHeight = 480;

int leftMouseButton;    	 						// status of the mouse button
glm::vec2 mousePos;			              		  	// last known X and Y of the mouse

glm::vec3 camPos;            						// camera position in cartesian coordinates
float cameraTheta, cameraPhi;               		// camera DIRECTION in spherical coordinates
float camDistance = 5.0f;							// camera distance from the hero (lookAt point)
float cameraSpeed = 0.05f;							// speed the camera moves as a factor of a unit vector
bool cameraIn = false;								// values that determine whether or not the camera is moving and in what direction
bool cameraOut = false;
bool cameraLeft = false;
bool cameraRight = false;

glm::vec3 heroPos;									// position of the hero
float heroAngle;									// direction the hero is facing in radians (0 is along the positive Z axis)
glm::vec3 heroDir;									// vector for the direction the hero is facing
float walkSpeed = 0.05f;							// speed the hero is walking as a factor of a unit vector
float turnSpeed = 0.05f;							// speed the hero is turning

bool walking = false;								// values that determine whether or not the hero is turning and/or walking
bool turning = false;
float direction = 1.0f;								// direction the hero is walking (forwards = 1.0, backwards = -1.0)
float turnDirection = 1.0f;							// direction the hero is turning (clockwise = -1.0, counterclockwise = 1.0)

float animationFrame = 0.0f;						// value for the current animation cycle (used for legs moving and body bobbing)
int animateIndex = 5;								// index the value of the animation cycle is in the following vector
vector<float> animateVals;							// vector to store a few different animation cycle values

vector<glm::vec3> controlPoints; 					// control points for Bezier curve 										// how many curve pieces the whole curve is made of


bool cageOn = true;									// Determines if the cage/curve should be visible or not
bool curveOn = true;

glm::mat4 transMtx; 								// global variables used for transformations
glm::mat4 rotateMtx;
glm::mat4 scaleMtx;

GLuint environmentDL;                       		// display list for the 'city'

//*************************************************************************************
//
// Helper Functions

// getRand() ///////////////////////////////////////////////////////////////////
//
//  Simple helper function to return a random number between 0.0f and 1.0f.
//
////////////////////////////////////////////////////////////////////////////////
float getRand() { return rand() / (float)RAND_MAX; }

// recomputeOrientation() //////////////////////////////////////////////////////
//
// This function updates the camera's direction in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraTheta or cameraPhi is updated.
//
////////////////////////////////////////////////////////////////////////////////
void recomputeOrientation() {
	heroDir = glm::vec3(sin(heroAngle), 0, cos(heroAngle));
	camPos = glm::vec3(camDistance * sin(cameraTheta) * sin(cameraPhi), -camDistance * cos(cameraPhi), -camDistance * cos(cameraTheta) * sin(cameraPhi)) + heroPos;
}

// checkBounds() ///////////////////////////////////////////////////////////////
//
// This function determines if, when the hero walks, their position is
//  out of bounds of the map, and if it is, places the hero back on the map
//
////////////////////////////////////////////////////////////////////////////////
void checkBounds(){
	if(heroPos.x < -50){
		heroPos.x = -50;
	}
	else if(heroPos.x > 50){
		heroPos.x = 50;
	}
	else if(heroPos.z < -50){
		heroPos.z = -50;
	}
	else if(heroPos.z > 50){
		heroPos.z = 50;
	}
}

// loadControlPoints() /////////////////////////////////////////////////////////
//
//  Load our control points from file and store them in
//	the global variable controlPoints
//
////////////////////////////////////////////////////////////////////////////////
bool loadControlPoints( char* filename ) {
	ifstream inputFile(filename);

	string numPointsString;
	int numPoints;
	getline(inputFile, numPointsString);
	sscanf(numPointsString.c_str(), "%d", &numPoints);

	for(int i = 0; i < numPoints; i++){
		string x, y, z;
		getline(inputFile, x, ',');
		getline(inputFile, y, ',');
		getline(inputFile, z);
		glm::vec3 controlPoint = glm::vec3(atof(x.c_str()), atof(y.c_str()), atof(z.c_str()));
		controlPoints.push_back(controlPoint);
	}



	return true;
}


// evaluateBezierCurve() ////////////////////////////////////////////////////////
//
// Computes a location along a Bezier Curve.
//
////////////////////////////////////////////////////////////////////////////////
glm::vec3 evaluateBezierCurve( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t ) {
	glm::vec3 point = (float(pow((1.0 - t), 3)) * p0) + (3.0f * float(pow((1.0 - t), 2)) * t * p1) + (3.0f * (1.0f - t) * float(pow(t, 2)) * p2) + (float(pow(t, 3)) * p3);
	return point;
}

// renderBezierCurve() //////////////////////////////////////////////////////////
//
// Responsible for drawing a Bezier Curve as defined by four control points.
//  Breaks the curve into n segments as specified by the resolution.
//
////////////////////////////////////////////////////////////////////////////////
void renderBezierCurve( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int resolution ) {
	glBegin(GL_LINE_STRIP);
	float t = 0.0;
	while(t <= 1.0 + (1.0 / resolution)){
		glm::vec3 nextPoint = evaluateBezierCurve(p0, p1, p2, p3, t);
		glVertex3f(nextPoint.x, nextPoint.y, nextPoint.z);
		t += (1.0/resolution);
	}
	glEnd();
}

//*************************************************************************************
//
// Event Callbacks

//
//	void error_callback( int error, const char* description )
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
static void error_callback( int error, const char* description ) {
	fprintf( stderr, "[ERROR]: %s\n", description );
}

static void keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
	if( action == GLFW_PRESS || action == GLFW_REPEAT ) {
		switch( key ) {
			case GLFW_KEY_ESCAPE: // Escape and Q quit the program
			case GLFW_KEY_Q:
				exit(EXIT_SUCCESS);
				break;
			case GLFW_KEY_UP:	// Up arrow key moves camera inward
				cameraIn = true;
				break;
			case GLFW_KEY_DOWN:	// Down arrow key moves camera outward
				cameraOut = true;
				break;
			case GLFW_KEY_LEFT: // Left arrow key moves camera clockwise around hero
				cameraLeft = true;
				break;
			case GLFW_KEY_RIGHT: // Right arrow key moves counterclockwise around hero
				cameraRight = true;
				break;
			case GLFW_KEY_W: // W key moves hero forward
				walking = true;
				direction = 1.0f;
				break;
			case GLFW_KEY_S: // S key moves hero backward
				walking = true;
				direction = -1.0f;
				break;
			case GLFW_KEY_A: // A key turns hero counterclockwise
				turning = true;
				turnDirection = 1.0f;
				break;
			case GLFW_KEY_D: // D key turns hero clockwise
				turning = true;
				turnDirection = -1.0f;
				break;
		}
	}
	else{
		walking = false; // Resets all boolean values because nothing is happening
		turning = false;
		direction = 1.0f;
		cameraIn = false;
		cameraOut = false;
		cameraLeft = false;
		cameraRight = false;
	}

	if(action == GLFW_PRESS){
		switch(key){
			case GLFW_KEY_1: // 1 key toggles control cage visibility
				cageOn = !cageOn;
				break;
			case GLFW_KEY_2: // 2 key toggles Bezier curve visibility
				curveOn = !curveOn;
				break;
		}
	}
}

// cursor_callback() ///////////////////////////////////////////////////////////
//
//  GLFW callback for mouse movement. We update cameraPhi and/or cameraTheta
//      based on how much the user has moved the mouse in the
//      X or Y directions (in screen space) and whether they have held down
//      the left or right mouse buttons. If the user hasn't held down any
//      buttons, the function just updates the last seen mouse X and Y coords.
//
////////////////////////////////////////////////////////////////////////////////
static void cursor_callback( GLFWwindow *window, double x, double y ) {
	if( leftMouseButton == GLFW_PRESS ) {
		cameraTheta -= (0.005 * (x - mousePos.x));
		cameraPhi += (0.005 * (mousePos.y - y));
		if(cameraPhi > M_PI - 0.01){
			cameraPhi = M_PI - 0.01;
		}
		else if(cameraPhi < 0.01){
			cameraPhi = 0.01;
		}
		recomputeOrientation();     // update camera (x,y,z) based on (theta,phi)
	}

	mousePos.x = x;
	mousePos.y = y;
}

// mouse_button_callback() /////////////////////////////////////////////////////
//
//  GLFW callback for mouse clicks. We save the state of the mouse button
//      when this is called so that we can check the status of the mouse
//      buttons inside the motion callback (whether they are up or down).
//
////////////////////////////////////////////////////////////////////////////////
static void mouse_button_callback( GLFWwindow *window, int button, int action, int mods ) {
	if( button == GLFW_MOUSE_BUTTON_LEFT ) {
		leftMouseButton = action;
	}
}

// scroll_callback() /////////////////////////////////////////////////////
//
//  GLFW callback for mouse scrolling. Used to zoom the camera in or out
//
////////////////////////////////////////////////////////////////////////////////
static void scroll_callback( GLFWwindow *window, double xoffset, double yoffset){
	camDistance -= yoffset;
	if(camDistance < 5){
		camDistance = 5;
	}
	else if(camDistance > 15){
		camDistance = 15;
	}
	recomputeOrientation();
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

// drawGrid() //////////////////////////////////////////////////////////////////
//
//  Function to draw a grid in the XZ-Plane using OpenGL 2D Primitives (GL_LINES)
//
////////////////////////////////////////////////////////////////////////////////
void drawGrid() {
	/*
     *	We will get to why we need to do this when we talk about lighting,
     *	but for now whenever we want to draw something with an OpenGL
     *	Primitive - like a line, quad, point - we need to disable lighting
     *	and then reenable it for use with the CSCI441 3D Objects.
     */
	glDisable( GL_LIGHTING );

	glColor3ub(255, 255, 255);
	glBegin(GL_LINES);
		for(int i = -50; i < 50; i++){
			glVertex3f(i, 0, -50);
			glVertex3f(i, 0, 50);
			glVertex3f(-50, 0, i);
			glVertex3f(50, 0, i);
		}
	glEnd();

	/*
     *	As noted above, we are done drawing with OpenGL Primitives, so we
     *	must turn lighting back on.
     */
	glEnable( GL_LIGHTING );
}

void drawTopHalf(){ // Draws body, head (with eyes), and horns of character
	// Draws body (torso)
	glColor3ub(82, 84, 105);
	CSCI441::drawSolidCone(.35, 1, 5, 20);

	// Draws bottom of cloak
	rotateMtx = glm::rotate(glm::mat4(), static_cast<float>(M_PI)/2.0f, glm::vec3(1, 0, 0));
	glMultMatrixf(&rotateMtx[0][0]);
	CSCI441::drawSolidDisk(0, 0.35, 20, 20);
	rotateMtx = glm::rotate(glm::mat4(), static_cast<float>(-M_PI)/2.0f, glm::vec3(1, 0, 0));
	glMultMatrixf(&rotateMtx[0][0]);

	// Draws head
	transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0.95, 0));
	glMultMatrixf(&transMtx[0][0]);
	glColor3ub(255, 255, 255);
	CSCI441::drawSolidSphere(.3, 20, 20);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0, -0.95, 0));
	glMultMatrixf(&transMtx[0][0]);

	// Draws horns
	scaleMtx = glm::scale(glm::mat4(), glm::vec3(1, 5, 1));
	glMultMatrixf(&scaleMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(-0.2, 0.225, 0));
	glMultMatrixf(&transMtx[0][0]);
	rotateMtx = glm::rotate(glm::mat4(), -0.5f, glm::vec3(0, 0, 1));
	glMultMatrixf(&rotateMtx[0][0]);
	CSCI441::drawSolidCube(0.1);
	rotateMtx = glm::rotate(glm::mat4(), 0.5f, glm::vec3(0, 0, 1));
	glMultMatrixf(&rotateMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.2, -0.225, 0));
	glMultMatrixf(&transMtx[0][0]);
	scaleMtx = glm::scale(glm::mat4(), glm::vec3(1, 0.2, 1));
	glMultMatrixf(&scaleMtx[0][0]);
	scaleMtx = glm::scale(glm::mat4(), glm::vec3(1, 5, 1));
	glMultMatrixf(&scaleMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.2, 0.225, 0));
	glMultMatrixf(&transMtx[0][0]);
	rotateMtx = glm::rotate(glm::mat4(), 0.5f, glm::vec3(0, 0, 1));
	glMultMatrixf(&rotateMtx[0][0]);
	CSCI441::drawSolidCube(0.1);
	rotateMtx = glm::rotate(glm::mat4(), -0.5f, glm::vec3(0, 0, 1));
	glMultMatrixf(&rotateMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(-0.2, -0.225, 0));
	glMultMatrixf(&transMtx[0][0]);
	scaleMtx = glm::scale(glm::mat4(), glm::vec3(1, 0.2, 1));
	glMultMatrixf(&scaleMtx[0][0]);

	// Draws eyes
	glColor3ub(0, 0, 0);
	transMtx = glm::translate(glm::mat4(), glm::vec3(-0.1, 0.9, 0.25));
	glMultMatrixf(&transMtx[0][0]);
	CSCI441::drawSolidSphere(0.05, 20, 20);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.1, 0.2, -0.25));
	glMultMatrixf(&transMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.1, -0.2, 0.25));
	glMultMatrixf(&transMtx[0][0]);
	CSCI441::drawSolidSphere(0.05, 20, 20);
	transMtx = glm::translate(glm::mat4(), glm::vec3(-0.1, -0.9, -0.25));
	glMultMatrixf(&transMtx[0][0]);
}

void drawLeg(){ // Draws a single leg
	glColor3ub(30, 30, 30);
	CSCI441::drawSolidCylinder(0.08, 0.08, 0.3, 20, 20);
	CSCI441::drawSolidSphere(0.08, 20, 20);
}

void drawCharacter(){ // Draws character from upper half and two legs
	transMtx = glm::translate(glm::mat4(), heroPos); // Checks the orientation of the hero and draws the upper half accordingly
	glMultMatrixf(&transMtx[0][0]);
	rotateMtx = glm::rotate(glm::mat4(), heroAngle, glm::vec3(0, 1, 0));
	glMultMatrixf(&rotateMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0, animationFrame / 20, 0));
	glMultMatrixf(&transMtx[0][0]);
	drawTopHalf();
	transMtx = glm::translate(glm::mat4(), glm::vec3(0, -animationFrame / 20, 0));
	glMultMatrixf(&transMtx[0][0]);

	transMtx = glm::translate(glm::mat4(), glm::vec3(-0.1, -0.22, 0)); // Draws both legs according to current animation frame
	glMultMatrixf(&transMtx[0][0]);
	rotateMtx = glm::rotate(glm::mat4(), 1.2f*animationFrame, glm::vec3(1, 0, 0));
	glMultMatrixf(&rotateMtx[0][0]);
	drawLeg();

	rotateMtx = glm::rotate(glm::mat4(), -1.2f*animationFrame, glm::vec3(1, 0, 0));
	glMultMatrixf(&rotateMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.2, 0, 0));
	glMultMatrixf(&transMtx[0][0]);
	rotateMtx = glm::rotate(glm::mat4(), -1.2f*animationFrame, glm::vec3(1, 0, 0));
	glMultMatrixf(&rotateMtx[0][0]);
	drawLeg();

	rotateMtx = glm::rotate(glm::mat4(), 1.2f*animationFrame, glm::vec3(1, 0, 0)); // Undoes transformations
	glMultMatrixf(&rotateMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(-0.1, 0.22, 0));
	glMultMatrixf(&transMtx[0][0]);
	rotateMtx = glm::rotate(glm::mat4(), -heroAngle, glm::vec3(0, 1, 0));
	glMultMatrixf(&rotateMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), -heroPos);
	glMultMatrixf(&transMtx[0][0]);
}

void drawLamppost(){ // Draws a single lamppost
	transMtx = glm::translate(glm::mat4(), glm::vec3(10, 0, 10)); // Moves lamppost to (10, 0, 10);
	glMultMatrixf(&transMtx[0][0]);

	transMtx = glm::translate(glm::mat4(), glm::vec3(0, 2.37, 0)); // Draws post
	glMultMatrixf(&transMtx[0][0]);
	scaleMtx = glm::scale(glm::mat4(), glm::vec3(1, 16, 1));
	glMultMatrixf(&scaleMtx[0][0]);
	glColor3ub(160, 160, 160);
	CSCI441::drawSolidCube(0.3);

	scaleMtx = glm::scale(glm::mat4(), glm::vec3(1, 1.0/16, 1)); // Draws the top cover for the light
	glMultMatrixf(&scaleMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0, -2.37, 0));
	glMultMatrixf(&transMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.35, 4.9, 0));
	glMultMatrixf(&transMtx[0][0]);
	scaleMtx = glm::scale(glm::mat4(), glm::vec3(3, 1, 2));
	glMultMatrixf(&scaleMtx[0][0]);
	CSCI441::drawSolidCube(0.3);

	glDisable(GL_LIGHTING);
	transMtx = glm::translate(glm::mat4(), glm::vec3(0, -0.2, 0)); // Draws the light
	glMultMatrixf(&transMtx[0][0]);
	glColor3ub(255, 255, 0);
	CSCI441::drawSolidCube(0.2);
	glEnable(GL_LIGHTING);

	transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0.2, 0)); // Undoes transformations
	glMultMatrixf(&transMtx[0][0]);
	scaleMtx = glm::scale(glm::mat4(), glm::vec3(1.0/3, 1, 0.5));
	glMultMatrixf(&scaleMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(-0.35, -4.9, 0));
	glMultMatrixf(&transMtx[0][0]);
	transMtx = glm::translate(glm::mat4(), glm::vec3(-10, 0, -10));
	glMultMatrixf(&transMtx[0][0]);
}


// generateEnvironmentDL() /////////////////////////////////////////////////////
//
//  This function creates a display list with the code to draw a simple
//      environment for the user to navigate through.
//
//  And yes, it uses a global variable for the display list.
//  I know, I know! Kids: don't try this at home. There's something to be said
//      for object-oriented programming after all.
//
////////////////////////////////////////////////////////////////////////////////
void generateEnvironmentDL() {
	environmentDL = glGenLists(1);
	glNewList(environmentDL, GL_COMPILE);
		drawGrid();
	glEndList();
}

//
//	void renderScene()
//
//		This method will contain all of the objects to be drawn.
//
void renderScene(void)  {
	glCallList(environmentDL);
	drawCharacter();
	drawLamppost();
	
	glColor3ub(0, 255, 0);
	for(unsigned int i = 0; i < controlPoints.size(); i++){
		transMtx = glm::translate(glm::mat4(), glm::vec3(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z));
		glMultMatrixf(&transMtx[0][0]);
		glLoadName(i);
		CSCI441::drawSolidSphere(0.3, 20, 20);
		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	}
	
	glDisable(GL_LIGHTING);
	
	glLineWidth(3);
	glColor3ub(0, 0, 255);
	glBegin(GL_LINE_LOOP);
	for(unsigned int i = 0; i < controlPoints.size(); i++){
		glVertex3f(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
	}
	glEnd();
	glLineWidth(1);
	
	glColor3ub(255, 255, 0);
	for(unsigned int i = 0; i + 1 < controlPoints.size(); i+=3){
		renderBezierCurve(controlPoints[i], controlPoints[i + 1], controlPoints[i + 2], controlPoints[i + 3], 20);
	}

	glEnable(GL_LIGHTING);



}

//*************************************************************************************
//
// Setup Functions

//
//  void setupGLFW()
//
//      Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
GLFWwindow* setupGLFW() {
	// set what function to use when registering errors
	// this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
	// all other GLFW calls must be performed after GLFW has been initialized
	glfwSetErrorCallback( error_callback );

	// initialize GLFW
	if( !glfwInit() ) {
		fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW initialized\n" );
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );	// request OpenGL v2.X
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );	// request OpenGL v2.1
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );		// do not allow our window to be able to be resized

	// create a window for a given size, with a given title
	GLFWwindow *window = glfwCreateWindow( windowWidth, windowHeight, "Super Mario Kart", NULL, NULL );
	if( !window ) {						// if the window could not be created, NULL is returned
		fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW Window created\n" );
	}

	glfwMakeContextCurrent(window);		// make the created window the current window
	glfwSwapInterval(1);				     	// update our screen after at least 1 screen refresh

	glfwSetKeyCallback( window, keyboard_callback );							// set our keyboard callback function
	glfwSetCursorPosCallback( window, cursor_callback );					// set our cursor position callback function
	glfwSetMouseButtonCallback( window, mouse_button_callback );	// set our mouse button callback function
	glfwSetScrollCallback(window, scroll_callback);					// set a scholl wheel callback function

	return window;						       // return the window that was created
}

//
//  void setupOpenGL()
//
//      Used to setup everything OpenGL related.  For now, the only setting
//	we need is what color to make the background of our window when we clear
//	the window.  In the future we will be adding many more settings to this
//	function.
//
void setupOpenGL() {
	// tell OpenGL to perform depth testing with the Z-Buffer to perform hidden
	//		surface removal.  We will discuss this more very soon.
	glEnable( GL_DEPTH_TEST );

	//******************************************************************
	// this is some code to enable a default light for the scene;
	// feel free to play around with this, but we won't talk about
	// lighting in OpenGL for another couple of weeks yet.
	float lightCol[4] = { 1, 1, 1, 1};
	float ambientCol[4] = { 0.0, 0.0, 0.0, 1.0 };
	float lPosition[4] = { 10, 10, 10, 1 };
	glLightfv( GL_LIGHT0, GL_POSITION,lPosition );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,lightCol );
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientCol );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	// tell OpenGL not to use the material system; just use whatever we
	// pass with glColor*()
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	//******************************************************************

	// tells OpenGL to blend colors across triangles. Once lighting is
	// enabled, this means that objects will appear smoother - if your object
	// is rounded or has a smooth surface, this is probably a good idea;
	// if your object has a blocky surface, you probably want to disable this.
	glShadeModel( GL_SMOOTH );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// set the clear color to black
}

//
//  void setupScene()
//
//      Used to setup everything scene related.  Give our camera an
//	initial starting point and generate the display list for our city
//
void setupScene() {
	// Initialize animateVals vector to store different animation settings
	animateVals.push_back(-0.5);
	animateVals.push_back(-0.4);
	animateVals.push_back(-0.3);
	animateVals.push_back(-0.2);
	animateVals.push_back(-0.1);
	animateVals.push_back(0.0);
	animateVals.push_back(0.1);
	animateVals.push_back(0.2);
	animateVals.push_back(0.3);
	animateVals.push_back(0.4);
	animateVals.push_back(0.5);
	animateVals.push_back(0.4);
	animateVals.push_back(0.3);
	animateVals.push_back(0.2);
	animateVals.push_back(0.1);
	animateVals.push_back(0.0);
	animateVals.push_back(-0.1);
	animateVals.push_back(-0.2);
	animateVals.push_back(-0.3);
	animateVals.push_back(-0.4);
	
	// give the camera a scenic starting point.
	camPos.x = 5;
	camPos.y = 5;
	camPos.z = 5;
	cameraTheta = -M_PI / 3.0f;
	cameraPhi = M_PI / 2.8f;

	// place the hero in a default position
	heroPos = glm::vec3(0, 0.3, 0);
	heroAngle = 0.0f;
	heroDir = glm::vec3(0, 0, 1);
	recomputeOrientation();

	srand( time(NULL) );	// seed our random number generator
	generateEnvironmentDL();
}

///*************************************************************************************
//
// Our main function

//
//	int main( int argc, char *argv[] )
//
//		Really you should know what this is by now.  We will make use of the parameters later
//
int main( int argc, char *argv[] ) {
	if(argc != 2){
		fprintf(stderr, "[ERROR]: Control point CSV not passed into command line\n");
		exit(EXIT_FAILURE);
	}

	loadControlPoints(argv[1]);

	// GLFW sets up our OpenGL context so must be done first
	GLFWwindow *window = setupGLFW();	// initialize all of the GLFW specific information releated to OpenGL and our window
	setupOpenGL();										// initialize all of the OpenGL specific information
	setupScene();											// initialize objects in our scene

	//  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
	//	until the user decides to close the window and quit the program.  Without a loop, the
	//	window will display once and then the program exits.
	while( !glfwWindowShouldClose(window) ) {	// check if the window was instructed to be closed
		glDrawBuffer( GL_BACK );				// work with our back frame buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

		// update the projection matrix based on the window size
		// the GL_PROJECTION matrix governs properties of the view coordinates;
		// i.e. what gets seen - use a perspective projection that ranges
		// with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
		glm::mat4 projMtx = glm::perspective( 45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.001f, 1000.0f );
		glMatrixMode( GL_PROJECTION );	// change to the Projection matrix
		glLoadIdentity();				// set the matrix to be the identity
		glMultMatrixf( &projMtx[0][0] );// load our orthographic projection matrix into OpenGL's projection matrix state

		// Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
		// when using a Retina display the actual window can be larger than the requested window.  Therefore
		// query what the actual size of the window we are rendering to is.
		GLint framebufferWidth, framebufferHeight;
		glfwGetFramebufferSize( window, &framebufferWidth, &framebufferHeight );

		// update the viewport - tell OpenGL we want to render to the whole window
		glViewport( 0, 0, framebufferWidth, framebufferHeight );

		glMatrixMode( GL_MODELVIEW );	// make the ModelView matrix current to be modified by any transformations
		glLoadIdentity();							// set the matrix to be the identity

		// set up our look at matrix to position our camera
		glm::mat4 viewMtx = glm::lookAt( camPos,		// position camera is located
										 heroPos + glm::vec3(0, 1, 0),		// where the camera is looking (at the hero)
										 glm::vec3(  0,  1,  0 ) );		// up vector is (0, 1, 0) - positive Y
		// multiply by the look at matrix - this is the same as our view martix
		glMultMatrixf( &viewMtx[0][0] );

		renderScene();					// draw everything to the window

		// Checks what direction the camera is moving (if any) and recomputes camera orientation
		if(cameraIn){
			camDistance -= 0.2;
			recomputeOrientation();
			if(camDistance < 5){
				camDistance = 5;
				recomputeOrientation();
			}
		}
		else if(cameraOut){
			camDistance += 0.2;
			recomputeOrientation();
			if(camDistance > 15){
				camDistance = 15;
				recomputeOrientation();
			}
		}
		else if(cameraLeft){
			cameraTheta += 0.05;
			recomputeOrientation();
		}
		else if(cameraRight){
			cameraTheta -= 0.05;
			recomputeOrientation();
		}

		// Checks what the hero is doing, and moves/animates the hero accordingly
		if(walking && turning){
			heroPos = heroPos + (direction * walkSpeed * heroDir);
			heroAngle += turnDirection * turnSpeed;
			recomputeOrientation();
			checkBounds();

			animateIndex = (animateIndex + 1) % animateVals.size();
			animationFrame = animateVals[animateIndex];
		}
		else if(walking){
			heroPos = heroPos + (direction * walkSpeed * heroDir);
			recomputeOrientation();
			checkBounds();

			animateIndex = (animateIndex + 1) % animateVals.size();
			animationFrame = animateVals[animateIndex];
		}
		else if(turning){
			heroAngle += turnDirection * turnSpeed;
			recomputeOrientation();

			animateIndex = (animateIndex + 1) % animateVals.size();
			animationFrame = animateVals[animateIndex];
		}
		else{
			animationFrame = 0; // Default state for the hero
			animateIndex = 5;
		}



		glfwSwapBuffers(window);// flush the OpenGL commands and make sure they get rendered!
		glfwPollEvents();				// check for any events and signal to redraw screen
	}

	glfwDestroyWindow( window );// clean up and close our window
	glfwTerminate();						// shut down GLFW to clean up our context

	return EXIT_SUCCESS;				// exit our program successfully!
}
