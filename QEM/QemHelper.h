#pragma once
#include <glm/glm.hpp>
#include <queue>

#include "../general/Tools.h"
#include "Structs.h"

class QemHelper {
private:
  std::priority_queue<VertexPair> cost_heap_;

  glm::vec4 CalcPlaneOfTriangle(const glm::vec3 &a, const glm::vec3 &b,
                                const glm::vec3 &c) {
    NOT_IMPLEMENTED
  }
};
