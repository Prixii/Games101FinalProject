#include <SDL3/SDL.h>
#include <vector>

#include "KdTree.h"
#include "PhotonMapping.h"
#include "PhotonMappingConfig.h"
#include "PhotonUtils.h"
#include "SDL3/SDL_surface.h"
#include "SDLHelper.h"

/// photon mapping
/// KD tree

int main() {
  Model model;
  model.LoadTestModel();


  auto photons = EmitPhotons(NUM_PHOTONS, model);

  KdTree photon_map;
  photon_map.SetPhotons(photons->data(), NUM_PHOTONS);
  photon_map.BuildTree();

  SDL_Window *window = nullptr;
  auto screen = InitializeSDL(WINDOW_WIDTH, WINDOW_HEIGHT, window);

  RayTrace(screen, window, photon_map, *photons, model.triangles_,
           model.spheres_);

  SDL_SaveBMP(screen, "output.bmp");

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
