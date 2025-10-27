#include "KdTree.h"
#include "../general/Tools.h"
#include "HitPoint.h"

void KdTree::SearchKNearestNode(int node_idx, const vec3 &query_point, int k,
                                KNNQueue &queue) const {
  if (node_idx == -1 || node_idx >= nodes_.size())
    return;

  const Node &node = nodes_[node_idx];

  const HitPoint &median = hit_points_[node.idx_];

  queue.emplace(GetSqrDist(query_point, median.position_), node.idx_);

  if (queue.size() > k) {
    queue.pop();
  }

  const bool is_lower = query_point[node.axis_] < median.position_[node.axis_];
  if (is_lower) {
    SearchKNearestNode(node.left_child_idx, query_point, k, queue);
  } else {
    SearchKNearestNode(node.right_child_idx, query_point, k, queue);
  }

  const float dist_to_siblings =
      median.position_[node.axis_] - query_point[node.axis_];

  if (queue.top().first > dist_to_siblings * dist_to_siblings) {
    if (is_lower) {
      SearchKNearestNode(node.right_child_idx, query_point, k, queue);
    } else {
      SearchKNearestNode(node.left_child_idx, query_point, k, queue);
    }
  }
}

void KdTree::SearchNearestNode(int node_idx, const vec3 &query_point,
                               float max_dist2, RangeList &results) const {
  if (node_idx == -1 || node_idx >= nodes_.size()) {
    return;
  }

  const Node &node = nodes_[node_idx];
  const HitPoint &median = hit_points_[node.idx_];

  float dist2 = GetSqrDist(query_point, median.position_);

  if (dist2 <= max_dist2) {
    results.push_back({dist2, node.idx_});
  }

  const bool is_lower = query_point[node.axis_] < median.position_[node.axis_];

  if (is_lower) {
    SearchNearestNode(node.left_child_idx, query_point, max_dist2, results);
  } else {
    SearchNearestNode(node.right_child_idx, query_point, max_dist2, results);
  }

  const float dist_to_plane =
      median.position_[node.axis_] - query_point[node.axis_];
  const float dist_to_plane_sq = dist_to_plane * dist_to_plane;

  if (max_dist2 > dist_to_plane_sq) {

    if (is_lower) {
      SearchNearestNode(node.right_child_idx, query_point, max_dist2, results);
    } else {
      SearchNearestNode(node.left_child_idx, query_point, max_dist2, results);
    }
  }
}

void KdTree::SetHitPoints(const HitPoint *hit_points, int num_hit_points) {
  hit_points_ = hit_points;
  num_hit_points_ = num_hit_points;
}
void KdTree::BuildTree() {
  std::vector<int> indices(num_hit_points_);
  iota(indices.begin(), indices.end(), 0);

  BuildNode(indices.data(), num_hit_points_, 0);
}

void KdTree::BuildNode(int *indices, int n_hit_points, int depth) {
  if (n_hit_points <= 0) {
    return;
  }

  const int axis = depth % 3;

  std::sort(indices, indices + n_hit_points,
            [&](const int idx1, const int idx2) {
              return hit_points_[idx1].position_[axis] <
                     hit_points_[idx2].position_[axis];
            });

  const int mid = (n_hit_points - 1) / 2;

  const auto parent_idx = nodes_.size();

  Node node;
  node.axis_ = axis;
  node.idx_ = indices[mid];
  nodes_.push_back(node);

  const auto left_child_idx = nodes_.size();
  BuildNode(indices, mid, depth + 1);

  if (left_child_idx == nodes_.size()) {
    nodes_[parent_idx].left_child_idx = -1;
  } else {
    nodes_[parent_idx].left_child_idx = left_child_idx;
  }

  const auto right_child_idx = nodes_.size();
  BuildNode(indices + mid + 1, n_hit_points - mid - 1, depth + 1);

  if (right_child_idx == nodes_.size()) {
    nodes_[parent_idx].right_child_idx = -1;
  } else {
    nodes_[parent_idx].right_child_idx = right_child_idx;
  }
}
std::vector<NeighborHitPoint>
KdTree::SearchKNearest(const vec3 &query_point, int k, float &max_dist2) const {
  KNNQueue queue;
  SearchKNearestNode(0, query_point, k, queue);

  std::vector<NeighborHitPoint> neighbors(queue.size());
  max_dist2 = 0;
  for (size_t i = 0; i < neighbors.size(); ++i) {
    const auto &p = queue.top();
    neighbors[i].index_ = p.second;
    neighbors[i].dist_ = sqrt(p.first);
    max_dist2 = std::max(max_dist2, p.first);
    queue.pop();
  }
  return neighbors;
}

std::vector<NeighborHitPoint> KdTree::SearchNearest(const vec3 &query_point,
                                                    float max_dist2) const {

  RangeList hit_point_indices;
  SearchNearestNode(0, query_point, max_dist2, hit_point_indices);

  std::vector<NeighborHitPoint> neighbors(hit_point_indices.size());
  for (size_t i = 0; i < neighbors.size(); ++i) {
    const auto &p = hit_point_indices[i];
    neighbors[i].index_ = p.second;
    neighbors[i].dist_ = sqrt(p.first);
  }
  return neighbors;
}
