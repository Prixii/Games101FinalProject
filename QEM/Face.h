#pragma once
#include <vector>

#include "HalfEdge.h"
#include "TypeAlias.h"

class Face {
 public:
  HalfEdgeIndex start_half_edge_;
  int face_degree_;

  VertexIndex new_vertex_;

  Face() : start_half_edge_(-1), face_degree_(0), new_vertex_(-1) {}
  Face(HalfEdgeIndex h_e, int f_d)
      : start_half_edge_(h_e), face_degree_(f_d), new_vertex_(-1) {}

  std::vector<VertexIndex> GetVertexIndices(
      std::vector<HalfEdge>& half_edges) const;

  void Print() const;
  void Print(int index) const;

  std::string ToObjString(std::vector<HalfEdge> &half_edge_lib_) const {
    auto current_index = start_half_edge_;
		std::string str = "f";
		do {
			const auto& current_edge = half_edge_lib_[current_index];
			str += " " + std::to_string(current_edge.tail_ + 1);
			current_index = current_edge.next_;
		} while (current_index != start_half_edge_);
    return str + "\n";
  }
};
