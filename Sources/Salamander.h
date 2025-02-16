#pragma once
#include "Joint.h"
#include "Shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include <list>
#include <vector>

class Salamander
{
public:
	std::vector<Joint> joints;
	std::vector<float> distances;
	float speed = 1.0f;
	Shader* shader;

	Salamander(Shader* circleShader): shader(circleShader) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &circleVAO);
		glGenBuffers(1, &circleVBO);

		GenerateCircleVertices();
	}

	~Salamander() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &circleVBO);
		glDeleteVertexArrays(1, &circleVAO);
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
		glBindVertexArray(VAO);
		glLineWidth(3.0f);
		glPointSize(10.0f);
		//glDrawArrays(GL_POINTS, 0, vertices.size());
		glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
		glBindVertexArray(0);

		shader->Use();
		glBindVertexArray(circleVAO);
		for (const auto& joint : joints)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(joint.position, 0.0f)); 
			model = glm::scale(model, glm::vec3(joint.width * 0.5f, joint.width * 0.5f, 1.0f)); 

			glm::mat4 projection = glm::mat4(1.0f);

			shader->SetMat4("model", model);
			shader->SetMat4("projection", projection);
			glDrawArrays(GL_LINE_LOOP, 0, circleVertices.size());
		}
		glBindVertexArray(0);
		glUseProgram(0);
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
	GLuint circleVAO, circleVBO;
	std::vector<glm::vec2> circleVertices;
};