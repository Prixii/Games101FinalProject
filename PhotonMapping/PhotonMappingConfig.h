#pragma once
#include "glm/ext/vector_float3.hpp"
#include <glm/glm.hpp>

constexpr glm::vec3 LIGHT_POS(0, -0.5, -0.4);
constexpr glm::vec3 LIGHT_COLOR = 5.f * glm::vec3(1.0f, 1.0f, 0.95f);
constexpr glm::vec3 LIGHT_POWER = 250.f * glm::vec3(1, 1, 0.95);

constexpr auto WINDOW_WIDTH = 800;
constexpr auto WINDOW_HEIGHT = 800;
constexpr auto FOCAL_LENGTH = WINDOW_HEIGHT;

constexpr glm::vec3 CAMERA_POS(0, 0, -3);

constexpr auto NUM_PHOTONS = 50'000;

constexpr auto K_NEAREST = 550;
constexpr auto CONE_FILTER_CONST = 1.2f;
