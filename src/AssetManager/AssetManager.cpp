#include "AssetManager.h"
#include <unordered_map>
#include <iostream>
#include <Utils/Util.h>

namespace AssetManager {

	std::vector<Model2D> g_models;
    std::vector<Mesh2D> g_meshes;
	std::unordered_map<std::string, int> g_modelIndexMap;

    std::vector<glm::vec2> g_vertices; // pos only for now
    std::vector<uint32_t> g_indices;

    void Init() {
		g_models.reserve(16);
		g_meshes.reserve(16);
		g_vertices.reserve(1024);
		g_indices.reserve(1024);

        // Build CPU data
		Build2DPrimitives();
		BuildIndexMaps();   
    }

    std::vector<Mesh2D>& GetMeshes() {
        return g_meshes;
    }

	int CreateMesh(const std::string& name, std::vector<glm::vec2>& vertices, std::vector<uint32_t>& indices, glm::vec2 aabbMin, glm::vec2 aabbMax) {
        std::vector<Mesh2D>& meshes = GetMeshes();
        std::vector<glm::vec2>& allVertices = GetVertices();
        std::vector<uint32_t>& allIndices = GetIndices();

        Mesh2D& mesh = meshes.emplace_back();
		mesh.baseVertex = allVertices.size();
		mesh.baseIndex = allIndices.size();
		mesh.vertexCount = (uint32_t)vertices.size();
        mesh.indexCount = (uint32_t)indices.size();
		mesh.SetName(name);
		mesh.aabbMin = aabbMin;
		mesh.aabbMax = aabbMax;
        mesh.extents = aabbMax - aabbMin;

        allVertices.reserve(allVertices.size() + vertices.size());
		allVertices.insert(allVertices.end(), vertices.begin(), vertices.end());
        allIndices.reserve(allIndices.size() + indices.size());
		allIndices.insert(allIndices.end(), indices.begin(), indices.end());

		return (int)(meshes.size() - 1);
	}

    Model2D* CreateModel(const std::string& name) {
        std::vector<Model2D>& models = GetModels();
        models.emplace_back();
        Model2D* model = &models[models.size() - 1];
        model->SetName(name);
        return model;
    }

    Model2D* GetModelByName(const std::string& name) {
        int index = GetModelIndexByName(name);
        return GetModelByIndex(index);
    }

    Model2D* GetModelByIndex(int index) {
        if (index >= 0 && index < GetModels().size()) {
            return &GetModels()[index];
        }
        std::cout << "AssetManager::GetModelByIndex() failed because index '" << index << "' was out of range in g_models!\n";
        return nullptr;
    }

    int GetModelIndexByName(const std::string& name) {
        std::unordered_map<std::string, int>& modelIndexMap = GetModelIndexMap();
        auto it = modelIndexMap.find(name);
        if (it != modelIndexMap.end()) {
            return it->second;
        }
        std::cout << "AssetManager::GetModelIndexByName() failed because name '" << name << "' was not found in g_models!\n";
        return -1;
    }

    void Build2DPrimitives() {
        Model2D* model = CreateModel("2D_Primitives");

        /* Line-loop Circle */
		std::vector<glm::vec2> circleVertices;
        std::vector<uint32_t> circleIndices;
        const int circleSegments = 32;
        const float radius = 1.0f;

        for (int i = 0; i < circleSegments; ++i) {
            float angle = i * 2.0f * 3.1415926f / circleSegments;
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            circleVertices.emplace_back(x, y);
            circleIndices.push_back(i); // line-loop indices
        }

        auto [aabbMin, aabbMax] = Util::ComputeAABB(circleVertices);

        int meshIndex = CreateMesh("CircleOutline", circleVertices, circleIndices, aabbMin, aabbMax);
        model->AddMeshIndex(meshIndex);
    }

    void BuildIndexMaps() {
        for (int i = 0; i < g_models.size(); ++i) {
            g_modelIndexMap[g_models[i].GetName()] = i;
        }
    }

    std::vector<Model2D>& GetModels() {
        return g_models;
    }

    std::unordered_map<std::string, int>& GetModelIndexMap() {
        return g_modelIndexMap;
    }

    int GetMeshIndexByModelNameMeshName(const std::string& modelName, const std::string& meshName) {
        Model2D* model = GetModelByName(modelName);
        if (!model) return -1;

        for (uint32_t meshIndex : model->GetMeshIndices()) {
            Mesh2D* mesh = GetMeshByIndex(meshIndex);
            if (mesh && mesh->GetName() == meshName) {
                return meshIndex;
            }
        }
        return -1;
    }

    Mesh2D* GetMeshByIndex(int index) {
        std::vector<Mesh2D>& meshes = GetMeshes();
        if (index >= 0 && index < meshes.size()) {
            return &meshes[index];
        }
        else {
            std::cout << "AssetManager::GetMeshByIndex() failed because index '" << index << "' is out of range. Size is " << meshes.size() << "!\n";
            return nullptr;
        }
    }

    std::vector<glm::vec2>& GetVertices() {
        return g_vertices;
    }

    std::vector<uint32_t>& GetIndices() {
        return g_indices;
    }
}   
