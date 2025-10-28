#pragma once

#include <glm/glm.hpp>
#include <string>

struct Vertex {
  glm::vec3 position_;
  glm::vec3 normal_;
  glm::vec2 texcoord_;

  Vertex() = default;
};

struct Material {
  std::string name_;
  glm::vec4 diffuse_color_;
  glm::vec4 ambient_color_;
  glm::vec4 specular_color_;

  Material() = default;
};

struct BasicMeshEntry {
  unsigned int indices_count_ = -1;
  unsigned int base_vertex_ = -1;
  unsigned int base_index = -1;
  unsigned int material_idx_ = -1;
};

struct Intersection {
  glm::vec3 position_;
  glm::vec3 normal_;
  float distance_;
  int mesh_index_;
  float t_;
};
