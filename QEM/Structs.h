#pragma once
#include "TypeAlias.h"
#include <glm/glm.hpp>

struct Quadric {
  // [ 0,  1,  2,  3]
  // [ -,  4,  5,  6]
  // [ -,  -,  7,  8]
  // [ -,  -,  -,  9]
  double data_[10];
  Quadric() {
    for (int i = 0; i < 10; i++) {
      data_[i] = 0;
    }
  }

  Quadric FromPlane(const glm::vec4 &plane) {
    Quadric result;

    auto a = plane.x;
    auto b = plane.y;
    auto c = plane.z;
    auto d = plane.w;

    data_[0] = a * a;
    data_[1] = a * b;
    data_[2] = a * c;
    data_[3] = a * d;

    data_[4] = b * b;
    data_[5] = b * c;
    data_[6] = b * d;

    data_[7] = c * c;
    data_[8] = c * d;

    data_[9] = d * d;

    return result;
  }

  Quadric operator+(const Quadric &other) const {
    Quadric result;
    for (int i = 0; i < 10; i++) {
      result.data_[i] = this->data_[i] + other.data_[i];
    }
    return result;
  }
};

struct VertexPair {
  VertexIndex first_;
  VertexIndex second_;
  double cost_;
  bool deleted_;

  VertexPair() : first_(-1), second_(-1), cost_(0), deleted_(false) {}
};
