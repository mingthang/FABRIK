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
	float footWidth = 7.0f;
	float legLength = 50.0f;
	// step Position
	glm::vec2 stepPos;
	bool isStepping = false;

	glm::vec2 prevFacingDir;


	Leg(Joint* bodyJoint, ELegSide side);

	glm::vec2 ComputeStepPosition();
	void SolveLegIK(glm::vec2 target);
};
