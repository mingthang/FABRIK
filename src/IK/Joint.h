#pragma once
#include <glm/glm.hpp>
#include "../Salamander/LegTypes.h"

const float EPSILON = 1e-5f;

struct Joint
{
	glm::vec2 position;
	float width;
	Joint* prevJoint;

	ELegSide lastSteppedLeg = ELegSide::RIGHT;
	bool isLegStepping = false;

	Joint(float x = 0.0f, float y = 0.0f, float w = 0.0f, Joint* prev = nullptr) : position(glm::vec2(x, y)), width(w), prevJoint(prev) {}
	Joint(glm::vec2 pos, float w, Joint* prev = nullptr): position(pos), width(w), prevJoint(prev) {}
};
