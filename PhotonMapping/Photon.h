#pragma once
#include <vector>

#include "../general/Tools.h"
#include "Intersection.h"
#include "Sphere.h"
#include "Triangle.h"

struct NeighborPhoton {
  int index_;
  float dist_;
};

struct Photon {
  glm::vec3 direction_;
  glm::vec3 source_;
  glm::vec3 destination_;
  glm::vec3 energy_;
  int bounces_;

  Photon(glm::vec3 direction, glm::vec3 source, glm::vec3 energy,
         int bounces = 0)
      : direction_(direction), source_(source), energy_(energy),
        bounces_(bounces) {}
};

bool Refract(const glm::vec3 &photon_dir, const glm::vec3 &surface_normal,
             glm::vec3 &T, const float &ior = 1.0f) {
  NOT_IMPLEMENTED
}

bool Refract(const Sphere s, const glm::vec3 photon_dir,
             const std::vector<Triangle> &triangles,
             const std::vector<Sphere> &spheres, const Intersection &i,
             Intersection &j) {
  NOT_IMPLEMENTED
}

bool Reflect(const Sphere s, const glm::vec3 photon_dir,
             const std::vector<Triangle> &triangles,
             const std::vector<Sphere> &spheres, const Intersection &i,
             Intersection &j) {
  NOT_IMPLEMENTED
}

float Fresnel(glm::vec3 photon_dir, glm::vec3 surface_normal,
              const float &ior = 1.75) {
  NOT_IMPLEMENTED
}
