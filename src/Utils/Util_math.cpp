#include "Util.h"

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

	// Constrain the distance of a point from an anchor point
	// If the distance is less than the minimum distance, move it to the minimum distance
	glm::vec2 ConstrainDistance(glm::vec2 point, glm::vec2 anchor, float distance, float max_distance) {
		glm::vec2 direction = point - anchor;
		float current_distance = glm::length(direction);

		if (current_distance > max_distance) {
			direction = glm::normalize(direction) * max_distance;
			point = anchor + direction;
		}
		else if (current_distance < distance) {
			direction = glm::normalize(direction) * distance;
			point = anchor + direction;
		}

		return point;
	}

	std::pair<glm::vec2, glm::vec2> ComputeAABB(const std::vector<glm::vec2>& vertices) {
		glm::vec2 min(std::numeric_limits<float>::max());
		glm::vec2 max(std::numeric_limits<float>::lowest());

		for (const auto& v : vertices) {
			min = glm::min(min, v);
			max = glm::max(max, v);
		}

		return { min, max };
	}

	glm::vec2 QuadraticBezier(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, float t) {
		float u = 1.0f - t;
		float tt = t * t;
		float uu = u * u;
		return uu * p0 + 2 * u * t * p1 + tt * p2;
	}

}