#pragma once
#include "IK/Joint.h"
#include "Salamander/Leg.h"
#include "Utils/Shader.h"

class Salamander
{
public:
	std::vector<Joint> bodyJoints;
	std::vector<float> distances;
	std::vector<Leg> legs;
	float speed = 5.0f;
	float pullStrength = 0.002f;
	Shader* circleShader;

	Salamander(Shader* circleShader);
	~Salamander();

	void AddBodyJoint(float x, float y, float width, bool hasLegs = false);
	void AddLeg(int bodyIndex);
	void UpdateBuffer();
	void GenerateCircleVertices(int segments = 12);
	void Draw();
	void Move(glm::vec2 target, float deltaTime);

private:
	GLuint VAO, VBO;
	std::vector<glm::vec2> vertices;
	GLuint circleVAO, circleVBO;
	std::vector<glm::vec2> circleVertices;
	GLuint legVAO, legVBO;
	std::vector<glm::vec2> legVertices;
};
