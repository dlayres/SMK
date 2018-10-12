#ifndef _DAVID_H
#define _DAVID_H

#include "HeroBase.h"

class David : public HeroBase {

private:
	glm::mat4 transMtx; 								// global variables used for transformations
	glm::mat4 rotateMtx;
	glm::mat4 scaleMtx;

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
public:
	David(glm::vec3 pos) : HeroBase(pos) {
	}

	void draw(bool animate) { // Draws character from upper half and two legs
		transMtx = glm::translate(glm::mat4(), pos); // Checks the orientation of the hero and draws the upper half accordingly
		glMultMatrixf(&transMtx[0][0]);
		rotateMtx = glm::rotate(glm::mat4(), yaw, glm::vec3(0, 1, 0));
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
		rotateMtx = glm::rotate(glm::mat4(), -yaw, glm::vec3(0, 1, 0));
		glMultMatrixf(&rotateMtx[0][0]);
		transMtx = glm::translate(glm::mat4(), -pos);
		glMultMatrixf(&transMtx[0][0]);
	}
};

#endif