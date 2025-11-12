#include "QemHelper.h"
glm::vec4 QemHelper::CalcPlaneOfTriangle(const glm::vec3 &v1,
                                         const glm::vec3 &v2,
                                         const glm::vec3 &v3) const {
  auto normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
  auto d = -glm::dot(normal, v1);
  return glm::vec4(normal, d);
}
std::map<FaceIndex, Quadric>
QemHelper::CalcErrorForAllFaces(const Model &model) const {
  std::map<FaceIndex, Quadric> error_map;
  auto faces = model.face_lib_;
  for (FaceIndex i = 0; i < faces.size(); i++) {
    auto vertices =
        faces[i].GetVertices(model.half_edge_lib_, model.vertex_lib_);
    auto &v0 = vertices[0];
    auto &v1 = vertices[1];
    auto &v2 = vertices[2];

    auto plane = CalcPlaneOfTriangle(v0.position_, v1.position_, v2.position_);

    auto q = Quadric(plane);
    error_map[i] = q;
  }
  return error_map;
}
std::map<VertexIndex, Quadric>
QemHelper::CalcQuadricForAllVertices(const Model &model) {
  std::map<VertexIndex, Quadric> vertex_quadrics;
  auto vertices = model.vertex_lib_;
  for (FaceIndex i = 0; i < vertices.size(); i++) {

    auto &vertex = vertices[i];
    auto q = Quadric();

    auto face_indices = model.GetAdjacentFaceIndices(vertex);
    for (auto face_index : face_indices) {
      auto &q_of_face = error_map_[face_index];
      q += q_of_face;
    }

    vertex_quadrics[i] = q;
  }
  return vertex_quadrics;
}
