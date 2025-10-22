#include "Photon.h"

bool Refract(const glm::vec3 &photon_dir, const glm::vec3 &surface_normal,
             glm::vec3 &T, const float &ior) {
  auto N = surface_normal;
  float cosi = glm::clamp(-1.f, 1.f, glm::dot(photon_dir, N));
  float n1 = 1;
  float n2 = ior;
  float n, c;

  if (cosi < 0) {
    cosi *= -1;
  } else {
    std::swap(n1, n2);
    N *= -1;
  }

  n = (n1 / n2);
  c = 1 - n * n * (1 - cosi * cosi);

  if (c < 0) {
    return false;
  }

  T = n * photon_dir + (n * cosi - sqrtf(c)) * N;

  return true;
}
bool Refract(const Sphere s, const glm::vec3 photon_dir,
             const std::vector<Triangle> &triangles,
             const std::vector<Sphere> &spheres, const Intersection &i,
             Intersection &j) {
  glm::vec3 sphere_normal = glm::normalize(i.position_ - s.center_);
  glm::vec3 ti, tj;

  if (!Refract(photon_dir, sphere_normal, ti)) {
    return false;
  }

  glm::vec3 x0, x1;
  float t0, t1;
  s.Intersect(i.position_, ti, x0, x1, t0, t1);
  sphere_normal = glm::normalize(x1 - s.center_);

  if (!Refract(ti, sphere_normal, tj)) {
    return false;
  }
  ClosestIntersection(x1, tj, triangles, spheres, j);
  return true;
}
bool Reflect(const Sphere s, const glm::vec3 photon_dir,
             const std::vector<Triangle> &triangles,
             const std::vector<Sphere> &spheres, const Intersection &i,
             Intersection &j) {
  auto sphere_normal = glm::normalize(i.position_ - s.center_);
  auto dir_reflect =
      photon_dir - 2.f * sphere_normal * glm::dot(photon_dir, sphere_normal);

  return ClosestIntersection(i.position_, dir_reflect, triangles, spheres, j);
}
float Fresnel(glm::vec3 photon_dir, glm::vec3 surface_normal,
              const float &ior) {
  float r0 = pow(1.f - ior, 2.f) / pow(1.f + ior, 2.f);
  float cosx = -glm::dot(surface_normal, photon_dir);

  return r0 + (1.f - r0) * pow(1.f - cosx, 5.f);
}
