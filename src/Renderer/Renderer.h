#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Renderer/Types/Model2D.h"

namespace Renderer
{
    void Init(unsigned int _width, unsigned int _height);
    void CleanUp();
    void LoadShaders();

    void SalamanderPass();

    void DrawPoint(glm::vec2 position, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), bool obeyDepth = false);
    void DrawLine(glm::vec2 begin, glm::vec2 end, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), bool obeyDepth = false);
    void DrawCircle(glm::vec2 position, float radius, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), bool obeyDepth = false);
}
