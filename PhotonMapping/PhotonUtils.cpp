#include "PhotonUtils.h"

int invisible_time = 0;

bool ClosestIntersection(const glm::vec3 start, const glm::vec3 dir,
                         const std::vector<Triangle> &triangles,
                         const std::vector<Sphere> &spheres,
                         Intersection &intersection) {
  intersection.distance_ = std::numeric_limits<float>::max();
  intersection.triangle_index_ = -1;
  intersection.sphere_index_ = -1;

  bool intersects = false;
  float t0, t1;
  glm::vec3 x0, x1;

  for (size_t ti = 0; ti < triangles.size(); ti++) {
    if (!IsVisible(triangles[ti].normal, dir)) {
      invisible_time++;
      continue;
    }

    if (triangles[ti].Intersect(start, dir, x0, t0) &&
        t0 < intersection.distance_) {
      intersects = true;
      intersection.position_ = x0;
      intersection.distance_ = t0;
      intersection.triangle_index_ = ti;
    }
  }

  for (size_t si = 0; si < spheres.size(); si++) {
    if (spheres[si].Intersect(start, dir, x0, x1, t0, t1) &&
        ((t0 < intersection.distance_ && t0 > 0.0001) ||
         (t1 < intersection.distance_ && t1 > 0.0001))) {
      intersects = true;
      intersection.position_ = t0 > 0 ? x0 : x1;
      intersection.distance_ = t0 > 0 ? t0 : t1;
      intersection.sphere_index_ = si;
    }
  }

  return intersects;
}
glm::vec3 DirectLight(const Intersection &i,
                      const std::vector<Triangle> &triangles,
                      const std::vector<Sphere> &spheres) {
  float r = glm::distance(LIGHT_POS, i.position_);
  glm::vec3 r_hat = glm::normalize(LIGHT_POS - i.position_);
  glm::vec3 n_hat = triangles[i.triangle_index_].normal;

  Intersection j;
  if (ClosestIntersection(LIGHT_POS, glm::normalize(i.position_ - LIGHT_POS),
                          triangles, spheres, j)) {
    if (j.sphere_index_ >= 0 || (glm::distance(LIGHT_POS, j.position_) < r &&
                                 i.triangle_index_ != j.triangle_index_)) {
      return glm::vec3(0, 0, 0);
    }
  }
  return (LIGHT_COLOR * std::max(glm::dot(r_hat, n_hat), 0.f)) /
         (4.f * PI * r * r);
}
bool IsVisible(glm::vec3 v1, glm::vec3 v2) { return glm::dot(v1, v2) <= 0.f; }
