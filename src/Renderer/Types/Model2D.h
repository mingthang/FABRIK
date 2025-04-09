#pragma once

#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "Mesh2D.h"

struct Model2D {
    Model2D() = default;

    void SetName(const std::string& name);
    void AddMeshIndex(uint32_t index);
    void SetAABB(glm::vec2 min, glm::vec2 max);

    const std::string& GetName() const { return m_name;  }
    const std::vector<uint32_t>& GetMeshIndices() const { return m_meshIndices; }
	const glm::vec2& GetAABBMin() const { return m_aabbMin; }
	const glm::vec2& GetAABBMax() const { return m_aabbMax; }
    glm::vec2 GetExtents() const { return m_aabbMax - m_aabbMin; }

private:
    std::string m_name = "undefined";
    std::vector<uint32_t> m_meshIndices;
    glm::vec2 m_aabbMin = glm::vec2(std::numeric_limits<float>::max());
    glm::vec2 m_aabbMax = glm::vec2(-std::numeric_limits<float>::max());
};
