#include "PhotonMapping.h"

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
      Reflect(spheres[i.sphere_index_], normalize(p.direction_),
                       triangles, spheres, i, j);
    } else {
      Refract(spheres[i.sphere_index_], p.direction_, triangles,
                       spheres, i, j);
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
  // 1. Surface 锁定 (已是线程安全措施)
  if (SDL_MUSTLOCK(screen)) {
    SDL_LockSurface(screen);
  }

  // 填充黑色背景 (在锁内执行)
  auto pixel_format = SDL_GetPixelFormatDetails(screen->format);

  auto black_color = SDL_MapRGBA(SDL_GetPixelFormatDetails(screen->format),
                                 nullptr, 0, 0, 0, 255);
  SDL_FillSurfaceRect(screen, nullptr, black_color);

  // 进度跟踪变量
  const int total_pixels = WINDOW_WIDTH * WINDOW_HEIGHT;
  // total_pixels 可以在 OpenMP reduction 中使用，但 update_interval
  // 最好在外部计算 进度更新可以简化，只在主线程中打印

  // ⚠️ pixels_drawn 必须在 OpenMP 区域外定义，并在循环中累加
  int pixels_drawn = 0;

#pragma omp parallel num_threads(6)
  {
    // 2. 线程私有变量
    // Intersection 必须是每个线程私有的，以避免数据竞争
    Intersection closet_intersection;

    // OpenMP for 循环：并行处理像素，并使用 reduction 安全累加 pixels_drawn
#pragma omp for reduction(+ : pixels_drawn) schedule(dynamic)
    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
      for (int x = 0; x < WINDOW_WIDTH; ++x) {
        // 3. 核心光线追踪和光照计算
        vec3 dir(x - WINDOW_WIDTH / 2, y - WINDOW_HEIGHT / 2, FOCAL_LENGTH);

        // ⚠️ closet_intersection 现在是局部变量，线程安全
        if (!ClosestIntersection(CAMERA_POS, dir, triangles, spheres,
                                 closet_intersection)) {
          // 如果没有交点，跳过当前像素
          continue;
        }

        // ⚠️ 原始代码中的 if 语句逻辑看起来是调试代码，可以移除或保持
        // if (!closet_intersection.IsSphere() &&
        // !closet_intersection.IsTriangle()) { int t = 1; }

        vec3 color;

        if (closet_intersection.IsSphere()) {
          color = GetRadianceSphere(closet_intersection, photon_map, photons,
                                    triangles, spheres);
        } else {  // 假设 IsTriangle() 为真，如果不是，这里会出错
          color = GetRadianceTriangle(closet_intersection, photon_map, photons,
                                      triangles, spheres);
        }

        // PutPixel (SDL_Surface 已锁定，写入是安全的)
        PutPixel(screen, x, y, color);

        // 4. 进度累加
        // reduction(+:pixels_drawn)
        // 会在循环结束后安全地将线程内的累加值加到主变量
        pixels_drawn++;
      }
    }

    // 5. 进度报告
    // 只有主线程进行进度报告，避免竞争
#pragma omp master
    {
      // 简化进度打印，只在循环结束后打印一次最终状态
      printf("\rProgress: 100%%\n");
    }
  }  // 结束 OpenMP 并行区域

  // 6. Surface 解锁与更新 (在并行区域外，已是线程安全)
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
