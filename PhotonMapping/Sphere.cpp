#include "Sphere.h"

Sphere::Sphere(const glm::vec3 &c, float r) : center_(c), radius_(r) {}

Sphere::Sphere() : center_(glm::vec3(0.0f)), radius_(1.0f) {}
bool Sphere::Intersect(const glm::vec3 &start, const glm::vec3 &dir,
                       glm::vec3 &x0, glm::vec3 &x1, float &t0,
                       float &t1) const {
  auto center_trn = start - this->center_;
  float A = glm::dot(dir, dir);
  float B = 2.0 * glm::dot(dir, center_trn);
  float C = glm::dot(center_trn, center_trn) - pow(radius_, 2.0);
  auto delta = pow(B, 2.0) - 4.0 * A * C;

  if (delta == 0.f) {
    t0 = t1 = -B / (2.0 * A);
    x0 = start + t0 * dir;
    x1 = start + t1 * dir;
    return true;
  } else if (delta > 0.f) {
    t0 = (-B + sqrt(delta)) / (2.0 * A);
    t1 = (-B - sqrt(delta)) / (2.0 * A);

    if (t0 < 0 && t1 < 0) {
      return false;
    }
    if (t0 > t1) {
      std::swap(t0, t1);
    }

    x0 = start + t0 * dir;
    x1 = start + t1 * dir;

    return true;
  } else {
    return false;
  }
}
