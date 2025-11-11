#include "Model.h"
#include "../general/Tools.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>


void Model::InitFromObj(const char* filename) {
  std::fstream file(filename);
  std::string line;
  std::string s[MAX_FACE_DEGREE];
  vertex_buffer_.clear();

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
  for (int i = 0; i < vertex_buffer_.size(); i++) {
    Vertex vertex(vertex_buffer_[i]);
    vertex_lib_.push_back(vertex);
  }

  // 2. save edges, faces
  std::map<EdgeKey, HalfEdgeIndex> edge_map;
  HalfEdgeIndex half_edge_index = 0;
  for (size_t face_ind = 0; face_ind < face_buffer_.size(); face_ind++) {
    auto& vertex_loop = face_buffer_[face_ind];

    HalfEdgeIndex prev_ind = -1;
    HalfEdgeIndex start_ind = half_edge_index;

    for (size_t vert_ind = 0; vert_ind < vertex_loop.size(); vert_ind++) {
      HalfEdge half_edge;

      half_edge.tail_ = vertex_loop[vert_ind];
      half_edge.face_ = face_ind;

      // check twin_
      auto head_vertex_ind = vertex_loop[(vert_ind + 1) % vertex_loop.size()];
      auto reverse_edge_key = EdgeKey(head_vertex_ind, half_edge.tail_);
      if (edge_map.count(reverse_edge_key)) {
        auto twin_index = edge_map[reverse_edge_key];
        auto& twin_edge = half_edge_lib_[twin_index];

        twin_edge.twin_ = half_edge_index;
        half_edge.twin_ = twin_index;

        edge_map.erase(reverse_edge_key);
      } else {
        edge_map[EdgeKey(half_edge.tail_, head_vertex_ind)] = half_edge_index;
      }

      // set next_ for previous edge
      if (prev_ind != -1) {
        half_edge_lib_[prev_ind].next_ = half_edge_index;
      }
      prev_ind = half_edge_index;

      // set start half edge for vertex
      auto& vertex = vertex_lib_[vertex_loop[vert_ind]];
      if (!vertex.HasStartHalfEdge()) {
        vertex.start_half_edge_ = half_edge_index;
      }
      // push
      half_edge_lib_.push_back(half_edge);
      half_edge_index++;
    }
    // close the loop
    if (prev_ind != -1) {
      half_edge_lib_[prev_ind].next_ = start_ind;
    }

    Face face_(half_edge_index - vertex_loop.size(), vertex_loop.size());
    face_lib_.push_back(face_);
  }

  for (auto& edge : half_edge_lib_) {
    edge.Print();
  }

  // 3. calc vertex degree
  for (auto& edge : half_edge_lib_) {
    vertex_lib_[edge.tail_].vertex_degree_++;
  }
}

void Model::ReadVertex(std::istringstream& sin, std::string s[3]) {
  sin.ignore(2);
  glm::vec3 position_{};
  for (auto i = 0; i < 3; i++) {
    sin >> s[i];
  }
  position_.x = std::stof(s[0]);
  position_.y = std::stof(s[1]);
  position_.z = std::stof(s[2]);
  vertex_buffer_.push_back(position_);
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
  face_buffer_.push_back(face_v_indices);
}

void Model::PrintVertexAndFaces() {
  for (auto i = 0; i < vertex_buffer_.size(); i++) {
    PrintInfo("Vertex%d: %f, %f, %f\n", i, vertex_buffer_[i].x,
              vertex_buffer_[i].y, vertex_buffer_[i].z);
  }
  for (auto i = 0; i < face_buffer_.size(); i++) {
    PrintInfo("Face%d: ", i);
    for (auto j = 0; j < face_buffer_[i].size(); j++) {
      printf("%d ", face_buffer_[i][j]);
    }
    printf("\n");
  }
}

void Model::CheckParseHalfEdgeResult() {
  // check
  bool has_error = false;

  // 检查 Twin 双向一致性
  for (size_t i = 0; i < half_edge_lib_.size(); ++i) {
    HalfEdge& he_A = half_edge_lib_[i];
    HalfEdgeIndex twin_idx = he_A.twin_;

    if (twin_idx == 0) {
      // 这是边界边，跳过 Twin 检查
      continue;
    }

    if (twin_idx >= half_edge_lib_.size()) {
      PrintErr("Error: HE %zu has twin_ %d out of bounds.\n", i, twin_idx);
      has_error = true;
      continue;
    }

    HalfEdge& he_B = half_edge_lib_[twin_idx];

    // 1. 检查对称性：A 的 twin_ 必须是 B，B 的 twin 必须是 A
    if (he_B.twin_ != i) {
      PrintErr(
          "Error: HE %zu (tail_ %d) has twin_ %d, but twin_'s twin_ is %d (not "
          "%zu).\n",
          i, he_A.tail_, twin_idx, he_B.twin_, i);
      has_error = true;
    }

    // 2. 检查方向性：A 的起点必须是 B 的终点，反之亦然
    if (he_A.tail_ != he_B.next_) {  // 假设你有一个获取头部顶点的辅助函数
      // 注意：因为 half_edge 没有 head 属性，需要通过 next 关系或 twin
      // 关系间接判断

      // 简单判断：A 的 tail_ 必须等于 B 的 head
      // B 的 head 是 B 的 twin_ 的 tail
      if (he_A.tail_ != half_edge_lib_[he_B.next_].tail_) {
        PrintErr("Error: HE %zu and twin_ %d have inconsistent vertex tails.\n",
                 i, twin_idx);
        has_error = true;
      }
    }
  }

  if (!has_error) {
    PrintInfo("Half-Edge integrity check passed!\n");
  }
}


