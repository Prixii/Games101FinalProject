#pragma once
#include "TypeAlias.h"
class HalfEdge {
  VertexIndex tail;
  HalfEdgeIndex twin;
  HalfEdgeIndex next;
  FaceIndex face;

  VertexIndex new_vertex;
};
