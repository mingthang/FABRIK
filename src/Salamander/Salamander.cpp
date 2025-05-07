#include "Salamander.h"
#include "glm/gtc/matrix_transform.hpp"
#include <list>
#include <vector>
#include <Utils/Util.h>
#include <Utils/Debug.h>
#include <Renderer/Types/Model2D.h>
#include <AssetManager/AssetManager.h>
#include <Renderer/Renderer.h>
#include <iostream>

Salamander::Salamander(const SalamanderConfig& config) {
	for (int i = 0; i < config.numJoints; ++i) {
		float y = config.startY - i * config.yStep;
		float width = config.startWidth - i * config.widthStep;

		// Check for width override
		if (config.overrideWidths.find(i) != config.overrideWidths.end()) {
			width = config.overrideWidths.at(i);
		}

		AddBodyJoint(400.0f, y, width);
	}

	// prevJoint pointers setup
	for (size_t i = 1; i < m_bodyJoints.size(); ++i) {
		m_bodyJoints[i].prevJoint = &m_bodyJoints[i - 1];
		m_distances.push_back(glm::distance(
			m_bodyJoints[i].position,
			m_bodyJoints[i - 1].position
		));
	}

	// Add legs
	for (int idx : config.legIndices) {
		AddLeg(idx);
		m_legIndices.push_back(idx);
	}
}

void Salamander::AddBodyJoint(float x, float y, float width, bool bHasLegs)
{
	//Joint* prevJoint = m_bodyJoints.empty() ? nullptr : &m_bodyJoints.back();
	m_bodyJoints.emplace_back(x, y, width, nullptr);
}

void Salamander::AddLeg(int bodyIndex) {
	if (bodyIndex > 0 && bodyIndex < m_bodyJoints.size())
	{
		m_legs.emplace_back(&m_bodyJoints[bodyIndex], ELegSide::LEFT);
		m_legs.emplace_back(&m_bodyJoints[bodyIndex], ELegSide::RIGHT);
	}
}

void Salamander::Draw() {

	this->IK = bIK;
	this->torque = btorque;

	switch (g_DebugMode) {
	case DebugMode::A:
		for (size_t i = 0; i < m_bodyJoints.size(); ++i) {
			Renderer::DrawPoint(m_bodyJoints[i].position);
		}
		break;
	case DebugMode::B:
		for (size_t i = 0; i < m_bodyJoints.size() - 1; ++i) {
			glm::vec2 p0 = m_bodyJoints[i].position;
			glm::vec2 p1 = m_bodyJoints[i + 1].position;
			Renderer::DrawLine(p0, p1);
			if (std::find(m_legIndices.begin(), m_legIndices.end(), i) != m_legIndices.end())
				Renderer::DrawCircle(m_bodyJoints[i].position, m_bodyJoints[i].width * 0.25f);
			else
				Renderer::DrawCircle(m_bodyJoints[i].position, m_bodyJoints[i].width * 0.25f);
		}
		break;
	case DebugMode::C:
		for (size_t i = 0; i < m_bodyJoints.size() - 1; ++i) {
			glm::vec2 p0 = m_bodyJoints[i].position;
			glm::vec2 p1 = m_bodyJoints[i + 1].position;
			Renderer::DrawLine(p0, p1);
			if (std::find(m_legIndices.begin(), m_legIndices.end(), i) != m_legIndices.end())
				Renderer::DrawCircle(m_bodyJoints[i].position, m_bodyJoints[i].width * 0.25f, glm::vec3(0.294f, 0.0f, 0.51f), true);
			else 
				Renderer::DrawCircle(m_bodyJoints[i].position, m_bodyJoints[i].width * 0.25f);
		}
		// Draw legs
		if (IK) {
			for (const auto& leg : m_legs) {
				glm::vec2 p0 = leg.bodyJoint->position;
				glm::vec2 p1 = leg.elbow.position;
				glm::vec2 p2 = leg.foot.position;

				const int segmentCount = 6;
				for (int i = 0; i < segmentCount; ++i) {
					float t0 = float(i) / segmentCount;
					float t1 = float(i + 1) / segmentCount;

					glm::vec2 a = Util::QuadraticBezier(p0, p1, p2, t0);
					glm::vec2 b = Util::QuadraticBezier(p0, p1, p2, t1);

					Renderer::DrawLine(a, b);
				}

				Renderer::DrawCircle(leg.foot.position, leg.footWidth, glm::vec3(0.8f, 0.0f, 0.8f), true);
			}
		}
		else {
			for (const auto& leg : m_legs) {
				glm::vec2 p0 = leg.bodyJoint->position;
				glm::vec2 p1 = leg.foot.position;

				Renderer::DrawLine(p0, p1);
				Renderer::DrawCircle(leg.foot.position, leg.footWidth, glm::vec3(0.8f, 0.0f, 0.8f), true);
			}
		}
		break;
	}
}

