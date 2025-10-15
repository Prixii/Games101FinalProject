#pragma once
#include "../general/Tools.h"
#include "TypeAlias.h"

class HalfEdge {
 public:
  VertexIndex tail;
  HalfEdgeIndex twin;
  HalfEdgeIndex next;
  FaceIndex face;

  VertexIndex new_vertex;

  HalfEdge() : tail(-1), twin(-1), next(-1), face(-1), new_vertex(-1) {}

  void Print() const;
  void Print(int index) const;

  bool HasTwin() const;
};
