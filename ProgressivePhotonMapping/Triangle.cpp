#include "Triangle.h"

Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 color)
    : v0_(v0), v1_(v1), v2_(v2), color_(color) {
  UpdateNormal();
}

Triangle::Triangle()
    : v0_(vec3(0.0f)),
      v1_(vec3(1.0f, 0.0f, 0.0f)),
      v2_(vec3(0.0f, 1.0f, 0.0f)),
      color_(vec3(1.0f, 1.0f, 1.0f)) {
  UpdateNormal();
}

void Triangle::UpdateNormal() {
  normal_ = normalize(cross(v2_ - v0_, v1_ - v0_));
}
bool Triangle::Intersect(const vec3 &ray_origin,
                         const vec3 &ray_direction, vec3 &x0,
                         float &t0) const {
  // Moller Trumbore intersection algorithm
  const float EPSILON = 0.0000001f;

  vec3 edge1 = v1_ - v0_, edge2 = v2_ - v0_;

  vec3 h = cross(ray_direction, edge2);
  float a = dot(edge1, h);

  if (fabs(a) < EPSILON) return false;  // ray is parallel to triangle

  float f = 1.f / a;
  vec3 s = ray_origin - v0_;
  float u = f * dot(s, h);
  if (u < 0.f || u > 1.f) return false;

  vec3 q = cross(s, edge1);
  float v = f * dot(ray_direction, q);
  if (v < 0.f || u + v > 1.f) return false;

  float t = f * dot(edge2, q);
  if (t > EPSILON) {  // ray intersection
    t0 = t;
    x0 = ray_origin + t * ray_direction;
    return true;
  }
  return false;

}
