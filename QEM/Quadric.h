#pragma once
struct Quadric {
  double m[10];

  Quadric() {
    for (int i = 0; i < 10; i++) m[i] = 0.0;
  }

  Quadric operator+(const Quadric& other) const {
    Quadric result;
    for (int i = 0; i < 10; i++) {
      result.m[i] = this->m[i] + other.m[i];
    }
    return result;
  }
};