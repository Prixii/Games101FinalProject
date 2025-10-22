#pragma once

#include "Intersection.h"
#include "KdTree.h"
#include "Model.h"
#include "Photon.h"
#include "PhotonMappingConfig.h"
#include "PhotonUtils.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "SDLHelper.h"
#include "Sphere.h"
#include "Triangle.h"
#include "glm/exponential.hpp"
#include "glm/geometric.hpp"

#include <algorithm>
#include <cstdio>
#include <glm/glm.hpp>
#include <vector>

std::vector<Photon> *EmitPhotons(int num_photons, Model &model);

void TracePhoton(Photon &p, Model &model, std::vector<Photon> &photons);

glm::vec3 GetRadianceSphere(const Intersection &i, const KdTree &photon_map,
                            const std::vector<Photon> &photons,
                            const std::vector<Triangle> &triangles,
                            const std::vector<Sphere> &spheres);

glm::vec3 GetRadianceTriangle(const Intersection &i, const KdTree &photon_map,
                              const std::vector<Photon> &photons,
                              const std::vector<Triangle> &triangles,
                              const std::vector<Sphere> &spheres);

void RayTrace(SDL_Surface *screen, SDL_Window *window, const KdTree &photon_map,
              const std::vector<Photon> &photons,
              const std::vector<Triangle> &triangles,
              const std::vector<Sphere> &spheres);
