#pragma once

#include "Photon.h"
#include <glm/glm.hpp>

using namespace glm;

struct NeighborHitPoint {
  int index_;
  float dist_;
};

struct HitPoint {
  vec3 position_;
  vec3 normal_;
  vec3 color_;
  vec3 direction_;
  vec2 screen_pos_;
  double weight_;

  HitPoint() = default;
  HitPoint(const vec3 &position, const vec3 &normal, const vec3 &color,
           const vec3 &direction, const vec2 &screen_pos, double weight)
      : position_(position), normal_(normal), color_(color),
        direction_(direction), screen_pos_(screen_pos), weight_(weight) {}
};
