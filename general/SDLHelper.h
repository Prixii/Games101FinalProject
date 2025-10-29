#pragma once

#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <vector>

using namespace glm;

SDL_Surface *InitializeSDL(int width, int height, SDL_Window *&out_window);

void PutPixel(SDL_Surface *surface, int x, int y, const glm::vec3 &color);

void PutPixelPatch(SDL_Surface *surface, int width, int height,
                   const std::vector<glm::vec3> &pixels, SDL_Window *window);
