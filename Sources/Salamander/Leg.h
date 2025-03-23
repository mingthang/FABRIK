#pragma once

#include "IK/Joint.h"
#include "IK/FABRIK.h"
#include "LegTypes.h"
#include <vector>

struct Leg
{
public:
	Joint* bodyJoint;
	ELegSide side;
	Joint foot;
	Joint elbow;
	float footWidth = 0.2f;
	float legLength = 0.15f;
	// step Position
	glm::vec2 stepPos;
	bool isStepping = false;


	Leg(Joint* bodyJoint, ELegSide side);

	glm::vec2 ComputeStepPosition();
	void SolveLegIK(glm::vec2 target);
};
