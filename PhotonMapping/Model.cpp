#include "Model.h"
#include "Sphere.h"
#include "Triangle.h"

#include <glm/glm.hpp>

void Model::LoadTestModel() {

  const vec3 WHITE = vec3(1.0f, 1.0f, 1.0f);
  const vec3 RED = vec3(1.0f, 0.0f, 0.0f);
  const vec3 BLUE = vec3(0.0f, 0.0f, 1.0f);
  const vec3 GREEN = vec3(0.0f, 1.0f, 0.0f);

  triangles_.clear();
  triangles_.reserve(5 * 2 * 3);

  // Define Room
  float L = 555; // Length of Cornell Box side.

  vec3 A(L, 0, 0);
  vec3 B(0, 0, 0);
  vec3 C(L, 0, L);
  vec3 D(0, 0, L);

  vec3 E(L, L, 0);
  vec3 F(0, L, 0);
  vec3 G(L, L, L);
  vec3 H(0, L, L);

  // Floor
  triangles_.push_back(Triangle(C, B, A, WHITE));
  triangles_.push_back(Triangle(C, D, B, WHITE));

  // Left wall
  triangles_.push_back(Triangle(A, E, C, RED));
  triangles_.push_back(Triangle(C, E, G, RED));

  // Right wall
  triangles_.push_back(Triangle(F, B, D, BLUE));
  triangles_.push_back(Triangle(H, F, D, BLUE));

  // Ceiling
  triangles_.push_back(Triangle(E, F, G, WHITE));
  triangles_.push_back(Triangle(F, H, G, WHITE));

  // Back wall
  triangles_.push_back(Triangle(G, D, C, WHITE));
  triangles_.push_back(Triangle(G, H, D, WHITE));

  // Front wall
  triangles_.push_back(Triangle(F, E, B, WHITE));
  triangles_.push_back(Triangle(B, E, A, WHITE));

  // Define the sphere
  spheres_.push_back(Sphere(vec3(130, 90, 130), 90));

  // Define the Tall block
  A = vec3(423, 0, 247);
  B = vec3(265, 0, 296);
  C = vec3(472, 0, 406);
  D = vec3(314, 0, 456);

  E = vec3(423, 330, 247);
  F = vec3(265, 330, 296);
  G = vec3(472, 330, 406);
  H = vec3(314, 330, 456);

  // Front
  triangles_.push_back(Triangle(E, B, A, WHITE));
  triangles_.push_back(Triangle(E, F, B, WHITE));

  // Front
  triangles_.push_back(Triangle(F, D, B, WHITE));
  triangles_.push_back(Triangle(F, H, D, WHITE));

  // BACK
  triangles_.push_back(Triangle(H, C, D, WHITE));
  triangles_.push_back(Triangle(H, G, C, WHITE));

  // LEFT
  triangles_.push_back(Triangle(G, E, C, WHITE));
  triangles_.push_back(Triangle(E, A, C, WHITE));

  // TOP
  triangles_.push_back(Triangle(G, F, E, WHITE));
  triangles_.push_back(Triangle(G, H, F, WHITE));

  // Scale triangles_ to the volume [-1,1]^3
  for (size_t i = 0; i < triangles_.size(); ++i) {
    triangles_[i].v0_ *= 2 / L;
    triangles_[i].v1_ *= 2 / L;
    triangles_[i].v2_ *= 2 / L;

    triangles_[i].v0_ -= vec3(1, 1, 1);
    triangles_[i].v1_ -= vec3(1, 1, 1);
    triangles_[i].v2_ -= vec3(1, 1, 1);

    triangles_[i].v0_.x *= -1;
    triangles_[i].v1_.x *= -1;
    triangles_[i].v2_.x *= -1;

    triangles_[i].v0_.y *= -1;
    triangles_[i].v1_.y *= -1;
    triangles_[i].v2_.y *= -1;

    triangles_[i].UpdateNormal();
  }

  // Scale spheres_ to the volume [-1,1]^3
  for (size_t i = 0; i < spheres_.size(); ++i) {
    spheres_[i].radius_ *= 2 / L;
    spheres_[i].center_ *= 2 / L;
    spheres_[i].center_ -= vec3(1, 1, 1);
    spheres_[i].center_.x *= -1;
    spheres_[i].center_.y *= -1;
  }
}