void Model::UpdateTwin(std::vector<HalfEdge>& new_half_edge_lib) {
  // 可选，在本样例中实际不需要
  std::map<EdgeKey, HalfEdgeIndex> twin_map;
  for (size_t edge_index = 0; edge_index < new_half_edge_lib.size();
       edge_index++) {
    auto& edge = new_half_edge_lib[edge_index];
    if (edge.HasTwin()) continue;

    auto& next_edge = new_half_edge_lib[edge.next_];
    EdgeKey reverse_key(next_edge.tail_, edge.tail_);
    if (twin_map.count(reverse_key)) {
      // find twin_

      auto twin_index = twin_map[reverse_key];
      edge.twin_ = twin_index;
      new_half_edge_lib[twin_index].twin_ = edge_index;
      twin_map.erase(reverse_key);
    } else {
      EdgeKey key(edge.tail_, next_edge.tail_);
      twin_map[key] = edge_index;
    }
  }
}

void Model::ReTopology(std::vector<HalfEdge>& new_half_edge_lib,
                         std::vector<Face>& new_face_lib) {
  HalfEdgeIndex half_edge_count = 0;
  FaceIndex face_count = 0;
  for (size_t i = 0; i < face_lib_.size(); i++) {
    auto& old_face = face_lib_[i];
    std::vector<VertexIndex> loop_vertex_indices;

    auto old_start_edge_index = old_face.start_half_edge_;
    auto old_current_edge_index = old_start_edge_index;
    do {
      auto& current_edge = half_edge_lib_[old_current_edge_index];

      loop_vertex_indices.push_back(current_edge.tail_);
      loop_vertex_indices.push_back(current_edge.new_vertex_);

      old_current_edge_index = current_edge.next_;
    } while (old_current_edge_index != old_start_edge_index);

    auto& center_vertex_index = old_face.new_vertex_;
    auto indices_size = loop_vertex_indices.size();
    for (size_t vertex_ind = 0; vertex_ind < indices_size; vertex_ind += 2) {
      auto v1_index = center_vertex_index,
           v2_index = loop_vertex_indices[(vertex_ind + indices_size - 1) %
                                          indices_size],

           v3_index = loop_vertex_indices[vertex_ind],

           v4_index = loop_vertex_indices[(vertex_ind + 1) % indices_size];

      HalfEdge e1, e2, e3, e4;
      e1.tail_ = v1_index;
      e2.tail_ = v2_index;
      e3.tail_ = v3_index;
      e4.tail_ = v4_index;

      e1.next_ = half_edge_count + 1;
      e2.next_ = half_edge_count + 2;
      e3.next_ = half_edge_count + 3;
      e4.next_ = half_edge_count;

      e1.face_ = face_count;
      e2.face_ = face_count;
      e3.face_ = face_count;
      e4.face_ = face_count;

      Face new_face;

      new_face.face_degree_ = 4;
      new_face.start_half_edge_ = half_edge_count;

      new_half_edge_lib.push_back(e1);
      new_half_edge_lib.push_back(e2);
      new_half_edge_lib.push_back(e3);
      new_half_edge_lib.push_back(e4);
      half_edge_count += 4;

      new_face_lib.push_back(new_face);
      face_count += 1;
    }
  }
}


void Model::ExportToObj(const char* filename) {
  // 1. 创建文件输出流对象
  std::ofstream output_file(filename);

  // 2. 检查文件是否成功打开
  if (!output_file.is_open()) {
    std::cerr << "Error: Could not open file for writing: " << filename
              << std::endl;
    return;
  }

  // 写入文件头信息
  output_file << "# OBJ file generated by Half-Edge Subdivider" << std::endl;
  output_file << "# Vertices: " << vertex_lib_.size() << std::endl;

  // --- 3. 写入所有顶点 (v) ---
  for (const auto& vertex : vertex_lib_) {
    // 使用文件流的 << 运算符，而不是 printf
    output_file << vertex.ToObjString();
  }

  output_file << "\n# Faces: " << face_lib_.size() << std::endl;

  // --- 4. 写入所有面 (f) ---
  for (const auto& face_ : face_lib_) {
    // 使用文件流的 << 运算符
    output_file << face_.ToObjString(half_edge_lib_);
  }

  // 5. 关闭文件流并确认成功
  output_file.close();
  std::cout << "Successfully exported OBJ file to: " << filename << std::endl;
}