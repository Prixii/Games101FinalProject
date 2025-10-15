#include <SDL3/SDL.h>

#include "SDLHelper.h"

constexpr auto WINDOW_WIDTH = 800;
constexpr auto WINDOW_HEIGHT = 800;

SDL_Window *window = nullptr;

int main() {
  InitializeSDL(WINDOW_WIDTH, WINDOW_HEIGHT, window);

  return 0;
}