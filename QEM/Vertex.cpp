#include "Vertex.h"

bool Vertex::HasStartHalfEdge() const { return start_half_edge_ != -1; }

void Vertex::Print() const {
  PrintInfo("Vertex: %f, %f, %f, start_half_edge_: %d, vertex_degree_: %d\n",
            position_.x, position_.y, position_.z, start_half_edge_, vertex_degree_);
}
void Vertex::Print(int index) const {
  PrintInfo("Vertex %d: %f, %f, %f, start_half_edge_: %d, vertex_degree_: %d\n",
            index, position_.x, position_.y, position_.z, start_half_edge_,
            vertex_degree_);
}

std::string Vertex::ToObjString() const {
  return "v " + std::to_string(position_.x) +
         " " + std::to_string(position_.y) + " " + std::to_string(position_.z) + "\n";
}
