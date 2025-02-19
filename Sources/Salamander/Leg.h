#pragma once

#include "IK/Joint.h"
#include "IK/FABRIK.h"
#include <vector>

enum class ELegSide { LEFT, RIGHT };

struct Leg
{
public:
	Joint* bodyJoint;
	ELegSide side;
	Joint foot;
	Joint elbow;
	float footWidth = 0.2f;
	float legLength = 0.125f;

	Leg(Joint* bodyJoint, ELegSide side);

	glm::vec2 ComputeStepPosition();
	void SolveLegIK(glm::vec2 target);
};
