#include "Model.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "Tools.h"

void Model::InitFromObj(const char* filename) {
  std::fstream file(filename);
  std::string line;
  std::string s[MAX_FACE_DEGREE];
  vertex_buffer.clear();

  while (getline(file, line)) {
    std::istringstream sin(line);
    if (StringStartsWith(line, "v ")) {
      ReadVertex(sin, s);
    } else if (StringStartsWith(line, "f ")) {
      ReadFace(sin);
    }
  }

  file.close();

  PrintVertexAndFaces();

  ParseHalfEdge();

  CheckParseHalfEdgeResult();
}

void Model::ParseHalfEdge() {
  // 1. save vertices
  for (int i = 0; i < vertex_buffer.size(); i++) {
    Vertex vertex(vertex_buffer[i]);
    vertex_lib.push_back(vertex);
  }

  // 2. save edges, faces
  std::map<EdgeKey, HalfEdgeIndex> edge_map;
  HalfEdgeIndex half_edge_index = 0;
  for (size_t face_ind = 0; face_ind < face_buffer.size(); face_ind++) {
    auto& vertex_loop = face_buffer[face_ind];
    HalfEdgeIndex prev_ind = -1;
    HalfEdgeIndex start_ind = half_edge_index;
    for (size_t vert_ind = 0; vert_ind < vertex_loop.size(); vert_ind++) {
      HalfEdge half_edge;
      half_edge.tail = vertex_loop[vert_ind];

      half_edge.face = face_ind;

      auto head_vertex_ind = vertex_loop[(vert_ind + 1) % vertex_loop.size()];
      auto reverse_edge_key = EdgeKey(
          head_vertex_ind, half_edge.tail);
      if (edge_map.count(reverse_edge_key)) {
        auto twin_index = edge_map[reverse_edge_key];
        auto& twin_edge = half_edge_lib[twin_index];

        twin_edge.twin = half_edge_index;
        half_edge.twin = twin_index;

        edge_map.erase(reverse_edge_key);
      } else {
        edge_map[EdgeKey(half_edge.tail, head_vertex_ind)] = half_edge_index;
      }

      half_edge_lib.push_back(half_edge);

      if (prev_ind != -1) {
        half_edge_lib[prev_ind].next = half_edge_index;
      }
      prev_ind = half_edge_index;
      half_edge_index++;
    }
    // close the loop
    if (prev_ind != -1) {
      half_edge_lib[prev_ind].next = start_ind;
    }

    Face face(half_edge_index - vertex_loop.size(), vertex_loop.size());
    face_lib.push_back(face);
  }

  for (auto edge : half_edge_lib) {
    edge.Print();
  }

  // 3. calc vertex degree
  for (auto& edge : half_edge_lib) {
    vertex_lib[edge.tail].vertex_degree++;
  }

  for (size_t i = 0; i < vertex_lib.size(); i++) {
    PrintInfo("degree of vertex%d: %d\n", i, vertex_lib[i].vertex_degree);
  }
}

void Model::CheckParseHalfEdgeResult() {
  // check
  bool has_error = false;

  // 检查 Twin 双向一致性
  for (size_t i = 0; i < half_edge_lib.size(); ++i) {
    HalfEdge& he_A = half_edge_lib[i];
    HalfEdgeIndex twin_idx = he_A.twin;

    if (twin_idx == 0) {
      // 这是边界边，跳过 Twin 检查
      continue;
    }

    if (twin_idx >= half_edge_lib.size()) {
      PrintErr("Error: HE %zu has twin %d out of bounds.\n", i, twin_idx);
      has_error = true;
      continue;
    }

    HalfEdge& he_B = half_edge_lib[twin_idx];

    // 1. 检查对称性：A 的 twin 必须是 B，B 的 twin 必须是 A
    if (he_B.twin != i) {
      PrintErr(
          "Error: HE %zu (tail %d) has twin %d, but twin's twin is %d (not "
          "%zu).\n",
          i, he_A.tail, twin_idx, he_B.twin, i);
      has_error = true;
    }

    // 2. 检查方向性：A 的起点必须是 B 的终点，反之亦然
    if (he_A.tail != he_B.next) {  // 假设你有一个获取头部顶点的辅助函数
      // 注意：因为 half_edge 没有 head 属性，需要通过 next 关系或 twin
      // 关系间接判断

      // 简单判断：A 的 tail 必须等于 B 的 head
      // B 的 head 是 B 的 twin 的 tail
      if (he_A.tail != half_edge_lib[he_B.next].tail) {
        PrintErr("Error: HE %zu and twin %d have inconsistent vertex tails.\n",
                 i, twin_idx);
        has_error = true;
      }
    }
  }

  if (!has_error) {
    PrintInfo("Half-Edge integrity check passed!\n");
  }
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

      VertexIndex v_idx = obj_v_index - 1;
      face_v_indices.push_back(v_idx);
    } catch (const std::invalid_argument& e) {
      PrintErr("invalid argument, %s\n", segment.c_str());
      break;
    }
  }
  face_buffer.push_back(face_v_indices);
}

void Model::PrintVertexAndFaces() {
  for (auto i = 0; i < vertex_buffer.size(); i++) {
    PrintInfo("Vertex%d: %f, %f, %f\n", i, vertex_buffer[i].x,
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

void Model::DoCatmullClarkSubdivision() {
  auto old_vertex_size = vertex_lib.size();
  // 1. calc center for each face
  for (auto& face : face_lib) {
    Vertex center_vertex;
    glm::vec3 center_pos(0.f, 0.f, 0.f);

    auto vertex_indices = face.GetVertexIndices(half_edge_lib);
    for (auto index : vertex_indices) {
      center_pos += vertex_lib[index].posiiton;
    }
    center_pos /= vertex_lib.size();
    center_vertex.posiiton = center_pos;
  }

  // 2. calc center for each edge
  std::map<EdgeKey, VertexIndex> edge_center_map;
  for (auto& edge : half_edge_lib) {
    auto& twin_edge = half_edge_lib[edge.twin];
    auto tail_pos = vertex_lib[edge.tail].posiiton,
         head_pos = vertex_lib[twin_edge.tail].posiiton;
  }
}

void Model::ExportToObj(const char* filename) {}
