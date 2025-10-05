#include "Face.h"

std::vector<VertexIndex> Face::GetVertexIndices(
    std::vector<HalfEdge>& half_edges) const {
  std::vector<VertexIndex> indices;
  auto start_index = start_half_edge, current_index = start_half_edge;

  do {
    const auto& current_edge = half_edges[current_index];
    indices.push_back(current_edge.tail);
    current_index = current_edge.next;

  } while (current_index != start_index);

  return indices;
}

void Face::Print() const {
  PrintInfo("start_half_edge: %d, face_degree: %d\n", start_half_edge,
            face_degree);
}

void Face::Print(int index) const {
  PrintInfo("Face %d: start_half_edge: %d, face_degree: %d\n", index, start_half_edge,
            face_degree);
}