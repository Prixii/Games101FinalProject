#pragma once
#include "Tools.h"
#include "TypeAlias.h"

class HalfEdge {
 public:
  VertexIndex tail;
  HalfEdgeIndex twin;
  HalfEdgeIndex next;
  FaceIndex face;

  VertexIndex new_vertex;

  HalfEdge() : tail(-1), twin(-1), next(-1), face(-1), new_vertex(-1) {}

  void Print() {
    PrintInfo("tail: %d, ", tail);
    printf("twin: %d, ", twin);
    printf("next: %d, ", next);
    printf("face: %d, ", face);
    printf("new_vertex: %d\n", new_vertex);
  }
};
