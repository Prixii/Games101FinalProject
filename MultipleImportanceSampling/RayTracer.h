#pragma once
#include <cmath>
#include <cstddef>
#include <glm/glm.hpp>
#include <utility>
#include <vector>

#include "BasicMesh.h"
#include "MISConfig.h"
#include "Ray.h"
#include "Structs.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"

class RayTracer {
 private:
  std::vector<glm::vec3> pixels;

 public:
  RayTracer() = default;
  ~RayTracer() = default;

  void Init();

  std::vector<glm::vec3> RayTracing(BasicMesh &mesh);

  Ray CreateRay(int x, int y, glm::vec3 &right, glm::vec3 &up);

  glm::vec3 TracePath(Ray &ray, BasicMesh &mesh);

  std::pair<bool, Intersection> ClosestIntersection(Ray &ray, BasicMesh &mesh);
};
