#pragma once
#include <glm/glm.hpp>

using namespace glm;

class Triangle {
public:
  vec3 v0_, v1_, v2_;
  vec3 normal_;
  vec3 color_;

  Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 color);
  Triangle();

  void UpdateNormal();

  bool Intersect(const vec3 &ray_origin, const vec3 &ray_direction,
                 vec3 &x0, float &t0) const;
};
