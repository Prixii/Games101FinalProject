#include "HalfEdge.h"

void HalfEdge::Print() const {
  PrintInfo("tail: %d, ", tail);
  printf("twin: %d, ", twin);
  printf("next: %d, ", next);
  printf("face: %d, ", face);
  printf("new_vertex: %d\n", new_vertex);
}
void HalfEdge::Print(int index) const {
  PrintInfo("HalfEdge %d: tail: %d, ", index, tail);
  printf("twin: %d, ", twin);
  printf("next: %d, ", next);
  printf("face: %d, ", face);
  printf("new_vertex: %d\n", new_vertex);
}
bool HalfEdge::HasTwin() const { return twin != -1; }
