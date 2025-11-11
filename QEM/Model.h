#pragma once
#include <sstream>
#include <vector>

#include "Face.h"
#include "HalfEdge.h"
#include "Vertex.h"

constexpr int MAX_FACE_DEGREE = 4;

class Model {
 public:
  std::vector<Vertex> vertex_lib_;
  std::vector<HalfEdge> half_edge_lib_;
  std::vector<Face> face_lib_;

  std::vector<glm::vec3> vertex_buffer_;
  std::vector<std::vector<VertexIndex>> face_buffer_;

  void InitFromObj(const char* filename);
  void ExportToObj(const char* filename);

 private:
  void ParseHalfEdge();
  void CheckParseHalfEdgeResult();
  void ReadVertex(std::istringstream& sin, std::string s[3]);
  void ReadFace(std::istringstream& sin);
  void PrintVertexAndFaces();
  void UpdateTwin(std::vector<HalfEdge>& new_half_edge_lib);
  void ReTopology(std::vector<HalfEdge>& new_half_edge_lib,
                  std::vector<Face>& new_face_lib);
};
