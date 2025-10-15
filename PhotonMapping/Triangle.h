#pragma once
#include <glm/glm.hpp>
class Triangle {
 public:
  glm::vec3 v0, v1, v2;
  glm::vec3 normal;
  glm::vec3 color;

  Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color);
  Triangle();

  void ComputeNormal();

  bool Intersect(const glm::vec3& ray_origin, const glm::vec3& ray_direction) const {
    // Moller Trumbore intersection algorithm
    auto edge1 = v1 - v0, edge2 = v2 - v0;
    auto b = ray_origin - v0;

    glm::mat3 A(-ray_direction, edge1, edge2);
    auto tuv = glm::inverse(A) * b;

    auto t = tuv.x, u = tuv.y, v = tuv.z;

    if (t < 0 || u < 0 || v < 0 || u + v > 1) {
      return false;
    }
    // TODO more infomation
    return true;
  }
};
