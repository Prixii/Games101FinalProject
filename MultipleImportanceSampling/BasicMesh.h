#pragma once
#include "../general/Tools.h"
#include "Structs.h"

#include "assimp/Importer.hpp"
#include "assimp/SceneCombiner.h"
#include "assimp/material.h"
#include "assimp/scene.h"
#include "glm/geometric.hpp"
#include <cstdint>
#include <utility>
#include <vector>

class BasicMesh {
public:
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
  std::vector<Material> materials_;
  std::vector<BasicMeshEntry> meshes_;

  BasicMesh() = default;

  bool InitFromScene(const aiScene &scene);

  std::pair<uint32_t, uint32_t> CountVerticesAndIndices(const aiScene &scene);

  void InitAllMeshes(const aiScene &scene);

  void InitSingleMesh(uint32_t mesh_index, const aiMesh &ai_mesh);

  void InitMaterials(const aiScene &scene);

  void NormalizeVertices() {
    float max = 0.0f;
    for (auto &vertex : vertices_) {
      auto pos = vertex.position_;
      auto max_pos =
          std::max(std::abs(pos.x), std::max(std::abs(pos.y), std::abs(pos.z)));
      max = std::max(max, max_pos);
    }
    for (auto &vertex : vertices_) {
      vertex.position_ /= max;
    }
  }
};
