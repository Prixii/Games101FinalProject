#pragma once
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <utility>

using namespace glm;

class Sphere {
public:
  vec3 center_;
  float radius_;

  Sphere(const vec3 &c, float r);
  Sphere();

  bool Intersect(const vec3 &start, const vec3 &dir, vec3 &x0,
                 vec3 &x1, float &t0, float &t1) const;
};
