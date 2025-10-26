#include "KdTree.h"
#include "../general/Tools.h"

void KdTree::SearchKNearestNode(int node_idx, const vec3 &query_point,
                                int k, KNNQueue &queue) const {
  if (node_idx == -1 || node_idx >= nodes_.size())
    return;

  const Node &node = nodes_[node_idx];

  const Photon &median = photons_[node.idx_];

  queue.emplace(GetSqrDist(query_point, median.destination_), node.idx_);

  if (queue.size() > k) {
    queue.pop();
  }

  const bool is_lower =
      query_point[node.axis_] < median.destination_[node.axis_];
  if (is_lower) {
    SearchKNearestNode(node.left_child_idx, query_point, k, queue);
  } else {
    SearchKNearestNode(node.right_child_idx, query_point, k, queue);
  }

  const float dist_to_siblings =
      median.destination_[node.axis_] - query_point[node.axis_];

  if (queue.top().first > dist_to_siblings * dist_to_siblings) {
    if (is_lower) {
      SearchKNearestNode(node.right_child_idx, query_point, k, queue);
    } else {
      SearchKNearestNode(node.left_child_idx, query_point, k, queue);
    }
  }
}
void KdTree::SetPhotons(const Photon *photons, int num_photons) {
  photons_ = photons;
  num_photons_ = num_photons;
}
void KdTree::BuildTree() {
  std::vector<int> indices(num_photons_);
  iota(indices.begin(), indices.end(), 0);

  BuildNode(indices.data(), num_photons_, 0);
}

void KdTree::BuildNode(int *indices, int n_photons, int depth) {
  if (n_photons <= 0) {
    return;
  }

  const int axis = depth % 3;

  std::sort(indices, indices + n_photons, [&](const int idx1, const int idx2) {
    return photons_[idx1].destination_[axis] <
           photons_[idx2].destination_[axis];
  });

  const int mid = (n_photons - 1) / 2;

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
  BuildNode(indices + mid + 1, n_photons - mid - 1, depth + 1);

  if (right_child_idx == nodes_.size()) {
    nodes_[parent_idx].right_child_idx = -1;
  } else {
    nodes_[parent_idx].right_child_idx = right_child_idx;
  }
}
std::vector<NeighborPhoton> KdTree::SearchKNearest(const vec3 &query_point,
                                                   int k,
                                                   float &max_dist2) const {
  KNNQueue queue;
  SearchKNearestNode(0, query_point, k, queue);

  std::vector<NeighborPhoton> neighbors(queue.size());
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
