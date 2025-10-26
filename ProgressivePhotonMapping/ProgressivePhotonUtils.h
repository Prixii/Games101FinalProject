#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <vector>

#include "../general/Tools.h"
#include "Intersection.h"
#include "ProgressivePhotonMappingConfig.h"
#include "Sphere.h"
#include "Triangle.h"

using namespace glm;
extern int invisible_time;

bool IsVisible(vec3 v1, vec3 v2);

bool ClosestIntersection(const vec3 start, const vec3 dir,
                         const std::vector<Triangle> &triangles,
                         const std::vector<Sphere> &spheres,
                         Intersection &intersection);

vec3 DirectLight(const Intersection &i,
                      const std::vector<Triangle> &triangles,
                      const std::vector<Sphere> &spheres);