void Salamander::Move(glm::vec2 target, float deltaTime)
{
	if (m_bodyJoints.empty()) return;

	float dist = glm::distance(m_bodyJoints[0].position, target);
	if (dist <= 0.1f)
		return;

	// Move the base towards the target smoothly
	if (glm::length(target - m_bodyJoints[0].position) < 0.000001f)
	{
		m_bodyJoints[0].position = target;
	}
	else
	{
		glm::vec2 direction = glm::normalize(target - m_bodyJoints[0].position);
		m_bodyJoints[0].position += direction * m_speed * deltaTime; // Control speed
	}
	// Forward Kinematics: Adjust all m_bodyJoints relative to the base
	if (m_bodyJoints.size() > 1) {
		glm::vec2 baseToJoint = m_bodyJoints[1].position - m_bodyJoints[0].position;
		float cumulativeAngle = glm::atan(baseToJoint.y, baseToJoint.x);
		m_bodyJoints[1].position = m_bodyJoints[0].position + glm::normalize(baseToJoint) * m_distances[0];

		for (size_t i = 2; i < m_bodyJoints.size(); i++) {
			glm::vec2 currentDir = glm::normalize(m_bodyJoints[i].position - m_bodyJoints[i - 1].position);
			float desiredAngle = glm::atan(currentDir.y, currentDir.x);
			float angleDiff = desiredAngle - cumulativeAngle;

			if (angleDiff > glm::pi<float>())
				angleDiff -= 2.0f * glm::pi<float>();
			else if (angleDiff < -glm::pi<float>())
				angleDiff += 2.0f * glm::pi<float>();

			float maxAngle = 0.5f;
			if (std::abs(angleDiff) > maxAngle)
				angleDiff = glm::sign(angleDiff) * maxAngle;

			cumulativeAngle += angleDiff;

			glm::vec2 newDir = glm::vec2(glm::cos(cumulativeAngle), glm::sin(cumulativeAngle));

			m_bodyJoints[i].position = m_bodyJoints[i - 1].position + newDir * m_distances[i - 1];
		}
	}


	// Leg Update
	for (Leg& leg : m_legs)
	{
		if (!leg.isStepping) // If the leg is not currently stepping
		{
			glm::vec2 newStepPos = leg.ComputeStepPosition();

			float distanceToStepPos = glm::distance(leg.foot.position, newStepPos);
	
			if (distanceToStepPos > leg.legLength * 1.1f) // Step if it's too far
			{
				// Ensure alternating step order and prevent simultaneous steps for the same joint
				if (!leg.bodyJoint->isLegStepping && leg.bodyJoint->lastSteppedLeg != leg.side)
				{
					leg.stepPos = newStepPos;
					leg.isStepping = true;
					leg.bodyJoint->isLegStepping = true; // Mark that this joint is stepping
					leg.bodyJoint->lastSteppedLeg = leg.side; // Update stepping order
				}
			}
		}

		if (leg.isStepping) // Move the leg if stepping
		{
			float legspeed = m_speed * 3.0f;
			float distanceToStep = glm::distance(leg.foot.position, leg.stepPos);
			float moveDist = legspeed * deltaTime;
			glm::vec2 target;
			leg.stepPos = leg.ComputeStepPosition();

			if (!IK) {
				if (moveDist >= distanceToStep) {
					target = leg.stepPos;
					leg.isStepping = false;
					leg.bodyJoint->isLegStepping = false;
				}
				else {
					glm::vec2 direction = glm::normalize(leg.stepPos - leg.foot.position);
					target = leg.foot.position + direction * moveDist;
				}

				leg.foot.position = target;
			}

			else {
				if (moveDist >= distanceToStep) {
					target = leg.stepPos;
					leg.isStepping = false;
					leg.bodyJoint->isLegStepping = false;
				}
				else {
					glm::vec2 direction = glm::normalize(leg.stepPos - leg.foot.position);
					target = leg.foot.position + direction * moveDist;
				}

				// TODO: Fix this mess
				std::vector<Joint> joints = {
					*leg.bodyJoint,
					leg.elbow,
					leg.foot
				};
				std::vector<float> distances = {
					leg.legLength * 0.5f, leg.legLength * 0.5f
				};

				FABRIK::SolveSEE(joints, distances, target);

				leg.elbow.position = joints[1].position;
				leg.foot.position = joints[2].position;
			}

			// Apply torque
			if (torque) {
				glm::vec2 pullVector = leg.foot.position - leg.bodyJoint->position;
				float len = glm::length(pullVector);
				if (len > 0.001f)
				{
					glm::vec2 pullDir = pullVector / len;
					glm::vec2 rawOffset = pullDir * m_pullStrength;
					int jointIndex = -1;
					for (size_t i = 0; i < m_bodyJoints.size(); ++i) {
						if (&m_bodyJoints[i] == leg.bodyJoint) {
							jointIndex = static_cast<int>(i);
							break;
						}
					}

					int count = m_bodyJoints.size() - jointIndex;

					for (size_t j = jointIndex; j < m_bodyJoints.size(); j++)
					{
						float factor = 1.0f - float(j - jointIndex) / float(count);
						glm::vec2 desired = m_bodyJoints[j].position + rawOffset * factor;
						float damp = 0.2f;
						m_bodyJoints[j].position = glm::mix(m_bodyJoints[j].position, desired, damp);
					}
				}
			}
		}
	}

}