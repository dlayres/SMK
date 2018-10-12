#ifndef _JOSH_H
#define _JOSH_H

#include "HeroBase.h"

class Josh : public HeroBase {

private:
	float wheelAngle;

void drawWheel() {
	glColor3f(0.568, 0.329, 0.133);

	glm::mat4 rotMtx = glm::rotate(glm::mat4(), wheelAngle, glm::vec3(0, 0, 1));
	glMultMatrixf(&rotMtx[0][0]);

	CSCI441::drawSolidPartialDisk(.15, .2, 10, 10, 0, 360);

	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);
}
void drawAxle() {
	glColor3f(0.329, 0.329, 0.329);

	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, -.08, -.4));
	glMultMatrixf(&transMtx[0][0]);

	glm::mat4 rotMtx = glm::rotate(glm::mat4(), float(M_PI / 2), glm::vec3(1, 0, 0.0f));
	glMultMatrixf(&rotMtx[0][0]);

	CSCI441::drawSolidCylinder(.03, .03, .8, 10, 10);

	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);

	drawWheel();

	glm::mat4 transMtx1 = glm::translate(glm::mat4(), glm::vec3(0, 0, .8));
	glMultMatrixf(&transMtx1[0][0]);

	drawWheel();

	glMultMatrixf(&(glm::inverse(transMtx1))[0][0]);

	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);



}

	void drawCockpit() {
		glColor3f(0.541, 0.529, 0.521);
		glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(1.2, .1, .7));
		glMultMatrixf(&scaleMtx[0][0]);
		CSCI441::drawSolidCube(1);
		glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);
	}
public:
	Josh(glm::vec3 pos) : HeroBase(pos) {
		wheelAngle = 0;
	}

	void draw(bool animate) { // Draws character from upper half and two legs
		 // rotate to current heading
		wheelAngle += .5;
	//	glm::mat4 transMtx2 = glm::translate(glm::mat4(), glm::vec3(0, 7, 0));
	//	glMultMatrixf(&transMtx2[0][0]);
		
		glm::mat4 rotMtx = glm::rotate(glm::mat4(), yaw, glm::vec3(0.0f, -1.0f, 0.0f));
		glMultMatrixf(&rotMtx[0][0]);

		//draw vehicle
		glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, .27, 0));
		glMultMatrixf(&transMtx[0][0]);

		drawCockpit();
		glm::mat4 transMtx1 = glm::translate(glm::mat4(), glm::vec3(.3, 0, 0));	// move towards front
		glMultMatrixf(&transMtx1[0][0]);

		drawAxle();

		glMultMatrixf(&(glm::inverse(transMtx1))[0][0]);

		transMtx1 = glm::translate(glm::mat4(), glm::vec3(-.3, 0, 0));	//move towards back
		glMultMatrixf(&transMtx1[0][0]);

		drawAxle();

		glMultMatrixf(&(glm::inverse(transMtx1))[0][0]);

		glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

		//rotate back to heading
		glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);

		//glMultMatrixf(&(glm::inverse(transMtx2))[0][0]);
	}
};

#endif