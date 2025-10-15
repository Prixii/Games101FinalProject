#pragma once

#include <vector>

#include "Sphere.h"
#include "Triangle.h"

class Model {
 public:
  std::vector<Triangle> triangles;
  std::vector<Sphere> spheres;

  Model() {}

  void LoadTestModel();
};
