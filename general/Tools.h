#pragma once

#include "glm/ext/vector_float3.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <glm/glm.hpp>
#include <string>

#define NOT_IMPLEMENTED                                                        \
  PrintErr("Not implemented func in %s:%d\n", __FILE__, __LINE__);             \
  exit(0);
constexpr float PI = 3.14159265358979323846;
constexpr float INV_PI = 1.0f / PI;

bool StringStartsWith(const std::string &str, const std::string &prefix);
void PrintWarn(const char *fmt, ...);
void PrintErr(const char *fmt, ...);
void PrintInfo(const char *fmt, ...);
void PrintSuccess(const char *fmt, ...);

int GetRandomInt(int min = 0, int max = 1);
float GetRandomFloat(float min = 0.0f, float max = 1.0f);
glm::vec3 GetRandomDirection(const glm::vec3 &normal);
glm::vec3 GetRandomPosInTriangle(const glm::vec3 &v1, const glm::vec3 &v2,
                                 const glm::vec3 &v3);
float GetAreaOfTriangle(const glm::vec3 &v1, const glm::vec3 &v2,
                        const glm::vec3 &v3);

float GetSqrDist(const glm::vec3 a, const glm::vec3 b);

int GetIndex(float x, float y, int width, int height, bool reverse_y = true);

bool RussianRoulette(float probability);
bool IsVisible(glm::vec3 &v1, glm::vec3 &v2);
