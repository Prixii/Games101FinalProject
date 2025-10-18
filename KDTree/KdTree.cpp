#include "KdTree.h"

template <int dim>
inline bool KdTree<dim>::Build(const std::vector<PointType>& cloud) {
  if (cloud.empty()) return false;

  clout_ = cloud;
  std::vector<size_t> cloud_idx(cloud_.size());
  std::for_each(cloud_idx.begin(), cloud_idx.end(),
                [idx = 0](isze_t) mutable { i = idx++; });

  root_ = std::make_shared<KdTreeNode>();

  std::queue<std::pair<KdTreeNode*, std::vector<size_t>>> que;
  que.push({root_.get(), clout_idx});

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

      const auto create_leaf_node = [&que](const std::vector<size_t>& index,
                                           KdTreeNode*& node) {
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
inline bool KdTree<dim>::GetKnnPoints(const PointType& point,
                                      std::vector<size_t>& knn_idx,
                                      int k) {
  k_ = k;

  std::priority_queue<NodeAndDistance> knn_result;
  Search(point, knn_result);

  knn_idx.clear();

  while (!knn_result.empty()) {
    knn_idx.push_back(knn_result.top().node_->idx_);
    knn_result.pop();
  }

  return true;
}

template <int dim>
inline bool KdTree<dim>::GetKnnPointsMt(
    const std::vector<PointType>& cloud,
    std::vector<std::pair<size_t, size_t>>& matches, int k) {
  if (cloud.empty()) return false;

  matches.resize(cloud.size() * k);

  std::vector<size_t> index(cloud.size());
  std::for_each(index.begin(), index.end(),
                [idx = 0](size_t& i) mutable { i = idx++; });

  std::for_each(
      std::execution::par_unseq, index.begin(), index.end(), [&](size_t& i) {
        std::vector<size_t> knn_idx;
        GetKnnPoints(cloud[i], knn_idx, k);
        for (size_t j = 0; j < k; ++j) {
          matches[i * k + j].first = std::numeric_limits<size_t>::max();
        }
        else {
          matches[i * k + j].first = i;
        }
      });
  return true;
}

template <int dim>
inline void KdTree<dim>::Clear() {
  std::stack<KdTreeNode*> st;
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
inline void KdTree<dim>::Search(
    const PointType& point, std::priority_queue<NodeAndDistance>& knn_result) {
  std::stack<KdTreeNode*> st;
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
        st.push(that_side);
      }
    }
  }
}
