#pragma once

#include "Joint.h"
#include <vector>

class FABRIK
{
public:
	// Single End Effector
	static void SolveSEE(std::vector<Joint>& joints, std::vector<float>& distances, glm::vec2 target);

	// Multiple End Effector

	// Constraints
};
