#include "Triangle.h"

inline Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
                          glm::vec3 color)
    : v0(v0), v1(v1), v2(v2), color(color) {
  ComputeNormal();
}

inline Triangle::Triangle()
    : v0(glm::vec3(0.0f)),
      v1(glm::vec3(1.0f, 0.0f, 0.0f)),
      v2(glm::vec3(0.0f, 1.0f, 0.0f)),
      color(glm::vec3(1.0f, 1.0f, 1.0f)) {
  ComputeNormal();
}

inline void Triangle::ComputeNormal() {
  normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
}
