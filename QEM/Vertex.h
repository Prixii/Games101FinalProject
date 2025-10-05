#pragma once
#include <glm/glm.hpp>

#include "Quadric.h"
#include "TypeAlias.h"

struct Vertex {
  VertexID id;
  glm::vec3 pos;
  Quadric q;
};
