#include "PhotonMapping.h"
#include "../general/Tools.h"
#include "Photon.h"
#include "PhotonMappingConfig.h"

void EmitPhotons(int num_photons) {
  float x, y, z;
  glm::vec3 photon_dir;
  for (int i = 0; i < num_photons; i++) {
    do {
      x = GetRandomFloat();
      y = GetRandomFloat();
      z = GetRandomFloat();
    } while (x * x + y * y + z * z > 1);

    photon_dir = glm::vec3(x, y, z);

    Photon p(photon_dir, LIGHT_POS, LIGHT_POWER / (float)num_photons, 0);
  }
}
void TracePhoton(Photon &p) { NOT_IMPLEMENTED; }
void RayTrace(SDL_Surface *screen, SDL_Window *window,
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
      ///
      glm::vec3 dir(x - WINDOW_WIDTH / 2, y - WINDOW_HEIGHT / 2, FOCAL_LENGTH);
      if (!ClosestIntersection(CAMERA_POS, dir, triangles, spheres,
                               closet_intersection)) {
        continue;
      }

      if (closet_intersection.IsSphere()) {
        PutPixel(screen, x, y, GetRadianceSpehere(closet_intersection));
      } else {
        PutPixel(screen, x, y, GetRadianceSpehere(closet_intersection));
      }
      ///

      pixels_drawn++;
      if (pixels_drawn % update_interval == 0 && pixels_drawn > 0) {
        double percentage = (double)pixels_drawn / total_pixels * 100.f;
        printf("\rProgress: %d%%", (int)percentage);
      }
    }
  }

  if (SDL_MUSTLOCK(screen)) {
    SDL_UnlockSurface(screen);
  }
  SDL_UpdateWindowSurface(window);
}
