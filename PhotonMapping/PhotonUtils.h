#pragma once

#include <limits>
#include <glm/glm.hpp>
#include <vector>

#include "../general/Tools.h"
#include "Intersection.h"
#include "PhotonMappingConfig.h"
#include "Sphere.h"
#include "Triangle.h"

bool IsVisible(glm::vec3 v1, glm::vec3 v2);

bool ClosestIntersection(const glm::vec3 start, const glm::vec3 dir,
                         const std::vector<Triangle> &triangles,
                         const std::vector<Sphere> &spheres,
                         Intersection &intersection);

glm::vec3 DirectLight(const Intersection &i,
                      const std::vector<Triangle> &triangles,
                      const std::vector<Sphere> &spheres);
