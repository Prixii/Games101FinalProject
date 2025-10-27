#include "BasicMesh.h"
bool BasicMesh::InitFromScene(const aiScene &scene) {
  meshes_.resize(scene.mNumMeshes);
  materials_.resize(scene.mNumMaterials);

  auto [num_vertices, num_indices] = CountVerticesAndIndices(scene);
  vertices_.reserve(num_vertices);
  indices_.reserve(num_indices);

  InitAllMeshes(scene);
  InitMaterials(scene);
  return true;
}

std::pair<uint32_t, uint32_t>
BasicMesh::CountVerticesAndIndices(const aiScene &scene) {
  uint32_t vertex_count = 0;
  uint32_t index_count = 0;
  for (int i = 0; i < scene.mNumMeshes; i++) {
    auto ai_mesh = scene.mMeshes[i];

    meshes_[i].material_idx_ = ai_mesh->mMaterialIndex;
    meshes_[i].indices_count_ = ai_mesh->mNumFaces * 3;
    meshes_[i].base_index = index_count;
    meshes_[i].base_vertex_ = vertex_count;

    vertex_count += ai_mesh->mNumVertices;
    index_count += ai_mesh->mNumFaces * 3;
  }
  return {vertex_count, index_count};
}

void BasicMesh::InitAllMeshes(const aiScene &scene) {
  for (auto i = 0; i < scene.mNumMeshes; i++) {
    auto mesh = scene.mMeshes[i];
    InitSingleMesh(i, *mesh);
  }
}

void BasicMesh::InitSingleMesh(uint32_t mesh_index, const aiMesh &ai_mesh) {
  Vertex vert{};
  for (auto i = 0; i < ai_mesh.mNumVertices; i++) {
    const auto &pos = ai_mesh.mVertices[i];
    vert.position_ = {pos.x, pos.y, pos.z};
    if (ai_mesh.mNormals) {
      vert.normal_ = {ai_mesh.mNormals[i].x, ai_mesh.mNormals[i].y,
                      ai_mesh.mNormals[i].z};
    } else {
      vert.normal_ = {0.0f, 1.0f, 0.0f};
    }
    vertices_.push_back(vert);
  }
  for (auto i = 0; i < ai_mesh.mNumFaces; i++) {
    const auto &face = ai_mesh.mFaces[i];
    if (face.mNumIndices != 3) {
      PrintWarn("Face with %d indices is not supported!\n", face.mNumIndices);
      continue;
    }

    indices_.push_back(face.mIndices[0]);
    indices_.push_back(face.mIndices[1]);
    indices_.push_back(face.mIndices[2]);
  }
}
void BasicMesh::InitMaterials(const aiScene &scene) {
  for (int i = 0; i < scene.mNumMaterials; i++) {
    auto material = scene.mMaterials[i];
    int shading_model = 0;
    if (material->Get(AI_MATKEY_SHADING_MODEL, shading_model) == AI_SUCCESS) {
      PrintInfo("ShadingModel: %d\n", shading_model);
    }
    aiColor3D ambient_color(0.f, 0.f, 0.f);
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambient_color) == AI_SUCCESS) {
      PrintInfo("AmbientColor: %f %f %f\n", ambient_color.r, ambient_color.g,
                ambient_color.b);
    }
    aiColor3D diffuse_color(0.f, 0.f, 0.f);
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color) == AI_SUCCESS) {
      PrintInfo("DiffuseColor: %f %f %f\n", diffuse_color.r, diffuse_color.g,
                diffuse_color.b);
    }
    aiColor3D specular_color(0.f, 0.f, 0.f);
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specular_color) == AI_SUCCESS) {
      PrintInfo("SpecularColor: %f %f %f\n", specular_color.r, specular_color.g,
                specular_color.b);
    }

    materials_[i].ambient_color_ = {ambient_color.r, ambient_color.g,
                                    ambient_color.b, 1.f};
    materials_[i].diffuse_color_ = {diffuse_color.r, diffuse_color.g,
                                    diffuse_color.b, 1.f};
    materials_[i].specular_color_ = {specular_color.r, specular_color.g,
                                     specular_color.b, 1.f};
    materials_[i].name_ = material->GetName().data;
  }
}
