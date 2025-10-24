#include <SDL3/SDL.h>

#include <vector>

#include "KdTree.h"
#include "PhotonMapping.h"
#include "PhotonMappingConfig.h"
#include "PhotonUtils.h"
#include "SDL3/SDL_surface.h"
#include "SDLHelper.h"




void Test(Model &model) {
  std::vector<Photon> *photons = new std::vector<Photon>();

  auto photon_dir = vec3(0.3, 0.84, 0.02);
  Intersection i, j;
  i.position_ = vec3(0.34, 0.46, -0.37);
  bool t = Refract(model.spheres_[0], photon_dir, model.triangles_, model.spheres_, i,
          j);
  if (t && !j.IsSphere() && !j.IsTriangle()) {
    PrintWarn("Shit\n");
  }
}

int main() {

  Model model;
  model.LoadTestModel();

  //Test(model);
  //return 0;
  PrintInfo("Emitting photons\n");
  auto photons = EmitPhotons(NUM_PHOTONS, model);

  KdTree photon_map;
  PrintInfo("Building tree\n");
  photon_map.SetPhotons(photons->data(), NUM_PHOTONS);
  photon_map.BuildTree();


  SDL_Window *window = nullptr;
  auto screen = InitializeSDL(WINDOW_WIDTH, WINDOW_HEIGHT, window);

  PrintInfo("Raytracing\n");
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
