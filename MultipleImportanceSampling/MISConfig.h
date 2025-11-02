#pragma once
#include "glm/ext/vector_float3.hpp"
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

constexpr auto ASSIMP_FLAG =
    aiProcess_CalcTangentSpace | aiProcess_Triangulate |
    aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;

constexpr auto WINDOW_HEIGHT = 800;
constexpr auto WINDOW_WIDTH = 800;

constexpr auto CAMERA_POS = glm::vec3(0.0f, 0.0f, -3.0f);
constexpr auto CAMERA_UP = glm::vec3(0.f, 1.f, 0.f);
constexpr auto VIEW_DIR = glm::vec3(0.f, 0.f, 1.f);
constexpr float FOV_RAD = glm::radians(90.f);
constexpr auto MAX_BOUNCES = 5;

constexpr auto EPSILON = 1e-6f;

constexpr auto BACKGROUND_COLOR = glm::vec3(0.f);

// BRDF
constexpr int FRESNEL_POWER = 5;
constexpr glm::vec3 FRESNEL_F = glm::vec3(0.04f);
