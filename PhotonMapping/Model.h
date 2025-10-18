#pragma once

#include <vector>

#include "Sphere.h"
#include "Triangle.h"

class Model {
public:
  std::vector<Triangle> triangles_;
  std::vector<Sphere> spheres_;

  Model() {}

  void LoadTestModel();
};
