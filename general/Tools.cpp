#include "Tools.h"
#include <stdarg.h>

bool StringStartsWith(const std::string &str, const std::string &prefix) {
  if (str.size() < prefix.size()) {
    return false;
  }

  for (size_t i = 0; i < prefix.size(); i++) {
    if (str[i] != prefix[i]) {
      return false;
    }
  }
  return true;
}

void PrintWarn(const char *fmt, ...) {
  printf("\033[1;33m[Warning]:\033[0m ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void PrintErr(const char *fmt, ...) {
  printf("\033[1;31m[Error]:\033[0m ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void PrintInfo(const char *fmt, ...) {
  printf("\033[1;34m[Info]:\033[0m ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void PrintSuccess(const char *fmt, ...) {
  printf("\033[1;32m[Success]:\033[0m ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}
float GetRandomFloat(float min, float max) {
  return min + static_cast<float>(rand()) /
                   (static_cast<float>(RAND_MAX / (max - min)));
}
float GetSqrDist(const glm::vec3 a, const glm::vec3 b) {
  auto sqr_dist = 0.f;
  for (int i = 0; i < 3; i++) {
    sqr_dist += (a[i] - b[i]) * (a[i] - b[i]);
  }
  return sqr_dist;
}
glm::vec3 GetRandomDirection(const glm::vec3 &normal) {
  float z = glm::sqrt(GetRandomFloat());
  float r = sqrt(1.0 - z * z);
  float phi = 2.0 * PI * GetRandomFloat();
  float x = r * cos(phi);
  float y = r * sin(phi);

  glm::vec3 w, u, v;
  w = normal;
  u = glm::normalize(
      glm::cross(abs(w.x) > .1 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0), w));
  v = glm::cross(normal, u);

  return x * u + y * v + z * w;
}
int GetIndex(float x, float y, int width, int height, bool reverse_y) {
  auto _x = std::clamp((int)x, 0, width - 1);
  auto _y = std::clamp((int)y, 0, height - 1);

  if (reverse_y) {
    _y = height - _y - 1;
  }

  return static_cast<int>(_x * height + _y);
}
bool RussianRoulette(float probability) {
  return GetRandomFloat() > probability;
}
bool IsVisible(glm::vec3 &v1, glm::vec3 &v2) { return glm::dot(v1, v2) > 0.0f; }
