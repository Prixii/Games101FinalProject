#pragma once
#include <glm/glm.hpp>

#include "Tools.h"
#include "TypeAlias.h"
class Vertex {
 public:
  glm::vec3 position;
  HalfEdgeIndex start_half_edge;
  int vertex_degree;

  glm::vec3 new_position;

  Vertex()
      : position(0, 0, 0),
        start_half_edge(-1),
        vertex_degree(0),
        new_position(0, 0, 0) {};
  Vertex(glm::vec3 position)
      : position(position),
        start_half_edge(-1),
        vertex_degree(0),
        new_position(0, 0, 0) {};

  bool HasStartHalfEdge() const;

  void Print() const;
  void Print(int index) const;

  std::string ToObjString() const;
};
