#pragma once
#include <glm/glm.hpp>
#include <map>
#include <queue>
#include <vector>

#include "../general/Tools.h"
#include "HalfEdge.h"
#include "Model.h"
#include "QemConfig.h"
#include "Structs.h"
#include "TypeAlias.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"

struct CompareCostIndex {
private:
  const std::vector<VertexPairCost> &cost_data_;

public:
  CompareCostIndex(const std::vector<VertexPairCost> &cost_data)
      : cost_data_(cost_data) {}

  bool operator()(VertexPairCostIndex a, VertexPairCostIndex b) const {
    return cost_data_[a].cost_ > cost_data_[b].cost_;
  }
};

class QemHelper {
  using CostResult = std::pair<std::map<VertexPair, VertexPairCostIndex>,
                               std::vector<VertexPairCost>>;
  using CostHeap =
      std::priority_queue<VertexPairCostIndex, std::vector<VertexPairCostIndex>,
                          CompareCostIndex>;

public:
  QemHelper() : cost_heap_(CompareCostIndex(cost_data_)) {}

  void DoQem(const Model &model) {
    NOT_IMPLEMENTED
    // 1. calculate abcd for each triangle
    // 2. calculate error for each triangle
    error_map_ = CalcErrorForAllFaces(model);

    // 3. calculate Q for each vertex
    vertex_quadrics_ = CalcQuadricForAllVertices(model);

    // 4. select all vertex pairs
    // 5. calculate new vertex v
    // 6. calculate cost
    auto [cost_map_, cost_data_list_] = FindVertexPairAndCalcCost(model);

    // 7. find possible vertex pairs and build heap
    InitCostHeap();

    // 8. select best vertex pair
    VertexPairCostIndex best_pair_idx;
    VertexPairCost cost_info;
    do {
      best_pair_idx = cost_heap_.top();
      cost_info = cost_data_list_[best_pair_idx];
      cost_heap_.pop();
    } while (cost_info.deleted_);

    // 9. collapse vertex pair, update model
    auto [v1_idx, v2_idx] = cost_info.pair_;
    auto v1 = model.vertex_lib_[v1_idx];
    auto v2 = model.vertex_lib_[v2_idx];

    { // collapse pair
      auto new_v = cost_info.new_vertex_;

      auto v1_adjacent_edge_indices = model.GetAdjacentHalfEdgeIndices(v1);
      auto v1_adjacent_face_indices = model.GetAdjacentFaceIndices(v1);
      auto v1_adjacent_vertex_indices = model.GetAdjacentVertexIndices(v1);

      auto v2_adjacent_edge_indices = model.GetAdjacentHalfEdgeIndices(v2);
      auto v2_adjacent_face_indices = model.GetAdjacentFaceIndices(v2);
      auto v2_adjacent_vertex_indices = model.GetAdjacentVertexIndices(v2);

      HalfEdge v1_common_edge, v2_common_edge;
      for (auto edge_idx : v1_adjacent_edge_indices) {
        auto edge = model.half_edge_lib_[edge_idx];
        if (edge.HasTwin()) {
          auto twin_edge = model.half_edge_lib_[edge.twin_];
          if (twin_edge.tail_ == v2_idx) {
            v1_common_edge = edge;
            v2_common_edge = twin_edge;
            break;
          }
        }
      }

      auto common_face_indices = model.GetAdjacentFaceIndices(v1_common_edge);

      auto [v1_ring_edges, v1_ring_vertices] = model.GetRingEdges(v1_idx);
      auto [v2_ring_edges, v2_ring_vertices] = model.GetRingEdges(v2_idx);

      // TODO
    }

    // 10. repeat until convergence
  }

private:
  std::vector<VertexPairCost> cost_data_;
  CostHeap cost_heap_;
  std::map<VertexPair, VertexPairCostIndex> cost_map_;
  std::map<VertexIndex, Quadric> vertex_quadrics_;
  std::map<FaceIndex, Quadric> error_map_;

  // ax + by + cz + d = 0; a^2 + b^2 + c^2 = 1;
  // return [a, b, c, d]
  glm::vec4 CalcPlaneOfTriangle(const glm::vec3 &v1, const glm::vec3 &v2,
                                const glm::vec3 &v3) const;

  std::map<FaceIndex, Quadric> CalcErrorForAllFaces(const Model &model) const;

  std::map<VertexIndex, Quadric> CalcQuadricForAllVertices(const Model &model);

  CostResult FindVertexPairAndCalcCost(const Model &model) {
    std::vector<VertexPairCost> cost_data;

    std::map<VertexPair, VertexPairCostIndex> cost_map;
    for (VertexIndex i = 0; i < model.vertex_lib_.size(); ++i) {
      auto v1_idx = i;
      auto &v1 = model.vertex_lib_[v1_idx];
      auto adjacent_vertices = model.GetAdjacentVertexIndices(v1);
      for (auto v2_idx : adjacent_vertices) {
        auto vertex_pair = VertexPair(v1_idx, v2_idx);
        if (cost_map.find(vertex_pair) != cost_map.end()) {
          continue;
        }

        auto pair_cost = CalcPairCost(model, v1_idx, v2_idx);

        cost_data.push_back(pair_cost);

        cost_map[vertex_pair] = cost_data.size() - 1;
      }
    }
    return {cost_map, cost_data};
  }

  VertexPairCost CalcPairCost(const Model &model, VertexIndex &v1_idx,
                              VertexIndex &v2_idx) {
    auto &v1 = model.vertex_lib_[v1_idx];
    auto &v2 = model.vertex_lib_[v2_idx];
    auto q = vertex_quadrics_[v1_idx] + vertex_quadrics_[v2_idx];
    auto q_mat = q.GetMatrix();
    // calculate new vertex v
    q_mat[3][0] = 0;
    q_mat[3][1] = 0;
    q_mat[3][2] = 0;
    q_mat[3][3] = 1;

    glm::vec4 new_vertex = glm::vec4(0.0f);

    if (glm::determinant(q_mat) == 0) {
      // cannot inverse, interpolate
      auto mix = glm::mix(v1.position_, v2.position_, 0.5f);
      new_vertex = glm::vec4(mix, 1.0f);
    } else {
      auto inverse = glm::inverse(q_mat);
      new_vertex = inverse * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    auto cost = glm::dot(new_vertex, q_mat * new_vertex);

    VertexPairCost pair_cost;
    pair_cost.pair_ = VertexPair(v1_idx, v2_idx);
    pair_cost.cost_ = cost;
    pair_cost.new_vertex_ = new_vertex;

    return pair_cost;
  }

  void InitCostHeap() {
    for (VertexPairCostIndex i = 0; i < cost_data_.size(); ++i) {
      auto &cost_data = cost_data_[i];
      if (cost_data.cost_ > COST_THRESHOLD) {
        cost_heap_.push(i);
      } else {
        cost_data.deleted_ = true;
      }
    }
  }
};
