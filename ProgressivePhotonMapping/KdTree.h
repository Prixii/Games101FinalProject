#pragma once
#include "../general/Tools.h"
#include "HitPoint.h"

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <numeric>
#include <queue>
#include <utility>
#include <vector>

using namespace glm;

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
  int num_hit_points_;
  const HitPoint *hit_points_;

  using KNNQueue = std::priority_queue<std::pair<float, int>>;
  using RangeList = std::vector<std::pair<float, int>>;

  void BuildNode(int *indices, int hit_points_count, int depth);

  void SearchKNearestNode(int node_idx, const vec3 &query_point, int k,
                          KNNQueue &queue) const;
  void SearchNearestNode(int node_idx, const vec3 &query_point,
                         float max_dist2, RangeList &results) const;

public:
  KdTree() = default;
  ~KdTree() {
    nodes_.clear();
  }

  void SetHitPoints(const HitPoint *hit_points, int num_hit_points);

  void BuildTree();

  std::vector<NeighborHitPoint> SearchKNearest(const vec3 &query_point, int k,
                                               float &max_dist2) const;
  std::vector<NeighborHitPoint> SearchNearest(const vec3 &query_point,
                                              float max_dist2) const;
};
