#pragma once

#include "HitPoint.h"
#include "Intersection.h"
#include "KdTree.h"
#include "Model.h"
#include "Photon.h"
#include "ProgressivePhotonMappingConfig.h"
#include "ProgressivePhotonUtils.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "SDLHelper.h"
#include "Sphere.h"
#include "Triangle.h"
#include "glm/exponential.hpp"
#include "glm/geometric.hpp"

#include "../general/Tools.h"

#include <algorithm>
#include <cstdio>
#include <glm/glm.hpp>
#include <numeric>
#include <vector>

using namespace glm;

std::vector<Photon> *EmitPhotons(int num_photons, Model &model);

void TracePhoton(Photon &p, Model &model, std::vector<Photon> &photons);

vec3 GetRadianceSphere(const Intersection &i, const KdTree &photon_map,
                       const std::vector<Photon> &photons,
                       const std::vector<Triangle> &triangles,
                       const std::vector<Sphere> &spheres);

vec3 GetRadianceTriangle(const Intersection &i, const KdTree &photon_map,
                         const std::vector<Photon> &photons,
                         const std::vector<Triangle> &triangles,
                         const std::vector<Sphere> &spheres);

void RayTrace(SDL_Surface *screen, SDL_Window *window, const KdTree &photon_map,
              const std::vector<Photon> &photons,
              const std::vector<Triangle> &triangles,
              const std::vector<Sphere> &spheres);

void SinglePassRayTracing(Model &model, vec3 ray_origin, vec3 ray_dir,
                          vec3 color, int bounce, double dist, vec2 pos,
                          std::vector<vec3> &pixels,
                          std::vector<HitPoint> &hit_points);

std::vector<HitPoint> RayTracing(SDL_Surface *screen, Model &model);

std::vector<HitPoint> TestRayTracing(SDL_Surface *screen, Model &model);
