#include "Face.h"

std::vector<VertexIndex>
Face::GetVertexIndices(const std::vector<HalfEdge> &half_edges) const {
  std::vector<VertexIndex> indices;
  auto start_index = start_half_edge_, current_index = start_half_edge_;

  do {
    const auto &current_edge = half_edges[current_index];
    indices.push_back(current_edge.tail_);
    current_index = current_edge.next_;

  } while (current_index != start_index);

  return indices;
}

void Face::Print() const {
  PrintInfo("start_half_edge_: %d, face_degree: %d\n", start_half_edge_,
            face_degree_);
}

void Face::Print(int index) const {
  PrintInfo("Face %d: start_half_edge_: %d, face_degree: %d\n", index,
            start_half_edge_, face_degree_);
}
std::vector<Vertex>
Face::GetVertices(const std::vector<HalfEdge> &half_edges,
                  const std::vector<Vertex> &vertices) const {
  auto vs = GetVertexIndices(half_edges);
  auto v0 = vertices[vs[0]];
  auto v1 = vertices[vs[1]];
  auto v2 = vertices[vs[2]];
  return {v0, v1, v2};
}
