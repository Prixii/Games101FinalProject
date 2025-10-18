#pragma once
#include <Eigen/Core>
#include <execution>
#include <numeric>
#include <queue>
#include <stack>
#include <vector>

struct KdTreeNode {
  int idx_ = -1;
  size_t point_idx_ = 0;
  int split_axis_ = 0;
  double split_threval_ = 0.f;
  KdTreeNode *left_ = nullptr;
  KdTreeNode *right_ = nullptr;

  bool IsLeaf() const { return left_ == nullptr && right_ == nullptr; }
};

struct NodeAndDistance {
  NodeAndDistance(KdTreeNode *node, double distance)
      : distance_(distance), node_(node) {}

  double distance_ = 0.f;
  KdTreeNode *node_ = nullptr;

  bool operator<(const NodeAndDistance &other) const {
    return distance_ < other.distance_;
  }
};

template <int dim> class KdTree {
public:
  using PointType = Eigen::Matrix<double, dim, 1>;

  explicit KdTree() = default;
  ~KdTree() { Clear(); }

  bool Build(const std::vector<PointType> &cloud);

  bool GetKnnPoints(const PointType &point, std::vector<size_t> &knn_idx,
                    int k = 5);

  bool GetKnnPointsMt(const std::vector<PointType> &cloud,
                      std::vector<std::pair<size_t, size_t>> &matches,
                      int k = 5);

  void Clear();

  friend std::ostream &operator<<(std::ostream &out, const KdTree<dim> &tree) {
    auto root = tree.root_.get();

    std::queue<KdTreeNode *> que;
    que.push(root);

    while (!que.empty()) {
      int size = que.size();

      for (int i = 0; i < size; ++i) {
        KdTreeNode *node = que.front();
        que.pop();

        if (node->IsLeaf()) {
          out << "Leaf Node index: " << node->point_idx_ << std::endl;
          out << "Point Cloud: " << std::endl
              << tree.cloud_[node->point_idx_] << std::endl;
        }

        if (node->left_)
          que.push(node->left_);
        if (node->right_)
          que.push(node->right_);
      }
    }
    return out;
  }

private:
  void Search(const PointType &point,
              std::priority_queue<NodeAndDistance> &knn_result);

  bool NeedPruneBranches(const PointType &point, KdTreeNode *node,
                         std::priority_queue<NodeAndDistance> &knn_result);

  void ComputeSplitParameters(const std::vector<size_t> &points_index,
                              double &thre, int &axis,
                              std::vector<size_t> &left,
                              std::vector<size_t> &right);

  void ComputLeafDistance(const PointType &point, KdTreeNode *node,
                          std::priority_queue<NodeAndDistance> &knn_result);

  std::shared_ptr<KdTreeNode> root_ = nullptr;
  int k_;
  std::vector<PointType> cloud_;
};

template <int dim>
inline bool KdTree<dim>::Build(const std::vector<PointType> &cloud) {
  if (cloud.empty())
    return false;

  cloud_ = cloud;
  std::vector<size_t> cloud_idx(cloud_.size());
  std::for_each(cloud_idx.begin(), cloud_idx.end(),
                [idx = 0](size_t &i) mutable { i = idx++; });

  root_ = std::make_shared<KdTreeNode>();

  std::queue<std::pair<KdTreeNode *, std::vector<size_t>>> que;
  que.push({root_.get(), cloud_idx});

  while (!que.empty()) {
    auto size = que.size();
    for (size_t i = 0; i < size; ++i) {
      auto node_to_cloud = que.front();
      que.pop();

      auto node_temp = node_to_cloud.first;
      auto cloud_idx_temp = node_to_cloud.second;

      std::vector<size_t> left_cloud_idx, right_cloud_idx;
      ComputeSplitParameters(cloud_idx_temp, node_temp->split_threval_,
                             node_temp->split_axis_, left_cloud_idx,
                             right_cloud_idx);

      const auto create_leaf_node = [&que](const std::vector<size_t> &index,
                                           KdTreeNode *&node) {
        if (index.empty()) {
          return;
        }

        node = new KdTreeNode();
        if (index.size() == 1) {
          node->point_idx_ = index[0];
        } else {
          que.push({node, index});
        }
      };

      create_leaf_node(left_cloud_idx, node_temp->left_);
      create_leaf_node(right_cloud_idx, node_temp->right_);
    }
  }
  return true;
}

template <int dim>
inline bool KdTree<dim>::GetKnnPoints(const PointType &point,
                                      std::vector<size_t> &knn_idx, int k) {
  k_ = k;

  std::priority_queue<NodeAndDistance> knn_result;
  Search(point, knn_result);

  knn_idx.clear();

  while (!knn_result.empty()) {
    knn_idx.push_back(knn_result.top().node_->point_idx_);
    knn_result.pop();
  }

  return true;
}

