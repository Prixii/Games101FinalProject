#pragma once
#include <Eigen/Core>
#include <execution>
#include <queue>
#include <stack>
#include <vector>

struct KdTreeNode {
  int idx_ = -1;
  size_t point_idx_ = 0;
  int split_axis_ = 0;
  double split_threval_ = 0.f;
  KdTreeNode* left_ = nullptr;
  KdTreeNode* right_ = nullptr;

  bool IsLeaf() const { return left_ == nullptr && right_ == nullptr; }
};

struct NodeAndDistance {
  NodeAndDistance(double distance, KdTreeNode* node)
      : distance_(distance), node_(node) {}

  double distance_ = 0.f;
  KdTreeNode* node_ = nullptr;

  bool operator<(const NodeAndDistance& other) const {
    return distance_ < other.distance_;
  }
};

template <int dim>
class KdTree {
 public:
  using PointType = Eigen::Matrix<double, dim, 1>;

  explicit KdTree() = default;
  ~KdTree() { Clear(); }

  bool Build(const std::vector<PointType>& cloud);

  bool GetKnnPoints(const PointType& point, std::vector<size_t>& knn_idx,
                    int k = 5);

  bool GetKnnPointsMt(const std::vector<PointType>& cloud,
                      std::vector<std::pair<size_t, size_t>>& matches,
                      int k = 5);

  void Clear();

  template <int dim>
  friend std::ostream& operator<<(std::ostream& out, const KdTree<dim>& tree) {
    auto root = tree.root_.get();

    std::queue<KdTreeNode*> que;
    que.push(root);

    while (!que.empty()) {
      int size = que.size();

      for (int i = 0; i < size; ++i) {
        KdTreeNode* node = que.front();
        que.pop();

        if (node->is_leaf()) {
          out << "Leaf Node index: " << node->point_idx_ << std::endl;
          out << "Point Cloud: " << std::endl
              << tree.m_cloud[node->point_idx_] << std::endl;
        }

        if (node->left_) que.push(node->left_);
        if (node->right_) que.push(node->right_);
      }
    }
    return out;
  }

 private:
  void Search(const PointType& point,
              std::priority_queue<NodeAndDistance>& knn_result);

  bool NeedPruneBranches(const PointType& point, KdTreeNode* node,
                         std::priority_queue<NodeAndDistance>& knn_result);

  void ComputeSplitParameters(const std::vector<size_t>& points_index,
                              double& thre, int& axis,
                              std::vector<size_t>& left,
                              std::vector<size_t>& right);

  void ComputLeafDistance(const PointType& point, KdTreeNode* node,
                          std::priority_queue<NodeAndDistance>& knn_result);

  std::shared_ptr<KdTreeNode> root_ = nullptr;
  int k_;
  std::vector<PointType> cloud_;
};
