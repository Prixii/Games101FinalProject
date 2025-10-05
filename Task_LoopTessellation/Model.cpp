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

  DoCatmullClarkSubdivision();

  ExportToObj("tesslated.obj");
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

      // check twin
      auto head_vertex_ind = vertex_loop[(vert_ind + 1) % vertex_loop.size()];
      auto reverse_edge_key = EdgeKey(head_vertex_ind, half_edge.tail);
      if (edge_map.count(reverse_edge_key)) {
        auto twin_index = edge_map[reverse_edge_key];
        auto& twin_edge = half_edge_lib[twin_index];

        twin_edge.twin = half_edge_index;
        half_edge.twin = twin_index;

        edge_map.erase(reverse_edge_key);
      } else {
        edge_map[EdgeKey(half_edge.tail, head_vertex_ind)] = half_edge_index;
      }

      // set next for previous edge
      if (prev_ind != -1) {
        half_edge_lib[prev_ind].next = half_edge_index;
      }
      prev_ind = half_edge_index;

      // set start half edge for vertex
      auto& vertex = vertex_lib[vertex_loop[vert_ind]];
      if (!vertex.HasStartHalfEdge()) {
        vertex.start_half_edge = half_edge_index;
      }
      // push
      half_edge_lib.push_back(half_edge);
      half_edge_index++;
    }
    // close the loop
    if (prev_ind != -1) {
      half_edge_lib[prev_ind].next = start_ind;
    }

    Face face(half_edge_index - vertex_loop.size(), vertex_loop.size());
    face_lib.push_back(face);
  }

  for (auto& edge : half_edge_lib) {
    edge.Print();
  }

  // 3. calc vertex degree
  for (auto& edge : half_edge_lib) {
    vertex_lib[edge.tail].vertex_degree++;
  }
}

