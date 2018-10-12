#ifndef _SAV_H
#define _SAV_H

#include "HeroBase.h"

class Sav : public HeroBase {

private:
	float inc;
	bool moving;

	void drawWheels() {
		//draw and place wheels
		glColor3f(0.73, 0.73, 0.73);

		glPushMatrix();
		glTranslatef(0, 1.6, -5.5);
		glRotatef(90, 1, 0, 0);
		CSCI441::drawSolidCylinder(1.0, 1.0, 2.0, 50, 50);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 1.6, -5.5);
		CSCI441::drawSolidSphere(1, 50, 50);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 1.6, -3.5);
		CSCI441::drawSolidSphere(1, 50, 50);
		glPopMatrix();

		//back wheels
		glPushMatrix();
		glTranslatef(0, 1.6, 3.5);
		glRotatef(90, 1, 0, 0);
		CSCI441::drawSolidCylinder(1.0, 1.0, 2.0, 50, 50);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, 1.6, 3.5);
		CSCI441::drawSolidSphere(1, 50, 50);
		glPopMatrix();

		//fire tires
		//tire 1
		glPushMatrix();
		glColor3f(1.0, 0.0, 0.0);
		glTranslatef(0, 1.6, 5.5);
		glRotatef(90, 1, 0, 0);
		if (moving) {
			glTranslatef(0, inc, 0);
		}
		CSCI441::drawSolidCylinder(1.0, 0.0, 2.0, 50, 50);
		glTranslatef(0, 1, 0);
		glColor3f(.93, .48, .13);
		if (moving) {
			glTranslatef(0, inc, 0);
		}
		CSCI441::drawSolidCylinder(.8, 0.0, 1.8, 50, 50);
		glTranslatef(0, 1, 0);
		glColor3f(.88, .88, .05);
		if (moving) {
			glTranslatef(0, inc, 0);
			inc += .25;
			if (inc > 1) {
				inc = 0;
			}
		}
		CSCI441::drawSolidCylinder(.5, 0.0, 1.0, 50, 50);
		glPopMatrix();
	}

	void drawHandleBars() {
		//handle bars
		glColor3f(.7, .48, .47);
		glPushMatrix();
		glTranslatef(0, 2.5, -5);
		CSCI441::drawSolidCylinder(0.5, 0.5, 8, 50, 50);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(3, 10.5, -5);
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 0, 1);
		CSCI441::drawSolidCylinder(0.5, 0.5, 6, 50, 50);
		glPopMatrix();

		//balls on end of handle bars
		glPushMatrix();
		glTranslatef(3, 10.5, -5);
		CSCI441::drawSolidSphere(0.5, 50, 50);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-3, 10.5, -5);
		CSCI441::drawSolidSphere(0.5, 50, 50);
		glPopMatrix();

		//fringe 
		glColor3f(.83, .69, .22);
		glPushMatrix();
		glTranslatef(3.5, 10.5, -5);
		glRotatef(180, 1, 0, 0);
		CSCI441::drawWireCylinder(0., 0.25, 3, 10, 10);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(3.5, 10.5, -5);
		glRotatef(170, 1, 0, 1);
		CSCI441::drawWireCylinder(0.0, 0.25, 3, 10, 10);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(3.5, 10.5, -5);
		glRotatef(190, 1, 0, 1);
		CSCI441::drawWireCylinder(0., 0.25, 3, 10, 10);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-3.5, 10.5, -5);
		glRotatef(180, 1, 0, 0);
		CSCI441::drawWireCylinder(0., 0.25, 3, 10, 10);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-3.5, 10.5, -5);
		glRotatef(170, 1, 0, 1);
		CSCI441::drawWireCylinder(0.0, 0.25, 3, 10, 10);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-3.5, 10.5, -5);
		glRotatef(190, 1, 0, 1);
		CSCI441::drawWireCylinder(0., 0.25, 3, 10, 10);
		glPopMatrix();

	}
	
public:
	Sav(glm::vec3 pos) : HeroBase(pos) {
		inc = .5;
		moving = false;
	}

	void draw(bool animate) { 				  
		glPushMatrix();
		glRotatef(-90, 0, 1, 0);
		glColor3f(.7, .48, .47);
		//glColor3f(.64, .53, .82);
		for (int i = -5; i <= 5; i++) {
			for (int j = -1; j <= 1; j++) {
				glPushMatrix();
				glTranslatef(j, 3, i);
				CSCI441::drawSolidCube(1);
				glPopMatrix();
			}
		}

		drawWheels();

		drawHandleBars();

		glPopMatrix();
	}
};

#endif