#pragma once

#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace Util {

    // Math
    float SignedAngle(const glm::vec2& v1, const glm::vec2& v2);
    glm::vec2 ConstrainDistance(glm::vec2 point, glm::vec2 anchor, float distance, float max_distance);

    std::pair<glm::vec2, glm::vec2> ComputeAABB(const std::vector<glm::vec2>& vertices);
    glm::vec2 QuadraticBezier(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, float t);
}
