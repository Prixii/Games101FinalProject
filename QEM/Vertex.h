#pragma once
#include <glm/glm.hpp>

#include "../general/Tools.h"
#include "TypeAlias.h"
class Vertex {
 public:
  glm::vec3 position_;
  HalfEdgeIndex start_half_edge_;
  int vertex_degree_;

  glm::vec3 new_position_;

  Vertex()
      : position_(0, 0, 0),
        start_half_edge_(-1),
        vertex_degree_(0),
        new_position_(0, 0, 0) {};
  Vertex(glm::vec3 position_)
      : position_(position_),
        start_half_edge_(-1),
        vertex_degree_(0),
        new_position_(0, 0, 0) {};

  bool HasStartHalfEdge() const;

  void Print() const;
  void Print(int index) const;

  std::string ToObjString() const;
};
