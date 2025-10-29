#include "SDLHelper.h"

#include "../general/Tools.h"
#include "SDL3/SDL_video.h"
#include <vector>

SDL_Surface *InitializeSDL(int width, int height, SDL_Window *&out_window) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    PrintErr("Could not initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  atexit(SDL_Quit);

  Uint32 window_flags = 0;

  auto window = SDL_CreateWindow("Photon Mapping", width, height, window_flags);
  if (!window) {
    PrintErr("Could not create window: %s\n", SDL_GetError());
    exit(1);
  }
  out_window = window;

  auto surface = SDL_GetWindowSurface(window);
  if (!surface) {
    PrintErr("Could not get window surface: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    exit(1);
  }

  return surface;
}

void PutPixel(SDL_Surface *surface, int x, int y, const vec3 &color) {
  if (!surface) {
    PrintErr("Surface is null\n");
    return;
  }

  if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
    PrintErr("Illegal point: %d, %d", x, y);
    return;
  }

  Uint8 r = static_cast<Uint8>(clamp(color.r * 255.f, 0.f, 255.f));
  Uint8 g = static_cast<Uint8>(clamp(color.g * 255.f, 0.f, 255.f));
  Uint8 b = static_cast<Uint8>(clamp(color.b * 255.f, 0.f, 255.f));

  Uint8 a = 255u;

  SDL_WriteSurfacePixel(surface, x, y, r, g, b, a);
}

void PutPixelPatch(SDL_Surface *surface, int width, int height,
                   const std::vector<glm::vec3> &pixels, SDL_Window *window) {
  if (SDL_MUSTLOCK(surface)) {
    SDL_LockSurface(surface);
  }

  // 填充黑色背景 (在锁内执行)
  auto pixel_format = SDL_GetPixelFormatDetails(surface->format);

  auto black_color = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format),
                                 nullptr, 0, 0, 0, 255);
  SDL_FillSurfaceRect(surface, nullptr, black_color);

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      PutPixel(surface, x, y, pixels[GetIndex(x, y, width, height)]);
    }
  }

  if (SDL_MUSTLOCK(surface)) {
    SDL_UnlockSurface(surface);
  }
  SDL_UpdateWindowSurface(window);
}
