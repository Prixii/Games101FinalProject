#pragma once
#include <glm/glm.hpp>

#include "TypeAlias.h"

struct EdgeCollapse {
  double cost;
  VertexID v1, v2;
  glm::vec3 optimal;

  bool operator>(const EdgeCollapse& other) const { return cost > other.cost; }
  bool operator<(const EdgeCollapse& other) const { return cost < other.cost; }
};
