#pragma once
#include <glm/glm.hpp>

const float EPSILON = 1e-5f;

struct Joint
{
	glm::vec2 position;
	float width;

	Joint(float x, float y, const float w) : position(glm::vec2(x, y)), width(w) {}
};
