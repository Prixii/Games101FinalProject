#include "PhotonMapping.h"
#include "../general/Tools.h"
#include "Intersection.h"
#include "Photon.h"
#include "PhotonMappingConfig.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <cmath>
#include <utility>
#include <vector>

std::vector<Photon> EmitPhotons(int num_photons, Model &model) {
  float x, y, z;
  glm::vec3 photon_dir;

  std::vector<Photon> photons;

  for (int i = 0; i < num_photons; i++) {
    do {
      x = GetRandomFloat();
      y = GetRandomFloat();
      z = GetRandomFloat();
    } while (x * x + y * y + z * z > 1);

    photon_dir = glm::vec3(x, y, z);

    Photon p(photon_dir, LIGHT_POS, LIGHT_POWER / (float)num_photons, 0);
    TracePhoton(p, model, photons);
  }
  return photons;
}

void TracePhoton(Photon &p, Model &model, std::vector<Photon> &photons) {
  Intersection i, j;

  glm::vec3 x0, x1;
  float t0, t1;

  auto &spheres = model.spheres_;
  auto &triangles = model.triangles_;

  if (!ClosestIntersection(p.source_, p.direction_, triangles, spheres, i)) {
    return;
  }

  if (i.IsSphere()) {
    auto standard =
        Fresnel(glm::normalize(p.direction_),
                glm::normalize(i.position_ - spheres[i.sphere_index_].center_));
    if (GetRandomFloat() < standard) {
      Reflect(spheres[i.sphere_index_], glm::normalize(p.direction_), triangles,
              spheres, i, j);
    } else {
      Refract(spheres[i.sphere_index_], p.direction_, triangles, spheres, i, j);
    }

    p.destination_ = j.position_;
    photons.push_back(p);
    std::swap(i, j);
  } else {
    p.destination_ = i.position_;
    if (p.bounces_ > 0) {
      photons.push_back(p);
    }
  }
  auto triangle = triangles[i.triangle_index_];
  if (p.bounces_ == 0 || GetRandomFloat() < 0.5) {
    Photon p2(GetRandomDirection(triangle.normal), i.position_,
              p.energy_ * triangle.color / (float)std::sqrt(p.bounces_ + 1),
              p.bounces_ + 1);
  }
}

void RayTrace(SDL_Surface *screen, SDL_Window *window,
              const std::vector<Triangle> &triangles,
              const std::vector<Sphere> &spheres) {
  if (SDL_MUSTLOCK(screen)) {
    SDL_LockSurface(screen);
  }

  auto black_color = SDL_MapRGBA(SDL_GetPixelFormatDetails(screen->format),
                                 nullptr, 0, 0, 0, 255);
  SDL_FillSurfaceRect(screen, nullptr, black_color);

  Intersection closet_intersection;

  const int total_pixels = WINDOW_WIDTH * WINDOW_HEIGHT;
  int pixels_drawn = 0;
  const int update_interval = total_pixels / 100;

  for (int y = 0; y < WINDOW_HEIGHT; ++y) {
    for (int x = 0; x < WINDOW_WIDTH; ++x) {
      ///
      glm::vec3 dir(x - WINDOW_WIDTH / 2, y - WINDOW_HEIGHT / 2, FOCAL_LENGTH);
      if (!ClosestIntersection(CAMERA_POS, dir, triangles, spheres,
                               closet_intersection)) {
        continue;
      }

      if (closet_intersection.IsSphere()) {
        PutPixel(screen, x, y, GetRadianceSpehere(closet_intersection));
      } else {
        PutPixel(screen, x, y, GetRadianceSpehere(closet_intersection));
      }
      ///

      pixels_drawn++;
      if (pixels_drawn % update_interval == 0 && pixels_drawn > 0) {
        double percentage = (double)pixels_drawn / total_pixels * 100.f;
        printf("\rProgress: %d%%", (int)percentage);
      }
    }
  }

  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }
  SDL_UpdateWindowSurface(window);
}
