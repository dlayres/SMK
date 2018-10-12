#ifndef _ALEX_H
#define _ALEX_H

#include "HeroBase.h"

class Alex : public HeroBase {

private:	
	float headRadius, stickRadius, torsoHeight, armLength, armAngle, legLength, legAngle;

	void drawHead() {
		glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(0.0f, torsoHeight + headRadius - 0.25f, 0.0f));
		glMultMatrixf(&translate[0][0]);

			// HEAD
			CSCI441::drawSolidSphere(headRadius, 10, 10);

			// NOSE
			glm::mat4 noseTranslate = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, headRadius));
			glMultMatrixf(&noseTranslate[0][0]);

				glColor3f(1.0f, 1.0f, 0.0f);
				CSCI441::drawSolidCube(1.5f);

			glMultMatrixf(&(glm::inverse(noseTranslate))[0][0]);

		glMultMatrixf(&(glm::inverse(translate))[0][0]);
	}

	void drawTorso() {
		CSCI441::drawSolidCylinder(stickRadius, stickRadius, torsoHeight, 10, 10);
	}

	void drawArm(float armAngle) {
		glm::mat4 leftRotate = glm::rotate(glm::mat4(), armAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		glMultMatrixf(&leftRotate[0][0]);

			CSCI441::drawSolidCylinder(stickRadius * 0.2f, stickRadius, armLength, 10, 10);

		glMultMatrixf(&(glm::inverse(leftRotate))[0][0]);
	}

	void drawArms() {
		// Move arms toward top of torso
		glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(0.0f, torsoHeight, 0.0f));
		glMultMatrixf(&translate[0][0]);

			// LEFT ARM
			drawArm(armAngle);

			// RIGHT ARM
			drawArm(-armAngle);

		glMultMatrixf(&(glm::inverse(translate))[0][0]);
	}

	void drawLeg(float legAngle) {
		glm::mat4 rotate = glm::rotate(glm::mat4(), legAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		glMultMatrixf(&rotate[0][0]);

			CSCI441::drawSolidCylinder(stickRadius * 0.8f, stickRadius, legLength, 10, 10);

		glMultMatrixf(&(glm::inverse(rotate))[0][0]);
	}

	void drawLegs() {
		//LEFT LEG
		drawLeg(legAngle);

		// RIGHT LEG
		drawLeg(-legAngle);
	}
public:
	Alex(glm::vec3 pos) : HeroBase(pos) {
		headRadius = 4.0f;
		stickRadius = 1.0f;
		torsoHeight = 10.0f;
		armLength = 7.5f;
		armAngle = (float)M_PI / 1.25f;
		legLength = 10.0f;
		legAngle = (float)M_PI / 1.1f;
	}

	void draw(bool animate) {
		glm::mat4 posTrans = glm::translate(glm::mat4(), pos);
		glMultMatrixf(&posTrans[0][0]);

		glm::mat4 yawRotate = glm::rotate(glm::mat4(), yaw, glm::vec3(0, 1, 0));
		glMultMatrixf(&yawRotate[0][0]);

			glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(0.07f, 0.07f, 0.07f));
			glMultMatrixf(&scale[0][0]);
			
				// Translate so feet are on the ground.
				glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(0.0f, legLength, 0.0f));
				glMultMatrixf(&translate[0][0]);

					glColor3f(0.0f, 1.0f, 0.0f);
					drawHead();

					glColor3f(1.0f, 0.0f, 0.0f);
					drawTorso();

					glColor3f(1.0f, 0.0f, 1.0f);
					drawArms();

					glColor3f(0.0f, 0.0f, 1.0f);
					float walkingAngle = animate ? (float)sin(glfwGetTime() * 5.0f) : 0.0f;
					glm::mat4 walkingRot = glm::rotate(glm::mat4(), walkingAngle, glm::vec3(0.0f, 1.0f, 0.0f));
					glMultMatrixf(&walkingRot[0][0]);

						drawLegs();

					glMultMatrixf(&(glm::inverse(walkingRot))[0][0]);

				glMultMatrixf(&(glm::inverse(translate))[0][0]);
			glMultMatrixf(&(glm::inverse(scale))[0][0]);
			
		glMultMatrixf(&(glm::inverse(yawRotate))[0][0]);
		glMultMatrixf(&(glm::inverse(posTrans))[0][0]);
	}
};

#endif