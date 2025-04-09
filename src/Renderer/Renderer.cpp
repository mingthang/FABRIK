#include "Renderer.h"
#include <glad/glad.h>
#include <AssetManager/AssetManager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <API/OpenGL/Types/GL_shader.h>
#include <iostream>

namespace Renderer
{
	struct CircleDrawCommand {
		glm::mat4 modelMatrix;
		glm::vec3 color;
		bool fill;
	};

    GLuint g_VAO, g_VBO, g_EBO;

	std::vector<glm::vec2> g_points;
	std::vector<glm::vec2> g_lines;

	std::vector<CircleDrawCommand> g_circles;
	std::vector<glm::mat4> g_circleModels;

	unsigned int width, height;
	std::unordered_map<std::string, OpenGLShader> g_shaders;

    void Init(unsigned int _width, unsigned int _height)
	{
		width = _width;
		height = _height;

		const auto& allVertices = AssetManager::GetVertices();
		const auto& allIndices = AssetManager::GetIndices();

        glGenVertexArrays(1, &g_VAO);
        glGenBuffers(1, &g_VBO);
		glGenBuffers(1, &g_EBO);

		glBindVertexArray(g_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
		glBufferData(GL_ARRAY_BUFFER,
			allVertices.size() * sizeof(glm::vec2),
			allVertices.data(),
			GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			allIndices.size() * sizeof(uint32_t),
			allIndices.data(),
			GL_STATIC_DRAW);

		LoadShaders();

		glm::mat4 projection = glm::ortho(0.0f, 800.0f,
			600.0f, 0.0f, -1.0f, 1.0f);
		g_shaders["debug"].Use();
		g_shaders["debug"].SetMat4("u_projection", projection);
    }

	void LoadShaders() {
		g_shaders["debug"] = OpenGLShader({ "debug.vert", "debug.frag" });
	}

    void CleanUp()
    {
        glDeleteVertexArrays(1, &g_VAO);
        glDeleteBuffers(1, &g_VBO);
    }

    void SalamanderPass(){
		g_shaders["debug"].Use();

		glBindVertexArray(g_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, g_VBO);

		// Draw lines
		if (!g_lines.empty()) {
			g_shaders["debug"].SetMat4("u_model", glm::mat4(1.0f));
			g_shaders["debug"].SetVec3("u_color", glm::vec3(0.0f));
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * g_lines.size(), g_lines.data(), GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

			glLineWidth(2.75f);
			glDrawArrays(GL_LINES, 0, (GLsizei)g_lines.size());
		}

		// Draw points
		if (!g_points.empty()) {
			g_shaders["debug"].SetMat4("u_model", glm::mat4(1.0f));
			g_shaders["debug"].SetVec3("u_color", glm::vec3(0.0f));
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * g_points.size(), g_points.data(), GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

			glPointSize(8.0f);
			glDrawArrays(GL_POINTS, 0, (GLsizei)g_points.size());
		}

		if (!g_circles.empty()) {
			int circleFillMeshIndex = AssetManager::GetMeshIndexByModelNameMeshName("2D_Primitives", "CircleFill");
			Mesh2D* fillMesh = AssetManager::GetMeshByIndex(circleFillMeshIndex);

			int circleOutlineMeshIndex = AssetManager::GetMeshIndexByModelNameMeshName("2D_Primitives", "CircleOutline");
			Mesh2D* outlineMesh = AssetManager::GetMeshByIndex(circleOutlineMeshIndex);

			if (!fillMesh) {
				std::cout << "Failed to get circle fill mesh!" << std::endl;
				return;
			}
			if (!outlineMesh) {
				std::cout << "Failed to get circle outline mesh!" << std::endl;
				return;
			}

			std::vector<glm::vec2> circleVertices = AssetManager::GetVertices();
			std::vector<uint32_t> circleIndices = AssetManager::GetIndices();

			glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * circleVertices.size(), circleVertices.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * circleIndices.size(), circleIndices.data(), GL_STATIC_DRAW);

			for (const auto& cmd : g_circles) {
				g_shaders["debug"].SetMat4("u_model", cmd.modelMatrix);
				g_shaders["debug"].SetVec3("u_color", cmd.color);
				if (cmd.fill) {
					glDrawElementsBaseVertex(
						GL_TRIANGLES,
						fillMesh->indexCount,
						GL_UNSIGNED_INT,
						(void*)(sizeof(uint32_t) * fillMesh->baseIndex),
						fillMesh->baseVertex
					);
				}

				else {
					g_shaders["debug"].SetVec3("u_color", glm::vec3(0.0f)); // black
					glDrawElementsBaseVertex(
						GL_LINE_LOOP,
						outlineMesh->indexCount,
						GL_UNSIGNED_INT,
						(void*)(sizeof(uint32_t) * outlineMesh->baseIndex),
						outlineMesh->baseVertex
					);
				}
			}
		}

		glBindVertexArray(0);
		glUseProgram(0);

		g_points.clear();
		g_lines.clear();
		g_circles.clear();
    }

    void DrawPoint(glm::vec2 position, bool obeyDepth) {
        g_points.push_back(position);
    }

    void DrawLine(glm::vec2 begin, glm::vec2 end, bool obeyDepth) {
        g_lines.push_back(begin);
        g_lines.push_back(end);
    }

	void DrawCircle(glm::vec2 position, float radius, glm::vec3 color, bool fill, bool obeyDepth) {
		g_circles.push_back({
			glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(radius)),
			color,
			fill
			});
	}
}
