#pragma once
#include "Photon.h"
#include "glm/ext/vector_float3.hpp"
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

  void BuildNode(int *indices, int n_photons, int depth) { NOT_IMPLEMENTED }

  void SearchKNearestNode(int node_idx, const glm::vec3 &query_point, int k,
                          KNNQueue &queue) const {
    NOT_IMPLEMENTED
  }

public:
  KdTree() = default;

  void SetPhotons(const Photon *photons, int num_photons) {
    photons_ = photons;
    num_photons_ = num_photons;
  }

  void BuildTree() {
    std::vector<int> indices(num_photons_);
    iota(indices.begin(), indices.end(), 0);

    BuildNode(indices.data(), num_photons_, 0);
  }
};
