#include "Leg.h"

#include <iostream>
#include <Renderer/Renderer.h>

Leg::Leg(Joint* bodyJoint, ELegSide side)
	: bodyJoint(bodyJoint), side(side), foot(glm::vec2(0.0f), footWidth, nullptr)
{
    foot.position = ComputeStepPosition();
    glm::vec2 elbowPos = bodyJoint->position + (0.5f * (foot.position - bodyJoint->position));
    elbow = Joint(elbowPos, 0, bodyJoint);
    foot.prevJoint = &elbow;
}

glm::vec2 Leg::ComputeStepPosition()
{
    if (!bodyJoint || !bodyJoint->prevJoint)
        return glm::vec2(0.0f);

    glm::vec2 moveDirection = glm::normalize(bodyJoint->prevJoint->position - bodyJoint->position);

    float angle = glm::radians(side == ELegSide::LEFT ? 40.0f : -40.0f);
    glm::mat2 rotationMatrix = glm::mat2(
        glm::cos(angle), -glm::sin(angle),
        glm::sin(angle), glm::cos(angle)
    );
    glm::vec2 offset = rotationMatrix * moveDirection;

    glm::vec2 finalPos = bodyJoint->position + glm::normalize(moveDirection + offset) * legLength;

    return finalPos;
}

void Leg::SolveLegIK(glm::vec2 target)
{
    if (!bodyJoint) return;

    std::vector<Joint> joints = { *bodyJoint, elbow, foot };
    std::vector<float> distances = {
        glm::distance(joints[0].position, joints[1].position),
        glm::distance(joints[1].position, joints[2].position)
    };

    FABRIK::SolveSEE(joints, distances, target);

    elbow.position = joints[1].position;
    foot.position = joints[2].position;
}
