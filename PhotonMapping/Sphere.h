#pragma once
#include <glm/glm.hpp>

class Sphere {
 public:
  glm::vec3 center;
  float radius;

  Sphere(const glm::vec3& c, float r);
  Sphere();
};
