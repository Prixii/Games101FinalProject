#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <utility>
#include <vector>

#include "../general/Tools.h"
#include "Intersection.h"
#include "ProgressivePhotonUtils.h"
#include "Sphere.h"
#include "Triangle.h"

using namespace glm;

struct NeighborPhoton {
  int index_;
  float dist_;
};

struct Photon {
  vec3 direction_;
  vec3 source_;
  vec3 destination_;
  vec3 energy_;
  int bounces_;

  Photon(vec3 direction, vec3 source, vec3 energy,
         int bounces = 0)
      : direction_(direction), source_(source), energy_(energy),
        bounces_(bounces), destination_(vec3(0, 0, 0)) {}
};

bool Refract(const vec3 &photon_dir, const vec3 &surface_normal,
             vec3 &T, const float &ior = 1.75f);

bool Refract(const Sphere s, const vec3 photon_dir,
             const std::vector<Triangle> &triangles,
             const std::vector<Sphere> &spheres, const Intersection &i,
             Intersection &j);

bool Reflect(const Sphere s, const vec3 photon_dir,
             const std::vector<Triangle> &triangles,
             const std::vector<Sphere> &spheres, const Intersection &i,
             Intersection &j);

float Fresnel(vec3 photon_dir, vec3 surface_normal,
              const float &ior = 1.75);
