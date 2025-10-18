#pragma once

#include "Intersection.h"
#include "Model.h"
#include "Photon.h"
#include "PhotonMappingConfig.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "SDLHelper.h"
#include "Triangle.h"
#include "glm/ext/vector_float3.hpp"
#include <cstdio>
#include <vector>

std::vector<Photon> EmitPhotons(int num_photons, Model &model);

void TracePhoton(Photon &p, Model &model, std::vector<Photon> &photons);

bool ClosestIntersection(const glm::vec3 start, const glm::vec3 dir,
                         const std::vector<Triangle> &triangles,
                         const std::vector<Sphere> &spheres,
                         Intersection &intersection){NOT_IMPLEMENTED}

glm::vec3 GetRadianceSpehere(const Intersection &i){NOT_IMPLEMENTED}

glm::vec3 GetRadianceTriangle(const Intersection &i) {
  NOT_IMPLEMENTED
}

void RayTrace(SDL_Surface *screen, SDL_Window *window,
              const std::vector<Triangle> &triangles,
              const std::vector<Sphere> &spheres);
