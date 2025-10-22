#include "Triangle.h"

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color)
    : v0(v0), v1(v1), v2(v2), color(color) {
  UpdateNormal();
}

Triangle::Triangle()
    : v0(glm::vec3(0.0f)), v1(glm::vec3(1.0f, 0.0f, 0.0f)),
      v2(glm::vec3(0.0f, 1.0f, 0.0f)), color(glm::vec3(1.0f, 1.0f, 1.0f)) {
  UpdateNormal();
}

void Triangle::UpdateNormal() {
  normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
}
bool Triangle::Intersect(const glm::vec3 &ray_origin,
                         const glm::vec3 &ray_direction, glm::vec3 &x0,
                         float &t0) const {
  // Moller Trumbore intersection algorithm
  auto edge1 = v1 - v0, edge2 = v2 - v0;
  auto b = ray_origin - v0;

  glm::mat3 A(-ray_direction, edge1, edge2);
  auto tuv = glm::inverse(A) * b;

  auto t = tuv.x, u = tuv.y, v = tuv.z;

  if (t < 0 || u < 0 || v < 0 || u + v > 1) {
    return false;
  }
  x0 = v0 + u * edge1 + v * edge2;
  t0 = t;
  return true;
}
