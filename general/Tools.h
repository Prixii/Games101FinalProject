#pragma once

#include <glm/glm.hpp>
#include <string>

#define NOT_IMPLEMENTED                                                        \
  PrintErr("Not implemented func in %s:%d\n", __FILE__, __LINE__);             \
  exit(0);

bool StringStartsWith(const std::string &str, const std::string &prefix);
void PrintWarn(const char *fmt, ...);
void PrintErr(const char *fmt, ...);
void PrintInfo(const char *fmt, ...);
void PrintSuccess(const char *fmt, ...);

float GetRandomFloat(float min = 0.0f, float max = 1.0f);

float GetSqrDist(const glm::vec3 a, const glm::vec3 b) {
  auto sqr_dist = 0.f;
  for (int i = 0; i < 3; i++) {
    sqr_dist += (a[i] - b[i]) * (a[i] - b[i]);
  }
  return sqr_dist;
}
