#include "HalfEdge.h"

void HalfEdge::Print() const {
  PrintInfo("tail_: %lld, ", tail_);
  printf("twin_: %lld, ", twin_);
  printf("next_: %lld, ", next_);
  printf("face_: %lld, ", face_);
  printf("new_vertex: %lld\n", new_vertex_);
}
void HalfEdge::Print(int index) const {
  PrintInfo("HalfEdge %lld: tail_: %lld, ", index, tail_);
  printf("twin_: %lld, ", twin_);
  printf("next_: %lld, ", next_);
  printf("face_: %lld, ", face_);
  printf("new_vertex: %lld\n", new_vertex_);
}
bool HalfEdge::HasTwin() const { return twin_ != -1; }
