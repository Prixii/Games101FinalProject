#pragma once
#include <glm/glm.hpp>
#include "TypeAlias.h"
class Vertex {
 public:
  glm::vec3 posiiton;
  HalfEdgeIndex half_edge;
  int vertex_degree;

  glm::vec3 new_position;

  Vertex() :posiiton(0, 0, 0), half_edge(-1), vertex_degree(0), new_position(0, 0, 0) {};
  Vertex(glm::vec3 position) :posiiton(position), half_edge(-1), vertex_degree(0), new_position(0, 0, 0) {};
};
