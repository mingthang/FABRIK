#pragma once

#include <string>
#include <glm/gtc/type_ptr.hpp>

struct Mesh2D {
public:
    int32_t baseVertex = 0;
    uint32_t baseIndex = 0;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;

    glm::vec2 aabbMin = glm::vec2(0);
    glm::vec2 aabbMax = glm::vec2(0);
    glm::vec2 extents = glm::vec2(0);

    void SetName(const std::string& name);
    const std::string& GetName() const;
    int32_t GetIndexCount() const;

private:
    std::string m_name = "undefined";
};
