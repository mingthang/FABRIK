#pragma once
#include "IK/Joint.h"
#include "Salamander/Leg.h"
#include <unordered_map>

struct SalamanderConfig {
    int numJoints = 17;
    float startX = 0.1f;
    float startY = 0.1f;
    float yStep = 0.06f;
    float startWidth = 1.0f;
    float widthStep = 0.05f;
    std::vector<int> legIndices;
    std::unordered_map<int, float> overrideWidths;
};

class Salamander
{
public:
    Salamander(const SalamanderConfig& config);
    //~Salamander();

    void AddBodyJoint(float x, float y, float width, bool hasLegs = false);
    void AddLeg(int bodyIndex);
    void Move(glm::vec2 target, float deltaTime);
    //void Update();
    void Draw();


private:
    std::vector<Joint> m_bodyJoints;
    std::vector<float> m_distances;
    std::vector<Leg> m_legs;
	std::vector<int> m_legIndices;
    float m_speed = 200.0f;
    float m_pullStrength = 5.0f;

    // for debug
   	bool IK = true;
    bool torque = true;

};
