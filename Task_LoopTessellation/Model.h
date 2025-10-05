#pragma once
#include <vector>
#include <sstream>

#include "Vertex.h"
#include "HalfEdge.h"
#include "Face.h"

constexpr int MAX_FACE_DEGREE = 4;

class Model {
 public:
  std::vector<Vertex> vertex_lib;
  std::vector<HalfEdge> half_edge_lib;
  std::vector<Face> face_lib;

  std::vector<glm::vec3> vertex_buffer;
  std::vector<std::vector<VertexIndex>> face_buffer;

  void InitFromObj(const char* filename);
  void ParseHalfEdge();
  void CheckParseHalfEdgeResult();
  void ReadVertex(std::istringstream& sin, std::string s[3]);
  void ReadFace(std::istringstream& sin);
  void PrintVertexAndFaces();
  void DoCatmullClarkSubdivision();
  void GenerateCenterForEdge(std::vector<Vertex>& new_vertex_lib);
  void GenerateFaceCenterVertex(std::vector<Vertex>& new_vertex_lib);
  void CalcNewPositionForOldVertex(std::vector<Vertex>& new_vertex_lib);
  void GenerateEdgeAndFace(std::vector<HalfEdge>& new_half_edge_lib,
                           std::vector<Face>& new_face_lib);
  void GenerateEdgeTwin(std::vector<HalfEdge>& new_half_edge_lib);
  void ExportToObj(const char* filename);
};
