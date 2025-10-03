#pragma once
#include "TypeAlias.h"
class Face {
 public:
  HalfEdgeIndex half_edge;
  int face_degree;

  VertexIndex new_vertex;

  Face() : half_edge(-1), face_degree(0), new_vertex(-1) {}
  Face(HalfEdgeIndex h_e, int f_d)
      : half_edge(h_e), face_degree(f_d), new_vertex(-1) {}
};
