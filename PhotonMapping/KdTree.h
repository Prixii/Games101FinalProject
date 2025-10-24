#pragma once
#include "Photon.h"

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <numeric>
#include <queue>
#include <utility>
#include <vector>

class KdTree {
private:
  struct Node {
    int axis_;
    int idx_;
    int left_child_idx;
    int right_child_idx;

    Node() : axis_(-1), idx_(-1), left_child_idx(-1), right_child_idx(-1) {}
  };

  std::vector<Node> nodes_;
  int num_photons_;
  const Photon *photons_;

  using KNNQueue = std::priority_queue<std::pair<float, int>>;

  void BuildNode(int *indices, int n_photons, int depth);

  void SearchKNearestNode(int node_idx, const vec3 &query_point, int k,
                          KNNQueue &queue) const;

public:
  KdTree() = default;

  void SetPhotons(const Photon *photons, int num_photons);

  void BuildTree();

  std::vector<NeighborPhoton> SearchKNearest(const vec3 &query_point,
                                             int k, float &max_dist2) const;
};
