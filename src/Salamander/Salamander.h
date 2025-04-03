#pragma once
#include "IK/Joint.h"
#include "Salamander/Leg.h"
#include "Utils/Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <list>
#include <vector>
#include "Utils/Util.h"

class Salamander
{
public:

	std::vector<Joint> bodyJoints;
	std::vector<float> distances;
	std::vector<Leg> legs;
	float speed = 5.0f;
	Shader* circleShader;

	Salamander(Shader* circleShader): circleShader(circleShader) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &circleVAO);
		glGenBuffers(1, &circleVBO);
		glGenVertexArrays(1, &legVAO);
		glGenBuffers(1, &legVBO);

		bodyJoints.reserve(100); // Reserve space for 100 joints

		GenerateCircleVertices();
	}

	~Salamander() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &circleVBO);
		glDeleteVertexArrays(1, &circleVAO);
		glDeleteBuffers(1, &legVBO);
		glDeleteVertexArrays(1, &legVAO);
	}	

	void AddBodyJoint(float x, float y, float width, bool hasLegs = false)
	{
		Joint* prevJoint = bodyJoints.empty() ? nullptr : &bodyJoints.back();
		bodyJoints.emplace_back(x, y, width, prevJoint);
		if (bodyJoints.size() > 1)
		{
			auto last = std::prev(bodyJoints.end());
			auto secondLast = std::prev(last);
			distances.push_back(glm::distance(last->position, secondLast->position));
		}
	}

	void AddLeg(int bodyIndex) {
		if (bodyIndex > 0 && bodyIndex < bodyJoints.size()) 
		{
			legs.emplace_back(&bodyJoints[bodyIndex], ELegSide::LEFT);
			legs.emplace_back(&bodyJoints[bodyIndex], ELegSide::RIGHT);
		}
	}

	void UpdateBuffer()
	{
		vertices.clear();
		for (const auto& joint : bodyJoints)
			vertices.push_back(joint.position);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		legVertices.clear();
		for (const auto& leg : legs)
		{
			legVertices.push_back(leg.bodyJoint->position);
			legVertices.push_back(leg.elbow.position);
			legVertices.push_back(leg.foot.position);
		}

		glBindVertexArray(legVAO);
		glBindBuffer(GL_ARRAY_BUFFER, legVBO);
		glBufferData(GL_ARRAY_BUFFER, legVertices.size() * sizeof(glm::vec2), legVertices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void GenerateCircleVertices(int segments = 12) {
		circleVertices.clear();
		float angleStep = 2.0f * 3.1415926f / float(segments);

		for (int i = 0; i < segments; i++) {
			float angle = i * angleStep;
			circleVertices.push_back(0.1f * glm::vec2(cos(angle), sin(angle)));
		}

		glBindVertexArray(circleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
		glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(glm::vec2), circleVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Draw()
	{
		UpdateBuffer();

		glBindVertexArray(VAO);
		glLineWidth(3.0f);
		glPointSize(10.0f);

		// Draw the body joints as line strips
		//glDrawArrays(GL_POINTS, 0, vertices.size());
		glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
		glBindVertexArray(0);

		// Draw the body joints as circles
		circleShader->Use();
		glBindVertexArray(circleVAO);
		for (const auto& joint : bodyJoints)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(joint.position, 0.0f)); 
			model = glm::scale(model, glm::vec3(joint.width * 0.5f, joint.width * 0.5f, 1.0f)); 

			circleShader->SetMat4("model", model);
			glDrawArrays(GL_LINE_LOOP, 0, circleVertices.size());
		}
		glUseProgram(0);

		// Draw the legs
		glBindVertexArray(legVAO);
		for (size_t i = 0; i < legVertices.size(); i += 3) 
		{
			glDrawArrays(GL_LINE_STRIP, i, 3); 
		}
		glBindVertexArray(0);
		// Draw foot circles
		for (const auto& leg : legs)
		{
			circleShader->Use();
			glBindVertexArray(circleVAO);
			glm::mat4 footModel = glm::mat4(1.0f);
			footModel = glm::translate(footModel, glm::vec3(leg.foot.position, 0.0f));
			footModel = glm::scale(footModel, glm::vec3(leg.footWidth, leg.footWidth, 1.0f));

			circleShader->SetMat4("model", footModel);
			glDrawArrays(GL_LINE_LOOP, 0, circleVertices.size());
		}
		glUseProgram(0);

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void Move(glm::vec2 target, float deltaTime)
	{
		if (bodyJoints.empty()) return;

		float pullStrength = 0.002f;

		// Leg Update
		for (auto& leg : legs)
		{
			if (!leg.isStepping) // If the leg is not currently stepping
			{
				glm::vec2 stepPos = leg.ComputeStepPosition();
				leg.stepPos = stepPos;

				float distanceToStepPos = glm::distance(leg.foot.position, leg.stepPos);
				if (distanceToStepPos > leg.legLength * 1.25f) // Step if it's too far
				{
					// Ensure alternating step order and prevent simultaneous steps for the same joint
					if (!leg.bodyJoint->isLegStepping && leg.bodyJoint->lastSteppedLeg != leg.side)
					{
						leg.isStepping = true;
						leg.bodyJoint->isLegStepping = true; // Mark that this joint is stepping
						leg.bodyJoint->lastSteppedLeg = leg.side; // Update stepping order
					}
				}
			}

			if (leg.isStepping) // Move the leg if stepping
			{
				float legSpeed = speed * 2.0f;
				glm::vec2 direction = leg.stepPos - leg.foot.position; // no need to normalize because of OpenGL 
				glm::vec2 velocity = direction * 0.125f;
				glm::vec2 target = leg.foot.position + velocity;
				leg.SolveLegIK(target);

				if (glm::distance(leg.foot.position, leg.stepPos) < 0.01f) // Threshold check
				{
					leg.SolveLegIK(leg.stepPos);
					leg.isStepping = false;
					leg.bodyJoint->isLegStepping = false;
				}

				// Torque effect
				glm::vec2 pullVector = leg.foot.position - leg.bodyJoint->position;
				if (glm::length(pullVector) > 0.0001f)
				{
					glm::vec2 pullOffset = glm::normalize(pullVector) * pullStrength;
					int jointIndex = -1;
					for (size_t i = 0; i < bodyJoints.size(); i++)
					{
						if (&bodyJoints[i] == leg.bodyJoint)
						{
							jointIndex = i;
							break;
						}
					}

					if (jointIndex != -1)
					{
						int count = bodyJoints.size() - jointIndex;
						for (size_t j = jointIndex; j < bodyJoints.size(); j++)
						{
							// Pull strength decreasing
							float factor = 1.0f - float(j - jointIndex) / float(count);
							bodyJoints[j].position += pullOffset * factor;
						}
					}
				}
			}
		}

		float dist = glm::distance(bodyJoints[0].position, target);
		if (dist <= 0.1f)
			return;


		// Move the base towards the target smoothly
		if (glm::length(target - bodyJoints[0].position) < 0.000001f)
		{
			bodyJoints[0].position = target;
		}
		else 
		{
			glm::vec2 direction = glm::normalize(target - bodyJoints[0].position) * 0.1f;
			bodyJoints[0].position += direction * speed * deltaTime; // Control speed
		}

		// Forward Kinematics: Adjust all bodyJoints relative to the base
		if (bodyJoints.size() > 1) {
			glm::vec2 baseToJoint = bodyJoints[1].position - bodyJoints[0].position;
			float cumulativeAngle = glm::atan(baseToJoint.y, baseToJoint.x);
			bodyJoints[1].position = bodyJoints[0].position + glm::normalize(baseToJoint) * distances[0];

			for (size_t i = 2; i < bodyJoints.size(); i++) {
				glm::vec2 currentDir = glm::normalize(bodyJoints[i].position - bodyJoints[i - 1].position);
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

				bodyJoints[i].position = bodyJoints[i - 1].position + newDir * distances[i - 1];
			}
		}
	}

private:
	GLuint VAO, VBO;
	std::vector<glm::vec2> vertices;
	GLuint circleVAO, circleVBO;
	std::vector<glm::vec2> circleVertices;
	GLuint legVAO, legVBO;
	std::vector<glm::vec2> legVertices;

	float bodyRotation = 0.0f;
};