template <int dim>
inline bool
KdTree<dim>::GetKnnPointsMt(const std::vector<PointType> &cloud,
                            std::vector<std::pair<size_t, size_t>> &matches,
                            int k) {
  if (cloud.empty()) {
    return false;
  }

  matches.resize(cloud.size() * k);

  std::vector<size_t> index(cloud.size());
  std::for_each(index.begin(), index.end(),
                [idx = 0](size_t &i) mutable { i = idx++; });

  std::for_each(std::execution::par_unseq, index.begin(), index.end(),
                [&](size_t &i) {
                  std::vector<size_t> knn_idx;
                  GetKnnPoints(cloud[i], knn_idx, k);

                  for (size_t j = 0; j < k; ++j) {
                    auto idx = i * k + j;
                    matches[idx].second = knn_idx[j];

                    if (j >= knn_idx.size()) {
                      matches[idx].first = std::numeric_limits<size_t>::max();
                    } else {
                      matches[idx].first = i;
                    }
                  }
                });
  return true;
}

template <int dim> inline void KdTree<dim>::Clear() {
  std::stack<KdTreeNode *> st;
  auto root = root_.get();
  if (root) {
    if (root->right_) {
      st.push(root->right_);
    }
    if (root->left_) {
      st.push(root->left_);
    }
  }

  while (!st.empty()) {
    auto node_temp = st.top();
    st.pop();

    if (node_temp->right_) {
      st.push(node_temp->right_);
    }
    if (node_temp->left_) {
      st.push(node_temp->left_);
    }
    delete node_temp;
  }
}

template <int dim>
inline void
KdTree<dim>::Search(const PointType &point,
                    std::priority_queue<NodeAndDistance> &knn_result) {
  std::stack<KdTreeNode *> st;
  st.push(root_.get());

  while (!st.empty()) {
    auto node_temp = st.top();
    st.pop();

    if (node_temp->IsLeaf()) {
      ComputLeafDistance(point, node_temp, knn_result);
    } else {
      KdTreeNode *this_side, *that_side;
      if (point[node_temp->split_axis_] < node_temp->split_threval_) {
        this_side = node_temp->left_;
        that_side = node_temp->right_;
      } else {
        this_side = node_temp->right_;
        that_side = node_temp->left_;
      }

      if (NeedPruneBranches(point, node_temp, knn_result)) {
        if (that_side) {
          st.push(that_side);
        }
      }
      if (this_side) {
        st.push(this_side);
      }
    }
  }
}

template <int dim>
inline bool KdTree<dim>::NeedPruneBranches(
    const PointType &point, KdTreeNode *node,
    std::priority_queue<NodeAndDistance> &knn_result) {
  if (knn_result.size() < k_) {
    return true;
  }

  auto dist = point[node->split_axis_] - node->split_threval_;
  if (dist * dist < knn_result.top().distance_) {
    return true;
  }

  return false;
}

template <int dim>
inline void KdTree<dim>::ComputeSplitParameters(
    const std::vector<size_t> &points_index, double &thre, int &axis,
    std::vector<size_t> &left, std::vector<size_t> &right) {
  using DimVector = Eigen::Matrix<double, dim, 1>;
  DimVector means;
  DimVector sum_of_points = std::accumulate(
      points_index.begin(), points_index.end(), DimVector(DimVector::Zero()),
      [&](const DimVector &sum, const size_t &index) -> DimVector {
        return sum + cloud_[index];
      });
  means = sum_of_points / points_index.size();

  Eigen::Matrix<double, dim, 1> vars;
  vars = std::accumulate(points_index.begin(), points_index.end(),
                         DimVector(DimVector::Zero()),
                         [&](const DimVector &sum, const int &index) {
                           auto diff = cloud_[index] - means;
                           return (sum + diff.cwiseAbs2()).eval();
                         });

  vars.maxCoeff(&axis);
  thre = means[axis];

  std::for_each(points_index.begin(), points_index.end(),
                [&](const size_t &idx) {
                  if (cloud_[idx][axis] < thre) {
                    left.push_back(idx);
                  } else {
                    right.push_back(idx);
                  }
                });
}

template <int dim>
inline void KdTree<dim>::ComputLeafDistance(
    const PointType &point, KdTreeNode *node,
    std::priority_queue<NodeAndDistance> &knn_result) {
  auto distance = (cloud_[node->point_idx_] - point).squaredNorm();
  if (knn_result.size() < k_) {
    knn_result.push(NodeAndDistance(node, distance));
  } else {
    if (distance < knn_result.top().distance_) {
      knn_result.emplace(NodeAndDistance(node, distance));
      knn_result.pop();
    }
  }
}
