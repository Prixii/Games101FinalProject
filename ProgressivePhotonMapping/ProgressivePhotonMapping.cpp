#include "ProgressivePhotonMapping.h"

#include <cmath>
#include <utility>
#include <vector>

int closet_not_found_time = 0;
int tracep2_time = 0;

std::vector<Photon> *EmitPhotons(int num_photons, Model &model) {
  float x, y, z;
  vec3 photon_dir;

  std::vector<Photon> *photons = new std::vector<Photon>();

  for (int i = 0; i < num_photons; i++) {
    do {
      x = GetRandomFloat();
      y = GetRandomFloat();
      z = GetRandomFloat();
    } while (x * x + y * y + z * z > 1);

    photon_dir = vec3(x, y, z);

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

  vec3 x0, x1;
  float t0, t1;

  auto &spheres = model.spheres_;
  auto &triangles = model.triangles_;

  if (!ClosestIntersection(p.source_, p.direction_, triangles, spheres, i)) {
    return;
  }

  if (i.IsSphere()) {
    auto standard =
        Fresnel(normalize(p.direction_),
                normalize(i.position_ - spheres[i.sphere_index_].center_));
    if (GetRandomFloat() < standard) {
      Reflect(spheres[i.sphere_index_], normalize(p.direction_), triangles,
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
  if (p.bounces_ == 0 || GetRandomFloat() < 0.5) {
    auto &triangle = triangles[i.triangle_index_];
    Photon p2(GetRandomDirection(triangle.normal_), i.position_,
              p.energy_ * triangle.color_ / (float)std::sqrt(p.bounces_ + 1),
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
      vec3 dir(x - WINDOW_WIDTH / 2, y - WINDOW_HEIGHT / 2, FOCAL_LENGTH);
      if (!ClosestIntersection(CAMERA_POS, dir, triangles, spheres,
                               closet_intersection)) {
        continue;
      }
      if (!closet_intersection.IsSphere() &&
          !closet_intersection.IsTriangle()) {
        int t = 1;
      }

      vec3 color;

      if (closet_intersection.IsSphere()) {
        color = GetRadianceSphere(closet_intersection, photon_map, photons,
                                  triangles, spheres);
      } else {
        color = GetRadianceTriangle(closet_intersection, photon_map, photons,
                                    triangles, spheres);
      }
      PutPixel(screen, x, y, color);

      pixels_drawn++;
      if (pixels_drawn % update_interval == 0 && pixels_drawn > 0) {
        double percentage = (double)pixels_drawn / total_pixels * 100.f;
        printf("\rProgress: %d%%", (int)percentage);
      }
    }
  }
  printf("\n");

  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }
  SDL_UpdateWindowSurface(window);
}
vec3 GetRadianceSphere(const Intersection &i, const KdTree &photon_map,
                       const std::vector<Photon> &photons,
                       const std::vector<Triangle> &triangles,
                       const std::vector<Sphere> &spheres) {
  Intersection intersection_refract, intersection_reflect;

  Refract(spheres[i.sphere_index_], normalize(i.position_ - CAMERA_POS),
          triangles, spheres, i, intersection_refract);

  float f = Fresnel(normalize(i.position_ - CAMERA_POS),
                    normalize(i.position_ - spheres[i.sphere_index_].center_));

  float c = std::max(1.f, 1.7f * f);

  Reflect(spheres[i.sphere_index_], normalize(i.position_ - CAMERA_POS),
          triangles, spheres, i, intersection_reflect);

  auto refract_color = GetRadianceTriangle(intersection_refract, photon_map,
                                           photons, triangles, spheres);
  auto reflect_color = GetRadianceTriangle(intersection_reflect, photon_map,
                                           photons, triangles, spheres);
  return (1 - c) * refract_color + c * reflect_color;
}

vec3 GetRadianceTriangle(const Intersection &i, const KdTree &photon_map,
                         const std::vector<Photon> &photons,
                         const std::vector<Triangle> &triangles,
                         const std::vector<Sphere> &spheres) {
  vec3 color(0, 0, 0);
  vec3 delta_phi;
  float wpc;
  float dp;
  float r_sqr = 0.f;

  std::vector<NeighborPhoton> neighbors =
      photon_map.SearchKNearest(i.position_, K_NEAREST, r_sqr);
  for (size_t p = 0; p < neighbors.size(); p++) {
    dp = neighbors[p].dist_;
    wpc = 1 - dp / (CONE_FILTER_CONST * sqrt(r_sqr));

    float effect = dot(-photons[neighbors[p].index_].direction_,
                       triangles[i.triangle_index_].normal_);
    delta_phi = std::max(effect, 0.f) * photons[neighbors[p].index_].energy_;

    color += wpc * delta_phi;
  }
  auto cone_size = (1 - 2 / (3 * CONE_FILTER_CONST)) * PI * r_sqr;
  color /= (cone_size * 10);
  color += DirectLight(i, triangles, spheres);
  color *= triangles[i.triangle_index_].color_;

  return color;
}
void SinglePassRayTracing(Model &model, glm::vec3 ray_origin, glm::vec3 ray_dir,
                          glm::vec3 color, int bounce, double dist,
                          glm::vec2 pos, std::vector<glm::vec3> pixels,
                          std::vector<HitPoint> hit_points) {

  if (bounce > BOUNCE_LIMIT) {
    return;
  }

  Intersection i, j;
  if (!ClosestIntersection(CAMERA_POS, ray_dir, model.triangles_,
                           model.spheres_, i)) {
    pixels[pos.x * WINDOW_WIDTH + pos.y] += PURE_BLACK;
    return;
  }

  if (i.IsSphere()) {
    auto &sphere = model.spheres_[i.sphere_index_];
    Refract(sphere, normalize(ray_dir), model.triangles_, model.spheres_, i, j);
    std::swap(i, j);
  }
  auto &triangle = model.triangles_[i.triangle_index_];
  auto new_color = color * triangle.color_ / (float)std::sqrt(bounce + 1);

  hit_points.push_back(
      HitPoint{i.position_, triangle.normal_, new_color, pos, 1.f});

  auto new_ray_dir = GetRandomDirection(triangle.normal_);
  SinglePassRayTracing(model, i.position_, new_ray_dir, new_color, bounce + 1,
                       dist + i.distance_, pos, pixels, hit_points);
}
std::vector<HitPoint> RayTracing(SDL_Surface *screen, Model &model) {
  std::vector<int> samples_count(WINDOW_WIDTH * WINDOW_HEIGHT);
  std::iota(samples_count.begin(), samples_count.end(), 0);

  std::vector<glm::vec3> pixels(WINDOW_WIDTH * WINDOW_HEIGHT);
  std::iota(pixels.begin(), pixels.end(), PURE_BLACK);

  auto hit_points = new std::vector<HitPoint>(WINDOW_WIDTH * WINDOW_HEIGHT);

  for (int w = 0; w < WINDOW_WIDTH; w++) {
    for (int h = 0; h < WINDOW_HEIGHT; h++) {
      glm::vec2 target_pixel(w + GetRandomFloat(0.f, 0.5f),
                             h + GetRandomFloat(0.f, 0.5f));
      glm::vec3 ray_origin = CAMERA_POS;
      glm::vec3 ray_dir(target_pixel.x - (float)WINDOW_WIDTH / 2,
                        target_pixel.y - (float)WINDOW_HEIGHT / 2,
                        FOCAL_LENGTH);
      SinglePassRayTracing(model, ray_origin, ray_dir, WHITE, 0, 0,
                           target_pixel, pixels, *hit_points);
    }
  }

  return *hit_points;
}
