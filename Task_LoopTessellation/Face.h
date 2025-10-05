#pragma once
#include <vector>

#include "HalfEdge.h"
#include "TypeAlias.h"

class Face {
 public:
  HalfEdgeIndex start_half_edge;
  int face_degree;

  VertexIndex new_vertex;

  Face() : start_half_edge(-1), face_degree(0), new_vertex(-1) {}
  Face(HalfEdgeIndex h_e, int f_d)
      : start_half_edge(h_e), face_degree(f_d), new_vertex(-1) {}

  std::vector<VertexIndex> GetVertexIndices(
      std::vector<HalfEdge>& half_edges) const;

  void Print() const;
  void Print(int index) const;
};
