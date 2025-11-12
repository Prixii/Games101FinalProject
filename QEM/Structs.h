#pragma once
#include "TypeAlias.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"
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

  // plane: [a, b, c, d]
  Quadric(const glm::vec4 &plane) {

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
  }

  Quadric operator+(const Quadric &other) const {
    Quadric result;
    for (int i = 0; i < 10; i++) {
      result.data_[i] = this->data_[i] + other.data_[i];
    }
    return result;
  }

  Quadric operator+=(const Quadric &other) {
    for (int i = 0; i < 10; i++) {
      data_[i] += other.data_[i];
    }
    return *this;
  }

  glm::mat4 GetMatrix() const {
    glm::mat4 result;
    result[0][0] = data_[0];
    result[0][1] = data_[1];
    result[0][2] = data_[2];
    result[0][3] = data_[3];

    result[1][0] = data_[1];
    result[1][1] = data_[4];
    result[1][2] = data_[5];
    result[1][3] = data_[6];

    result[2][0] = data_[2];
    result[2][1] = data_[5];
    result[2][2] = data_[7];
    result[2][3] = data_[8];

    result[3][0] = data_[3];
    result[3][1] = data_[6];
    result[3][2] = data_[8];
    result[3][3] = data_[9];

    return result;
  }
};

struct VertexPair {
  VertexIndex first_;
  VertexIndex second_;

  VertexPair() : first_(-1), second_(-1) {}
  VertexPair(VertexIndex first, VertexIndex second) {
    first_ = first > second ? second : first;
    second_ = first > second ? first : second;
  }

  bool operator<(const VertexPair &other) const {
    if (first_ != other.first_) {
      return first_ < other.first_;
    }
    return second_ < other.second_;
  }
};

struct VertexPairCost {
  VertexPair pair_;
  glm::vec4 new_vertex_;
  double cost_;
  bool deleted_;

  VertexPairCost() : pair_(-1, -1), new_vertex_(0), cost_(0), deleted_(false) {}
  VertexPairCost(VertexPair pair, glm::vec4 &new_vertex, double cost,
                 bool deleted)
      : pair_(pair), new_vertex_(new_vertex), cost_(cost), deleted_(deleted) {}
};
