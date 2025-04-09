#pragma once
#include <Renderer/Types/Model2D.h>
#include <vector>
#include <unordered_map>

namespace AssetManager {
	void Init();

	// Meshs
	std::vector<Mesh2D>& GetMeshes();
	int CreateMesh(const std::string& name, std::vector<glm::vec2>& vertices, std::vector<uint32_t>& indices, glm::vec2 aabbMin, glm::vec2 aabbMax);
	int GetMeshIndexByModelNameMeshName(const std::string& modelName, const std::string& meshName);
	Mesh2D* GetMeshByIndex(int index);

	// Models
	Model2D* CreateModel(const std::string& name);
	Model2D* GetModelByName(const std::string& name);
	Model2D* GetModelByIndex(int index);
	int GetModelIndexByName(const std::string& name);
	void Build2DPrimitives();

	// Building
	void BuildIndexMaps();

	// GET
	std::vector<Model2D>& GetModels();
	std::unordered_map<std::string, int>& GetModelIndexMap();
	std::vector<glm::vec2>& GetVertices();
	std::vector<uint32_t>& GetIndices();
}
