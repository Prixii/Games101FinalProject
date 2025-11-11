#include "HalfEdge.h"

void HalfEdge::Print() const {
  PrintInfo("tail_: %d, ", tail_);
  printf("twin_: %d, ", twin_);
  printf("next_: %d, ", next_);
  printf("face_: %d, ", face_);
  printf("new_vertex: %d\n", new_vertex_);
}
void HalfEdge::Print(int index) const {
  PrintInfo("HalfEdge %d: tail_: %d, ", index, tail_);
  printf("twin_: %d, ", twin_);
  printf("next_: %d, ", next_);
  printf("face_: %d, ", face_);
  printf("new_vertex: %d\n", new_vertex_);
}
bool HalfEdge::HasTwin() const { return twin_ != -1; }
