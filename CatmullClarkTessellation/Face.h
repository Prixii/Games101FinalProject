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

  std::string ToObjString(std::vector<HalfEdge> &half_edge_lib) const {
    auto current_index = start_half_edge;
		std::string str = "f";
		do {
			const auto& current_edge = half_edge_lib[current_index];
			str += " " + std::to_string(current_edge.tail + 1);
			current_index = current_edge.next;
		} while (current_index != start_half_edge);
    return str + "\n";
  }
};
