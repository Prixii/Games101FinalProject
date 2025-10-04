#pragma once
#include <vector>
#include "TypeAlias.h"
#include "HalfEdge.h"

class Face {
 public:
  HalfEdgeIndex half_edge;
  int face_degree;

  VertexIndex new_vertex;

  Face() : half_edge(-1), face_degree(0), new_vertex(-1) {}
  Face(HalfEdgeIndex h_e, int f_d)
      : half_edge(h_e), face_degree(f_d), new_vertex(-1) {}

  std::vector<VertexIndex> GetVertexIndices(
      std::vector<HalfEdge>& half_edges) const {
    std::vector<VertexIndex> indices;
    auto start_index = half_edge, current_index = half_edge;

    do {
      const auto& current_edge = half_edges[current_index];
      indices.push_back(current_edge.tail);
      current_index = current_edge.next;

    } while (current_index != start_index);

    return indices;
  }
};
