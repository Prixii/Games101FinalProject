#include <Eigen/Core>
#include <Eigen/Geometry>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <execution>
#include <iostream>
#include <utility>
#include <vector>

#include "Eigen/src/Core/Matrix.h"
#include "Eigen/src/Geometry/AngleAxis.h"
#include "KdTree.h"

using std::execution::par_unseq;

constexpr double PI = 3.14159265358979323846;

template <typename FuncT> void EvaluateAndCall(FuncT &&func, int run_num) {
  double total_time = 0.f;
  for (int i = 0; i < run_num; i++) {
    auto t1 = std::chrono::high_resolution_clock::now();
    func();
    auto t2 = std::chrono::high_resolution_clock::now();
    auto diff =
        std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1)
            .count();
    total_time += (diff * 1000);
  }
  std::cout << "run cost" << total_time / 1000.f << std::endl;
}

void GenerateSphereCoordinates(std::vector<Eigen::Vector3d> &points,
                               int num_points) {
  points.clear();
  double golden_ratio = (1 + std::sqrt(5)) / 2.f;
  double angle_increment = PI * 2 * golden_ratio;

  for (int i = 0; i < num_points; i++) {
    double t = static_cast<double>(i) / num_points;
    double inclination = std::acos(1 - 2 * t);
    double azimuth = angle_increment * i;

    double x = std::sin(inclination) * std::cos(azimuth);
    double y = std::sin(inclination) * std::sin(azimuth);
    double z = std::cos(inclination);

    points.push_back(Eigen::Vector3d(x, y, z));
  }
}

void TransformToEllipsoid(std::vector<Eigen::Vector3d> &points, double a,
                          double b, double c) {
  for (auto &point : points) {
    point[0] *= a;
    point[1] *= b;
    point[2] *= c;
  }
}

std::vector<Eigen::Vector3d> GenerateEllipsoid() {
  double a = 200;
  double b = 100;
  double c = 150;

  int num_points = 10000;

  std::vector<Eigen::Vector3d> sphere_points;
  GenerateSphereCoordinates(sphere_points, num_points);
  TransformToEllipsoid(sphere_points, a, b, c);

  return sphere_points;
}

void BfNn(const std::vector<Eigen::Vector3d> &source,
          const std::vector<Eigen::Vector3d> &target,
          std::vector<std::pair<size_t, size_t>> &result) {
  std::vector<size_t> index(target.size());
  for_each(index.begin(), index.end(),
           [idx = 0](size_t &i) mutable { i = idx++; });
  const auto bfnn_points = [&source](const Eigen::Vector3d &p) -> int {
    return std::min_element(
               source.begin(), source.end(),
               [&p](const Eigen::Vector3d &p1, const Eigen::Vector3d &p2) {
                 return (p1 - p).norm() < (p2 - p).norm();
               }) -
           source.begin();
  };

  result.resize(target.size());
  for_each(par_unseq, index.begin(), index.end(), [&](size_t i) {
    result[i].second = bfnn_points(target[i]);
    result[i].first = i;
  });
}

void TestKdTree3D() {
  auto points = GenerateEllipsoid();

  Eigen::AngleAxisd rvec(PI / 6, Eigen::Vector3d::UnitZ());

  Eigen::Vector3d T(2, 2, 2);

  std::vector<Eigen::Vector3d> source(points.size());

  std::transform(points.begin(), points.end(), source.begin(),
                 [&rvec, &T](const Eigen::Vector3d &p) {
                   return rvec.toRotationMatrix() * p + T;
                 });

  KdTree<3> kd_tree;
  kd_tree.Build(source);
  std::vector<std::pair<size_t, size_t>> matches;
  kd_tree.GetKnnPointsMt(points, matches, 1);

  /*std::for_each(matches.begin(), matches.end(),
                [](std::pair<size_t, size_t>& match) {
                  std::cout << "Target Index: " << match.first
                            << ", Source Index: " << match.second << std::endl;
                });*/
}

void TestBfMatches() {
  std::vector<Eigen::Vector3d> points = GenerateEllipsoid();
  Eigen::AngleAxisd rvec(PI / 6, Eigen::Vector3d::UnitZ());
  Eigen::Vector3d T(2, 2, 2);
  std::vector<Eigen::Vector3d> source(points.size());
  std::transform(points.begin(), points.end(), source.begin(),
                 [&rvec, &T](const Eigen::Vector3d &p) {
                   return rvec.toRotationMatrix() * p + T;
                 });
  std::vector<std::pair<size_t, size_t>> matches;
  BfNn(source, points, matches);
  // std::for_each(matches.begin(), matches.end(),
  //               [](std::pair<size_t, size_t>& match) {
  //                 std::cout << "Target Index: " << match.first
  //                           << ", Source Index: " << match.second <<
  //                           std::endl;
  //               });
}

void TextKdTreeBfResult() {
  auto points = GenerateEllipsoid();
  Eigen::AngleAxisd rvec(PI / 6, Eigen::Vector3d::UnitZ());
  Eigen::Vector3d T(2, 2, 2);
  std::vector<Eigen::Vector3d> source(points.size());
  std::transform(points.begin(), points.end(), source.begin(),
                 [&rvec, &T](const Eigen::Vector3d &p) {
                   return rvec.toRotationMatrix() * p + T;
                 });
  // kd match
  KdTree<3> kd_tree;
  kd_tree.Build(source);
  std::vector<std::pair<size_t, size_t>> kd_matches;
  kd_tree.GetKnnPointsMt(points, kd_matches, 1);
  // bf match
  std::vector<std::pair<size_t, size_t>> bf_matches;
  BfNn(source, points, bf_matches);
  //
  size_t correct_count = 0;
  for (size_t i = 0; i < kd_matches.size(); i++) {
    if (kd_matches[i].second == bf_matches[i].second) {
      correct_count++;
    }
  }
  std::cout << "Correct Matches: " << correct_count << " out of "
            << kd_matches.size() << std::endl;
}

int main() {
  EvaluateAndCall(TestKdTree3D, 1);
  EvaluateAndCall(TestBfMatches, 1);
  EvaluateAndCall(TextKdTreeBfResult, 1);

  return 0;
}
