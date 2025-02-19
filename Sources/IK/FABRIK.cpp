#include "FABRIK.h"

// Single End Effector
void FABRIK::SolveSEE(std::vector<Joint>& joints, std::vector<float>& distances, glm::vec2 target)
{
	if (joints.size() <= 1)
		return;

	glm::vec2 base = joints[0].position;

	float totalDist = 0; // Total chain distance
	for (float d : distances)
		totalDist += d;
	float distBT = glm::distance(joints[0].position, target); // root -> target distance

	// Target is reachable
	if (distBT <= totalDist)
	{
		float distET = glm::distance(joints[joints.size()-1].position, target); // EE -> target distance
		while (distET > EPSILON)
		{
			// STAGE 1: FORWARD REACHING
			joints[joints.size()-1].position = target;
			for (int i = joints.size() - 2; i >= 0; --i)
			{
				glm::vec2 direction = glm::normalize(joints[i + 1].position - joints[i].position);
				joints[i].position = joints[i + 1].position - direction * distances[i];
			}
			// STAGE 2: BACKWARD REACHING
			joints[0].position = base;
			for (int i = 0; i <= (joints.size() - 2); ++i) 
			{
				glm::vec2 direction = glm::normalize(joints[i].position - joints[i + 1].position);
				joints[i + 1].position = joints[i].position - direction * distances[i];
			}
			distET = glm::distance(joints[joints.size() - 1].position, target);
		}
	}
	// Target is unreachable
	else
	{
		glm::vec2 direction = glm::normalize(target - base);
		for(int i = 1; i < joints.size(); ++i)
			joints[i].position = joints[i - 1].position + direction * distances[i - 1];
	}
}
