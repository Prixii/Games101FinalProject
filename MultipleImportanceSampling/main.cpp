#include <SDL3/SDL.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "../general/SDLHelper.h"
#include "../general/Tools.h"
#include "./MISConfig.h"
#include "BasicMesh.h"
#include "RayTracer.h"

//
// 课程中提到我们可以对光源采样，也可以对 BSDF采样，那么两种方法怎样
// 才能结合起来？另外结合两种方法又有什么优势？你可以探索多重重要性采样的
// 方法，当渲染表面的粗糙度较低时，更多依赖于对 BSDF的采样，而当光源较小
// 时，更多依赖于对光源的采样。注意这里要求你先了解并实现对BSDF的重要性
// 采样，再考虑多重重要性采样。
//

int main() {
  PrintInfo("Project: MIS\n");


  auto s = glm::cross(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));

  Assimp::Importer importer;

  auto obj = "../assets/cube.obj";

  const aiScene *scene = importer.ReadFile(obj, ASSIMP_FLAG);
  if (scene == nullptr) {
    PrintErr("Failed to load scene");
    return -1;
  }

  PrintInfo("Loading Mesh\n");
  BasicMesh basic_mesh;
  basic_mesh.InitFromScene(*scene);
  basic_mesh.Rotate(65.f);
  basic_mesh.NormalizeVertices();


  PrintInfo("Initializing Ray Tracer\n");
  RayTracer ray_tracer{};

  PrintInfo("Ray Tracing\n");
  auto pixels = ray_tracer.RayTracing(basic_mesh);

  SDL_Window *window = nullptr;
  auto screen = InitializeSDL(WINDOW_WIDTH, WINDOW_HEIGHT, window);


  PrintInfo("max_z: %f, min_z: %f\n", ray_tracer.max_z, ray_tracer.min_z);

  PrintInfo("Displaying Image\n");
  PutPixelPatch(screen, WINDOW_WIDTH, WINDOW_HEIGHT, pixels, window);

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
