#pragma once
#include "../general/Tools.h"
#include "BRDF.h"
#include "Structs.h"

#include "assimp/Importer.hpp"
#include "assimp/SceneCombiner.h"
#include "assimp/material.h"
#include "assimp/scene.h"
#include "glm/geometric.hpp"
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <vector>

class BasicMesh {
public:
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
  std::vector<Material> materials_;
  std::vector<BasicMeshEntry> meshes_;
  std::vector<BRDF> brdfs_;

  BasicMesh() = default;

  bool InitFromScene(const aiScene &scene);

  std::pair<uint32_t, uint32_t> CountVerticesAndIndices(const aiScene &scene);

  void InitAllMeshes(const aiScene &scene);

  void InitSingleMesh(uint32_t mesh_index, const aiMesh &ai_mesh);

  void InitMaterials(const aiScene &scene);

  void NormalizeVertices();

  void Rotate(float deg);
};
