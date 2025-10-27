#include "Photon.h"

bool Refract(const vec3 &photon_dir, const vec3 &surface_normal,
             vec3 &T, const float &ior) {
  auto N = surface_normal;
  //PrintInfo("dot result: %f", dot(photon_dir, N));
  float cosi = clamp(dot(photon_dir, N) ,- 1.0f, 1.0f);
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

bool Refract(const Sphere s, const vec3 photon_dir,
             const std::vector<Triangle> &triangles,
             const std::vector<Sphere> &spheres, const Intersection &i,
             Intersection &j) {
  vec3 sphere_normal = normalize(i.position_ - s.center_);
  vec3 Ti, Tj;

  if (!Refract(photon_dir, sphere_normal, Ti)) {
    return false;
  }

  vec3 x0, x1;
  float t0, t1;
  s.Intersect(i.position_, Ti, x0, x1, t0, t1);
  sphere_normal = normalize(x1 - s.center_);

  if (!Refract(Ti, sphere_normal, Tj)) {
    return false;
  }
  ClosestIntersection(x1, Tj, triangles, spheres, j);

  
  if (!j.IsTriangle() && !j.IsSphere()) {
    int p = 1;
  }



  return true;
}


bool Reflect(const Sphere s, const vec3 photon_dir,
             const std::vector<Triangle> &triangles,
             const std::vector<Sphere> &spheres, const Intersection &i,
             Intersection &j) {
  auto sphere_normal = normalize(i.position_ - s.center_);
  auto dir_reflect =
      photon_dir - 2.f * sphere_normal * dot(photon_dir, sphere_normal);

  return ClosestIntersection(i.position_, dir_reflect, triangles, spheres, j);
}
float Fresnel(vec3 photon_dir, vec3 surface_normal,
              const float &ior) {
  float r0 = pow(1.f - ior, 2.f) / pow(1.f + ior, 2.f);
  float cosx = -dot(surface_normal, photon_dir);

  return r0 + (1.f - r0) * pow(1.f - cosx, 5.f);
}
