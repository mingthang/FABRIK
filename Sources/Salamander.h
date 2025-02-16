#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Joint.h"
#include <list>
#include <vector>

class Salamander
{
public:
	std::vector<Joint> joints;
	std::vector<float> distances;
	float speed = 2.0f;

	Salamander() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
	}

	~Salamander() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}	

	void AddJoint(float x, float y, float width)
	{
		joints.emplace_back(x, y, width);
		if (joints.size() > 1)
		{
			auto last = std::prev(joints.end());
			auto secondLast = std::prev(last);
			distances.push_back(glm::distance(last->position, secondLast->position));
		}
		UpdateBuffer();
	}

	void UpdateBuffer()
	{
		vertices.clear();
		for (const auto& joint : joints)
			vertices.push_back(joint.position);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Draw()
	{
		glBindVertexArray(VAO);

		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, vertices.size());

		//glDrawArrays(GL_LINE_STRIP, 0, vertices.size());

		glBindVertexArray(0);
	}

	void Move(glm::vec2 target, float deltaTime)
	{
		float dist = glm::distance(joints[0].position, target);
		if (dist <= 0.1f)
			return;
		// Move the base towards the target smoothly
		glm::vec2 direction = glm::normalize(target - joints[0].position);
		joints[0].position += direction * speed * deltaTime; // Control speed

		// Forward Kinematics: Adjust all joints relative to the base
		for (size_t i = 1; i < joints.size(); i++)
		{
			glm::vec2 prevToCurrent = joints[i].position - joints[i - 1].position;
			glm::vec2 newDirection = glm::normalize(prevToCurrent);

			// Keep joint distance fixed
			joints[i].position = joints[i - 1].position + newDirection * distances[i - 1];
		}

		UpdateBuffer();
	}


private:
	GLuint VAO, VBO;
	std::vector<glm::vec2> vertices;

};