void Model::ReadVertex(std::istringstream& sin, std::string s[3]) {
  sin.ignore(2);
  glm::vec3 position{};
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

void Model::DoCatmullClarkSubdivision() {
  auto new_vertex_lib = std::vector<Vertex>(vertex_lib);
  auto new_vertex_index = vertex_lib.size();
  // 1. calc center for each face
  for (auto& face : face_lib) {
    Vertex center_vertex;
    glm::vec3 center_pos(0.f, 0.f, 0.f);

    auto vertex_indices = face.GetVertexIndices(half_edge_lib);
    for (auto index : vertex_indices) {
      center_pos += vertex_lib[index].position;
    }
    center_pos /= (float)face.face_degree;
    center_vertex.position = center_pos;
    center_vertex.vertex_degree = face.face_degree;

    new_vertex_lib.push_back(center_vertex);
    face.new_vertex = new_vertex_index;
    new_vertex_index++;
  }

  // 2. calc center for each edge
  std::map<HalfEdgeIndex, bool> is_edge_processed;
  for (size_t i = 0; i < half_edge_lib.size(); i++) {
    auto& edge = half_edge_lib[i];
    Vertex center_vertex;
    auto& twin_edge = half_edge_lib[edge.twin];

    if (is_edge_processed.count(i)) {
      continue;
    }

    auto &face_center = new_vertex_lib[face_lib[edge.face].new_vertex],
         &twin_face_center =
             new_vertex_lib[face_lib[twin_edge.face].new_vertex];

    auto tail_pos = vertex_lib[edge.tail].position,
         head_pos = vertex_lib[twin_edge.tail].position,
         face_center_pos = face_center.position,
         twin_face_center_pos = twin_face_center.position;
    auto center_pos =
        (tail_pos + head_pos + face_center_pos + twin_face_center_pos);
    center_pos /= 4.f;
    center_vertex.position = center_pos;
    center_vertex.vertex_degree = 4;

    edge.new_vertex = new_vertex_index;
    twin_edge.new_vertex = new_vertex_index;
    is_edge_processed[i] = true;
    is_edge_processed[edge.twin] = true;

    new_vertex_lib.push_back(center_vertex);
    new_vertex_index++;
  }

  // 3. calc new position of old vertices
  for (size_t i = 0; i < vertex_lib.size(); i++) {
    auto& vertex = vertex_lib[i];
    auto start_edge_index = vertex.start_half_edge;
    auto current_edge_index = start_edge_index;

    glm::vec3 face_position(0.f, 0.f, 0.f), edge_position(0.f, 0.f, 0.f);
    PrintInfo("Vertex: %d\n", i);
    do {
      auto& edge = half_edge_lib[current_edge_index];
      auto edge_tail_pos = vertex_lib[edge.tail].position;
      auto edge_head_pos = vertex_lib[half_edge_lib[edge.twin].tail]
                               .position;  
      auto edge_center_pos = (edge_tail_pos + edge_head_pos) / 2.f;
      edge_position += edge_center_pos;
      printf("Edge: %d <- %d\n", edge.tail, half_edge_lib[edge.twin].tail);
      auto& face = face_lib[edge.face];
      printf("Face: %d\n", edge.face);
      face_position += new_vertex_lib[face.new_vertex].position;

      current_edge_index = half_edge_lib[edge.twin].next;

    } while (current_edge_index != start_edge_index);

    float n = vertex.vertex_degree;
    float n2 = (n * n);
    vertex.new_position = (face_position / n2) + (2.f * edge_position / n2) +
                          ((n - 3) * vertex.position / n);
    new_vertex_lib[i].position = (face_position / n2) +
                                 (2.f * edge_position / n2) +
                                 ((n - 3) * vertex.position / n);
  }

  // 4. Re-topology
  // 4.1 calc face, edge
  std::vector<Face> new_face_lib;
  std::vector<HalfEdge> new_half_edge_lib;
  HalfEdgeIndex half_edge_count = 0;
  FaceIndex face_count = 0;
  for (size_t i = 0; i < face_lib.size(); i++) {
    auto& old_face = face_lib[i];
    std::vector<VertexIndex> loop_vertex_indices;

    auto old_start_edge_index = old_face.start_half_edge;
    auto old_current_edge_index = old_start_edge_index;
    do {
      auto& current_edge = half_edge_lib[old_current_edge_index];

      loop_vertex_indices.push_back(current_edge.tail);
      loop_vertex_indices.push_back(current_edge.new_vertex);

      old_current_edge_index = current_edge.next;
    } while (old_current_edge_index != old_start_edge_index);

    auto& center_vertex_index = old_face.new_vertex;
    auto indices_size = loop_vertex_indices.size();
    for (size_t vertex_ind = 0; vertex_ind < indices_size; vertex_ind += 2) {
      auto v1_index = center_vertex_index,
           v2_index = loop_vertex_indices[(vertex_ind + indices_size - 1) %
                                          indices_size],

           v3_index = loop_vertex_indices[vertex_ind],

           v4_index = loop_vertex_indices[(vertex_ind + 1) % indices_size];

      HalfEdge e1, e2, e3, e4;
      e1.tail = v1_index;
      e2.tail = v2_index;
      e3.tail = v3_index;
      e4.tail = v4_index;

      e1.next = half_edge_count + 1;
      e2.next = half_edge_count + 2;
      e3.next = half_edge_count + 3;
      e4.next = half_edge_count;

      e1.face = face_count;
      e2.face = face_count;
      e3.face = face_count;
      e4.face = face_count;

      Face new_face;

      new_face.face_degree = 4;
      new_face.start_half_edge = half_edge_count;

      new_half_edge_lib.push_back(e1);
      new_half_edge_lib.push_back(e2);
      new_half_edge_lib.push_back(e3);
      new_half_edge_lib.push_back(e4);
      half_edge_count += 4;

      new_face_lib.push_back(new_face);
      face_count += 1;
    }
  }

  // 4.2 set twin
  std::map<EdgeKey, HalfEdgeIndex> twin_map;
  for (size_t edge_index = 0; edge_index < new_half_edge_lib.size();
       edge_index++) {
    auto& edge = new_half_edge_lib[edge_index];
    if (edge.HasTwin()) continue;

    auto& next_edge = new_half_edge_lib[edge.next];
    EdgeKey reverse_key(next_edge.tail, edge.tail);
    if (twin_map.count(reverse_key)) {
      // find twin

      auto twin_index = twin_map[reverse_key];
      edge.twin = twin_index;
      new_half_edge_lib[twin_index].twin = edge_index;
      twin_map.erase(reverse_key);
    } else {
      EdgeKey key(edge.tail, next_edge.tail);
      twin_map[key] = edge_index;
    }
  }

  for (size_t i = 0; i < new_vertex_lib.size(); i++) {
    new_vertex_lib[i].Print(i);
  }

  for (size_t i = 0; i < new_face_lib.size(); i++) {
    new_face_lib[i].Print(i);
  }

  for (size_t i = 0; i < new_half_edge_lib.size(); i++) {
    new_half_edge_lib[i].Print(i);
  }

  // write
  face_lib = std::move(new_face_lib);
  half_edge_lib = std::move(new_half_edge_lib);
  vertex_lib = std::move(new_vertex_lib);
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
  output_file << "# Vertices: " << vertex_lib.size() << std::endl;

  // --- 3. 写入所有顶点 (v) ---
  for (const auto& vertex : vertex_lib) {
    // 使用文件流的 << 运算符，而不是 printf
    output_file << vertex.ToObjString();
  }

  output_file << "\n# Faces: " << face_lib.size() << std::endl;

  // --- 4. 写入所有面 (f) ---
  for (const auto& face : face_lib) {
    // 使用文件流的 << 运算符
    output_file << face.ToObjString(half_edge_lib);
  }

  // 5. 关闭文件流并确认成功
  output_file.close();
  std::cout << "Successfully exported OBJ file to: " << filename << std::endl;
}