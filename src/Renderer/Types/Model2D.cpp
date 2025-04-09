#include "Model2D.h"

#include <algorithm> 

void Model2D::SetName(const std::string& name) {
    m_name = name;
}

void Model2D::AddMeshIndex(uint32_t index) {
    m_meshIndices.push_back(index);
}

void Model2D::SetAABB(glm::vec2 min, glm::vec2 max) {
    m_aabbMin = min;
    m_aabbMax = max;
}