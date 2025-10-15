#include "Model.h"

inline void Model::LoadTestModel() {
  triangles.clear();
  triangles.reserve(5 * 2 * 3);

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

  triangles.push_back(Triangle(C, B, A, white));
  triangles.push_back(Triangle(C, D, B, white));

  triangles.push_back(Triangle(A, E, C, red));
  triangles.push_back(Triangle(C, E, G, red));

  triangles.push_back(Triangle(F, B, D, blue));
  triangles.push_back(Triangle(H, F, D, blue));

  triangles.push_back(Triangle(E, F, G, green));
  triangles.push_back(Triangle(F, H, G, green));

  triangles.push_back(Triangle(G, D, C, white));
  triangles.push_back(Triangle(G, H, D, white));

  triangles.push_back(Triangle(F, E, B, white));
  triangles.push_back(Triangle(B, E, A, white));

  spheres.push_back(Sphere(glm::vec3(130, 90, 130), 100.f));

  A = glm::vec3(423, 0, 247);
  B = glm::vec3(265, 0, 296);
  C = glm::vec3(472, 0, 406);
  D = glm::vec3(314, 0, 456);

  E = glm::vec3(423, 330, 247);
  F = glm::vec3(265, 330, 296);
  G = glm::vec3(472, 330, 406);
  H = glm::vec3(314, 330, 456);

  // Front
  triangles.push_back(Triangle(E, B, A, white));
  triangles.push_back(Triangle(E, F, B, white));

  // Front
  triangles.push_back(Triangle(F, D, B, white));
  triangles.push_back(Triangle(F, H, D, white));

  // BACK
  triangles.push_back(Triangle(H, C, D, white));
  triangles.push_back(Triangle(H, G, C, white));

  // LEFT
  triangles.push_back(Triangle(G, E, C, white));
  triangles.push_back(Triangle(E, A, C, white));

  // TOP
  triangles.push_back(Triangle(G, F, E, white));
  triangles.push_back(Triangle(G, H, F, white));

  for (auto &triangle : triangles) {
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

  for (auto &sphere : spheres) {
    sphere.radius *= 2 / L;
    sphere.center *= 2 / L;

    sphere.center -= glm::vec3(1, 1, 1);

    sphere.center.x *= -1;
    sphere.center.y *= -1;
  }
}
