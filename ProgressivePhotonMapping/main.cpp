

#include <SDL3/SDL.h>

#include <cmath>
#include <omp.h>
#include <vector>
#include <memory>
#include <string>

#include "HitPoint.h"
#include "KdTree.h"
#include "ProgressivePhotonMapping.h"
#include "ProgressivePhotonMappingConfig.h"
#include "SDL3/SDL_surface.h"
#include "SDLHelper.h"
#include "glm/ext/vector_float3.hpp"


int main() {
  Model model;
  model.LoadTestModel();

  SDL_Window *window = nullptr;
  auto screen = InitializeSDL(WINDOW_WIDTH, WINDOW_HEIGHT, window);

  if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
  auto black_color = SDL_MapRGBA(SDL_GetPixelFormatDetails(screen->format),
                                 nullptr, 0, 0, 0, 255);
  SDL_FillSurfaceRect(screen, nullptr, black_color);
  if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  SDL_UpdateWindowSurface(window);

  PrintInfo("Start RayTracing...\n");
  auto hit_points = RayTracing(screen, model);

  PrintInfo("KD Tree Building...\n");
  KdTree hit_points_kdtree;
  hit_points_kdtree.SetHitPoints(hit_points.data(), hit_points.size());
  hit_points_kdtree.BuildTree();

  std::vector<glm::vec3> pixels(WINDOW_WIDTH * WINDOW_HEIGHT,
                                glm::vec3(0, 0, 0));

  double current_radius = INITIAL_RADIUS;
  double current_energy = 1.f / log(TOTAL_ROUND);

  // 推荐设置线程数：使用环境变量或根据硬件自动选择
  int nthreads =
      omp_get_max_threads();  // 或者 omp_set_num_threads(x) 或手动设8
  PrintInfo("OpenMP max threads = %d\n", nthreads);

  for (int i = 0; i < TOTAL_ROUND; i++) {
    PrintInfo("Starting round %d with radius %f\n", i, current_radius);
    std::vector<glm::vec3> current_round_pixels(WINDOW_WIDTH * WINDOW_HEIGHT,
                                                glm::vec3(0, 0, 0));

    // 发射 photons（假设返回 std::vector<Photon>*）
    auto photons = EmitPhotons(500'000, model);
    auto &photon_vec = *photons;
    size_t nphotons = photon_vec.size();

    // 每个线程的私有像素缓冲（dense）
    std::vector<std::vector<glm::vec3>> thread_pixels(
        nthreads,
        std::vector<glm::vec3>(WINDOW_WIDTH * WINDOW_HEIGHT, glm::vec3(0)));

    // 并行处理 photons
    const int chunk = 1024;  // 调整为合适的 chunk 大小
#pragma omp parallel for schedule(dynamic, chunk) num_threads(nthreads)
    for (int pi = 0; pi < nphotons; ++pi) {
      int tid = omp_get_thread_num();
      const auto &photon = photon_vec[pi];

      // 注意：SearchNearest 必须是线程安全的（没有内部共享可变状态）
      auto effected_hit_points =
          hit_points_kdtree.SearchNearest(photon.destination_, current_radius);

      for (auto &neighbor : effected_hit_points) {
        auto &hit_point = hit_points[neighbor.index_];

        if (!IsVisible(hit_point.direction_, photon.direction_)) continue;

        // 计算颜色（你原来的计算）
        // delta/t 变量保留（若需）
        // float delta = ((float)current_radius - (hit_point.position_ -
        // photon.destination_).length()); float t = pow(delta / current_radius,
        // 2);

        glm::vec3 color =
            photon.energy_ * hit_point.color_ * (float)hit_point.weight_;

        int idx = GetIndex(hit_point.screen_pos_.x, hit_point.screen_pos_.y,
                           WINDOW_WIDTH, WINDOW_HEIGHT);
        thread_pixels[tid][idx] += color;
      }
    }  // end parallel for

    // 合并线程缓冲到 current_round_pixels（单线程合并）
    for (int t = 0; t < nthreads; ++t) {
      auto &tp = thread_pixels[t];
      for (size_t p = 0; p < tp.size(); ++p) {
        current_round_pixels[p] += tp[p];
      }
    }

    // 上传到屏幕并累加到总 pixels
    PrintInfo("\tStart Upload Screen\n");
    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);

    for (int x = 0; x < WINDOW_WIDTH; x++) {
      for (int y = 0; y < WINDOW_HEIGHT; y++) {
        auto idx = GetIndex(x, y, WINDOW_WIDTH, WINDOW_HEIGHT);
        pixels[idx] += current_round_pixels[idx] / 6.f;
        PutPixel(screen, x, y, pixels[idx]);
      }
    }

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_UpdateWindowSurface(window);

    auto filename = "output_" + std::to_string(i) + ".bmp";
    PrintInfo("\tSaving %s\n", filename.c_str());
    SDL_SaveBMP(screen, filename.c_str());

    delete photons;
    current_radius *= RADIUS_DECAY;
    current_energy /= ENERGY_DECAY;
  }

  hit_points.clear();

  SDL_Event event;
  while (true) {
    while (SDL_PollEvent(&event)) {
      if (event.key.key == SDLK_ESCAPE) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
      }
    }
  }
}
