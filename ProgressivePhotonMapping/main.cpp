

#include <SDL3/SDL.h>

#include "ProgressivePhotonMapping.h"
#include "ProgressivePhotonMappingConfig.h"
#include "SDL3/SDL_surface.h"
#include "SDLHelper.h"

int main() {
  Model model;
  model.LoadTestModel();

  // TODO
  SDL_Window *window = nullptr;
  auto screen = InitializeSDL(WINDOW_WIDTH, WINDOW_HEIGHT, window);

  auto hit_points = RayTracing(screen, model);

  int i = 1;

  // // TODO
  // PrintInfo("Saving output.bmp\n");
  // SDL_SaveBMP(screen, "output.bmp");

  // SDL_Event event;
  // while (true) {
  //   while (SDL_PollEvent(&event)) {
  //     if (event.key.key == SDLK_ESCAPE) {
  //       SDL_DestroyWindow(window);
  //       SDL_Quit();
  //       return 0;
  //     }
  //   }
  // }
}
