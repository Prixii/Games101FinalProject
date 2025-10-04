#pragma once
#include <glm/glm.hpp>

#include "TypeAlias.h"
class Vertex {
 public:
  glm::vec3 posiiton;
  HalfEdgeIndex start_half_edge;
  int vertex_degree;

  glm::vec3 new_position;

  Vertex()
      : posiiton(0, 0, 0),
        start_half_edge(-1),
        vertex_degree(0),
        new_position(0, 0, 0) {};
  Vertex(glm::vec3 position)
      : posiiton(position),
        start_half_edge(-1),
        vertex_degree(0),
        new_position(0, 0, 0) {};

  inline bool HasStartHalfEdge() const { return start_half_edge != -1; }
};
