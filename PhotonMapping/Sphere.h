#pragma once
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <utility>

class Sphere {
public:
  glm::vec3 center_;
  float radius_;

  Sphere(const glm::vec3 &c, float r);
  Sphere();

  bool Intersect(const glm::vec3 &start, const glm::vec3 &dir, glm::vec3 &x0,
                 glm::vec3 &x1, float &t0, float &t1) const;
};
