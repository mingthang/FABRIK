#pragma once

#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Util {

    float SignedAngle(const glm::vec2& v1, const glm::vec2& v2)
    {
        // Calculate the angle between the two vectors
        float angle = glm::atan(v2.y, v2.x) - glm::atan(v1.y, v1.x);

        // Normalize the angle to be within the range of -pi to pi
        if (angle > glm::pi<float>()) {
            angle -= 2.0f * glm::pi<float>();
        }
        else if (angle < -glm::pi<float>()) {
            angle += 2.0f * glm::pi<float>();
        }

        return angle;
    }

}
