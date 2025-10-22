#pragma once
#include <glm/glm.hpp>
class Triangle {
public:
  glm::vec3 v0, v1, v2;
  glm::vec3 normal;
  glm::vec3 color;

  Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color);
  Triangle();

  void UpdateNormal();

  bool Intersect(const glm::vec3 &ray_origin, const glm::vec3 &ray_direction,
                 glm::vec3 &x0, float &t0) const;
};
