#pragma once
#include "../general/Tools.h"
#include "TypeAlias.h"

class HalfEdge {
public:
  VertexIndex tail_;
  HalfEdgeIndex twin_;
  HalfEdgeIndex next_;
  FaceIndex face_;
  bool is_deleted_;
  VertexIndex new_vertex_;

  HalfEdge()
      : tail_(-1), twin_(-1), next_(-1), face_(-1), is_deleted_(false),
        new_vertex_(-1) {}

  void Print() const;
  void Print(int index) const;

  bool HasTwin() const;
};
