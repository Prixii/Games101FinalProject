#pragma once

#include <SDL3/SDL.h>

#include <glm/glm.hpp>

using namespace glm;


SDL_Surface* InitializeSDL(int width, int height,
                                  SDL_Window*& out_window);

void PutPixel(SDL_Surface* surface, int x, int y, const glm::vec3& color);
