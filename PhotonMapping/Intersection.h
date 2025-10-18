#pragma once
#include <glm/glm.hpp>

struct Intersection {
  glm::vec3 position_;
  float distance_;
  int sphere_index_;
  int triangle_index_;

  bool IsSphere() const { return sphere_index_ >= 0; }

  bool IsTriangle() const { return triangle_index_ >= 0; }
};
