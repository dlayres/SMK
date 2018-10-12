#ifndef HERO_BASE_H
#define HERO_BASE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class HeroBase {

public:
	HeroBase(glm::vec3 pos) {
		this->pos = pos;
		direction = glm::vec3(0, 0, 1);
		yaw = 0.0f;
		pitch = 0.0f;
		
		camPos.x = 5;
		camPos.y = 5;
		camPos.z = 5;
		cameraTheta = -M_PI / 3.0f;
		cameraPhi = M_PI / 2.8f;
		camDistance = 4.0f;
	}

	virtual void draw(bool animate) = 0;

	void setAnimationFrame(float newFrame) {
		animationFrame = newFrame;
	}

	// Hero position and direction
	glm::vec3 pos;
	glm::vec3 direction;
	float yaw, pitch;

	// Camera position, angles, and distance
	glm::vec3 camPos;
	float cameraTheta, cameraPhi, camDistance;
protected:
	float animationFrame;
};

#endif