#include "Model.h"
#include "Sphere.h"
#include "Triangle.h"
#include "glm/ext/vector_float3.hpp"

inline void Model::LoadTestModel() {
  triangles_.clear();
  triangles_.reserve(5 * 2 * 3);

  float L = 555.f;

  glm::vec3 A(L, 0, 0);
  glm::vec3 B(0, 0, 0);
  glm::vec3 C(L, 0, L);
  glm::vec3 D(0, 0, L);

  glm::vec3 E(L, L, 0);
  glm::vec3 F(0, L, 0);
  glm::vec3 G(L, L, L);
  glm::vec3 H(0, L, L);

  const glm::vec3 white(1, 1, 1);
  const glm::vec3 green(0, 1, 0);
  const glm::vec3 red(1, 0, 0);
  const glm::vec3 blue(0, 0, 1);

  triangles_.push_back(Triangle(C, B, A, white));
  triangles_.push_back(Triangle(C, D, B, white));

  triangles_.push_back(Triangle(A, E, C, red));
  triangles_.push_back(Triangle(C, E, G, red));

  triangles_.push_back(Triangle(F, B, D, blue));
  triangles_.push_back(Triangle(H, F, D, blue));

  triangles_.push_back(Triangle(E, F, G, green));
  triangles_.push_back(Triangle(F, H, G, green));

  triangles_.push_back(Triangle(G, D, C, white));
  triangles_.push_back(Triangle(G, H, D, white));

  triangles_.push_back(Triangle(F, E, B, white));
  triangles_.push_back(Triangle(B, E, A, white));

  spheres_.push_back(Sphere(glm::vec3(130, 90, 130), 100.f));

  A = glm::vec3(423, 0, 247);
  B = glm::vec3(265, 0, 296);
  C = glm::vec3(472, 0, 406);
  D = glm::vec3(314, 0, 456);

  E = glm::vec3(423, 330, 247);
  F = glm::vec3(265, 330, 296);
  G = glm::vec3(472, 330, 406);
  H = glm::vec3(314, 330, 456);

  // Front
  triangles_.push_back(Triangle(E, B, A, white));
  triangles_.push_back(Triangle(E, F, B, white));

  // Front
  triangles_.push_back(Triangle(F, D, B, white));
  triangles_.push_back(Triangle(F, H, D, white));

  // BACK
  triangles_.push_back(Triangle(H, C, D, white));
  triangles_.push_back(Triangle(H, G, C, white));

  // LEFT
  triangles_.push_back(Triangle(G, E, C, white));
  triangles_.push_back(Triangle(E, A, C, white));

  // TOP
  triangles_.push_back(Triangle(G, F, E, white));
  triangles_.push_back(Triangle(G, H, F, white));

  for (auto &triangle : triangles_) {
    triangle.v0 *= 2 / L;
    triangle.v1 *= 2 / L;
    triangle.v2 *= 2 / L;

    triangle.v0 -= glm::vec3(1, 1, 1);
    triangle.v1 -= glm::vec3(1, 1, 1);
    triangle.v2 -= glm::vec3(1, 1, 1);

    triangle.v0.x *= -1;
    triangle.v1.x *= -1;
    triangle.v2.x *= -1;

    triangle.v0.y *= -1;
    triangle.v1.y *= -1;
    triangle.v2.y *= -1;

    triangle.UpdateNormal();
  }

  for (auto &sphere : spheres_) {
    sphere.radius_ *= 2 / L;
    sphere.center_ *= 2 / L;

    sphere.center_ -= glm::vec3(1, 1, 1);

    sphere.center_.x *= -1;
    sphere.center_.y *= -1;
  }
}
