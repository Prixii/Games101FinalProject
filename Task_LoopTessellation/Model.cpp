#include "Model.h"

#include <fstream>
#include <iostream>
#include <string>

#include "Tools.h"

void Model::InitFromObj(const char* filename) {
  std::fstream file(filename);
  std::string line;
  std::string s[MAX_FACE_DEGREE];
  vertex_buffer.clear();
  vertex_buffer.push_back(
      glm::vec3(-1, -1, -1));  // dummy vertex 使其和 obj 的顶点编号一致

  while (getline(file, line)) {
    std::istringstream sin(line);
    if (StringStartsWith(line, "v ")) {
      ReadVertex(sin, s);
    } else if (StringStartsWith(line, "f ")) {
      ReadFace(sin);
    }
  }

  PrintVertexAndFaces();
}

void Model::ReadVertex(std::istringstream& sin, std::string s[3]) {
  sin.ignore(2);
  glm::vec3 position;
  for (auto i = 0; i < 3; i++) {
    sin >> s[i];
  }
  position.x = std::stof(s[0]);
  position.y = std::stof(s[1]);
  position.z = std::stof(s[2]);
  vertex_buffer.push_back(position);
}

void Model::ReadFace(std::istringstream& sin) {
  sin.ignore(2);
  std::string vertex_desc;
  std::vector<VertexIndex> face_v_indices;
  while (sin >> vertex_desc) {
    std::stringstream desc_ss(vertex_desc);
    std::string segment;
    if (!std::getline(desc_ss, segment, '/')) {
      continue;
    }
    try {
      int obj_v_index = std::stoi(segment);

      VertexIndex v_idx = obj_v_index;
      face_v_indices.push_back(v_idx);
    } catch (const std::invalid_argument& e) {
      PrintErr("invalid argument, %s\n", segment.c_str());
      break;
    }
  }
  face_buffer.push_back(face_v_indices);
}

void Model::PrintVertexAndFaces() {
  // check, print result
  for (auto i = 0; i < vertex_buffer.size(); i++) {
    PrintInfo("Vertex%d: %f, %f, %f\n", i+1, vertex_buffer[i].x,
              vertex_buffer[i].y, vertex_buffer[i].z);
  }
  for (auto i = 0; i < face_buffer.size(); i++) {
    PrintInfo("Face%d: ", i);
    for (auto j = 0; j < face_buffer[i].size(); j++) {
      printf("%d ", face_buffer[i][j]);
    }
    printf("\n");
  }
}

void Model::DoCatmullClarkSubdivision() {}

void Model::ExportToObj(const char* filename) {}
