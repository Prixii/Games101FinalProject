#include "SDLHelper.h"

#include "../general/Tools.h"

SDL_Surface* InitializeSDL(int width, int height,
                                  SDL_Window*& out_window) {
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

void PutPixel(SDL_Surface* surface, int x, int y, const vec3& color) {}
