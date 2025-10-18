#pragma once

#include "glm/exponential.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <string>

#define NOT_IMPLEMENTED                                                        \
  PrintErr("Not implemented func in %s:%d\n", __FILE__, __LINE__);             \
  exit(0);
constexpr auto PI = 3.14159265358979323846; 

bool StringStartsWith(const std::string &str, const std::string &prefix);
void PrintWarn(const char *fmt, ...);
void PrintErr(const char *fmt, ...);
void PrintInfo(const char *fmt, ...);
void PrintSuccess(const char *fmt, ...);

float GetRandomFloat(float min = 0.0f, float max = 1.0f);
glm::vec3 GetRandomDirection(const glm::vec3 &normal);

float GetSqrDist(const glm::vec3 a, const glm::vec3 b);
