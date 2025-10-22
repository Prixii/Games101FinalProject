#include "PhotonMapping.h"

#include <cmath>
#include <utility>
#include <vector>

int closet_not_found_time = 0;
int tracep2_time = 0;

std::vector<Photon> *EmitPhotons(int num_photons, Model &model) {
  float x, y, z;
  glm::vec3 photon_dir;

  std::vector<Photon> *photons = new std::vector<Photon>();

  for (int i = 0; i < num_photons; i++) {
    do {
      x = GetRandomFloat();
      y = GetRandomFloat();
      z = GetRandomFloat();
    } while (x * x + y * y + z * z > 1);

    photon_dir = glm::vec3(x, y, z);

    Photon p(photon_dir, LIGHT_POS, LIGHT_POWER / (float)num_photons, 0);
    TracePhoton(p, model, *photons);
  }

  PrintInfo("invisible_time:%d\n", invisible_time);
  PrintInfo("tracep2_time: %d\n", tracep2_time);
  PrintInfo("closet_not_found:%d\n", closet_not_found_time);

  return photons;
}

void TracePhoton(Photon &p, Model &model, std::vector<Photon> &photons) {
  Intersection i, j;

  glm::vec3 x0, x1;
  float t0, t1;

  auto &spheres = model.spheres_;
  auto &triangles = model.triangles_;

  if (!ClosestIntersection(p.source_, p.direction_, triangles, spheres, i)) {
    ////////
    closet_not_found_time++;
    return;
  }

  if (i.IsSphere()) {
    auto standard =
        Fresnel(glm::normalize(p.direction_),
                glm::normalize(i.position_ - spheres[i.sphere_index_].center_));
    if (GetRandomFloat() < standard) {
      bool t = Reflect(spheres[i.sphere_index_], glm::normalize(p.direction_),
                       triangles, spheres, i, j);
      if (t && !j.IsSphere() && !j.IsTriangle()) {
        PrintWarn("Nooooooo\n");
      }
    } else {
      // BUG Severe
      bool t = Refract(spheres[i.sphere_index_], p.direction_, triangles,
                       spheres, i, j);
      if (t && !j.IsSphere() && !j.IsTriangle()) {
        PrintWarn("Shit\n");
      }
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
  if (p.bounces_ == 0 || GetRandomFloat() < 0.5) {
    auto triangle = triangles[i.triangle_index_];
    Photon p2(GetRandomDirection(triangle.normal), i.position_,
              p.energy_ * triangle.color / (float)std::sqrt(p.bounces_ + 1),
              p.bounces_ + 1);
    ////////
    tracep2_time++;
    TracePhoton(p2, model, photons);
  }
}

void RayTrace(SDL_Surface *screen, SDL_Window *window, const KdTree &photon_map,
              const std::vector<Photon> &photons,
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
        PutPixel(screen, x, y,
                 GetRadianceSphere(closet_intersection, photon_map, photons,
                                   triangles, spheres));
      } else {
        PutPixel(screen, x, y,
                 GetRadianceSphere(closet_intersection, photon_map, photons,
                                   triangles, spheres));
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
glm::vec3 GetRadianceSphere(const Intersection &i, const KdTree &photon_map,
                            const std::vector<Photon> &photons,
                            const std::vector<Triangle> &triangles,
                            const std::vector<Sphere> &spheres) {
  Intersection intersection_refract, intersection_reflect;

  Refract(spheres[i.sphere_index_], glm::normalize(i.position_ - CAMERA_POS),
          triangles, spheres, i, intersection_refract);

  float f =
      Fresnel(glm::normalize(i.position_ - CAMERA_POS),
              glm::normalize(i.position_ - spheres[i.sphere_index_].center_));

  float c = std::max(1.f, 1.7f * f);

  Reflect(spheres[i.sphere_index_], glm::normalize(i.position_ - CAMERA_POS),
          triangles, spheres, i, intersection_reflect);

  return (1 - c) * GetRadianceTriangle(intersection_refract, photon_map,
                                       photons, triangles, spheres) +
         c * GetRadianceTriangle(intersection_reflect, photon_map, photons,
                                 triangles, spheres);
}
glm::vec3 GetRadianceTriangle(const Intersection &i, const KdTree &photon_map,
                              const std::vector<Photon> &photons,
                              const std::vector<Triangle> &triangles,
                              const std::vector<Sphere> &spheres) {
  glm::vec3 color(0, 0, 0);
  glm::vec3 delta_phi;
  float wpc;
  float dp;
  float r_sqr = 0.f;

  std::vector<NeighborPhoton> neighbors =
      photon_map.SearchKNearest(i.position_, K_NEAREST, r_sqr);
  for (size_t p = 0; p < neighbors.size(); p++) {
    dp = neighbors[p].dist_;
    wpc = 1 - dp / (CONE_FILTER_CONST * glm::sqrt(r_sqr));

    float effect = glm::dot(-photons[neighbors[p].index_].direction_,
                            triangles[i.triangle_index_].normal);
    delta_phi = std::max(effect, 0.f) * photons[neighbors[p].index_].energy_;

    color += delta_phi * wpc;
  }

  color /= (1 - 2 / (3 * CONE_FILTER_CONST) * PI * r_sqr);
  color += DirectLight(i, triangles, spheres);

  return color;
}
