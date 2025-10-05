#include "Vertex.h"

bool Vertex::HasStartHalfEdge() const { return start_half_edge != -1; }

void Vertex::Print() const {
  PrintInfo("Vertex: %f, %f, %f, start_half_edge: %d, vertex_degree: %d\n",
            position.x, position.y, position.z, start_half_edge, vertex_degree);
}
void Vertex::Print(int index) const {
  PrintInfo("Vertex %d: %f, %f, %f, start_half_edge: %d, vertex_degree: %d\n",
            index, position.x, position.y, position.z, start_half_edge,
            vertex_degree);
}

std::string Vertex::ToObjString() const {
  return "v " + std::to_string(position.x) +
         " " + std::to_string(position.y) + " " + std::to_string(position.z) + "\n";
}
