#include "ProgressivePhotonUtils.h"

int invisible_time = 0;

bool ClosestIntersection(const vec3 start, const vec3 dir,
                         const std::vector<Triangle> &triangles,
                         const std::vector<Sphere> &spheres,
                         Intersection &intersection) {
  intersection.distance_ = std::numeric_limits<float>::max();
  intersection.triangle_index_ = -1;
  intersection.sphere_index_ = -1;

  bool intersects = false;
  float t0, t1;
  vec3 x0, x1;

  for (size_t ti = 0; ti < triangles.size(); ti++) {
    if (!IsVisible(triangles[ti].normal_, dir)) {
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

  if (intersects && !intersection.IsSphere() && !intersection.IsTriangle()) {
    int i = 0;
  }

  return intersects;
}
vec3 DirectLight(const Intersection &i,
                      const std::vector<Triangle> &triangles,
                      const std::vector<Sphere> &spheres) {
  float r = distance(LIGHT_POS, i.position_);
  vec3 r_hat = normalize(LIGHT_POS - i.position_);
  vec3 n_hat = triangles[i.triangle_index_].normal_;

  Intersection j;
  if (ClosestIntersection(LIGHT_POS, normalize(i.position_ - LIGHT_POS),
                          triangles, spheres, j)) {
    if (j.sphere_index_ >= 0 || (distance(LIGHT_POS, j.position_) < r &&
                                 i.triangle_index_ != j.triangle_index_)) {
      return vec3(0, 0, 0);
    }
  }
  return (LIGHT_COLOR * std::max(dot(r_hat, n_hat), 0.f)) /
         (4.f * PI * r * r);
}
bool IsVisible(vec3 v1, vec3 v2) { 
  return dot(v1, v2) <= 0.f; 
}
