#include "Sphere.h"

inline Sphere::Sphere(const glm::vec3& c, float r) : center(c), radius(r) {}

inline Sphere::Sphere() : center(glm::vec3(0.0f)), radius(1.0f) {}
