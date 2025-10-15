#include <SDL3/SDL.h>

#include "SDLHelper.h"
#include "Model.h"

constexpr auto WINDOW_WIDTH = 800;
constexpr auto WINDOW_HEIGHT = 800;

SDL_Window *window = nullptr;

int main() {

  Model model;
  model.LoadTestModel();








  
  InitializeSDL(WINDOW_WIDTH, WINDOW_HEIGHT, window);

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