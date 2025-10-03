#pragma once
#include <algorithm>
class TwoPointIndex {
public:
  int first;
  int second;

  TwoPointIndex() : first(0), second(0) {};
  TwoPointIndex(int f, int s) : first(f), second(s) {
    if (first > second) {
      std::swap(first, second);
    }
  }

  bool operator<(const TwoPointIndex& next) const {
    if (first != next.first) {
      return first < next.first;
    } else {
      return second < next.second;
    }
  }


};
