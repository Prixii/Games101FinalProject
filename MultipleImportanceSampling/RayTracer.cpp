#include "RayTracer.h"
#include "MISConfig.h"
#include "glm/ext/vector_float3.hpp"
Ray RayTracer::CreateRay(int x, int y, glm::vec3 &right, glm::vec3 &up) {
  Ray ray;

  ray.origin_ = CAMERA_POS;

  float aspect_ratio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
  float half_height = tan(FOV_RAD / 2.f);
  float half_width = half_height * aspect_ratio;

  float alpha = (2.f * (x + 0.5f)) / WINDOW_WIDTH - 1.f;
  float beta = (2.f * (y + 0.5f)) / WINDOW_HEIGHT - 1.f;

  glm::vec3 target =
      VIEW_DIR + alpha * half_width * right + beta * half_height * up;
  ray.direction_ = glm::normalize(target - ray.origin_);

  return ray;
}
void RayTracer::Init() { pixels.reserve(WINDOW_HEIGHT * WINDOW_WIDTH); }

void RayTracer::RayTracing(BasicMesh &mesh) {
  glm::vec3 right = glm::normalize(glm::cross(VIEW_DIR, CAMERA_UP));
  glm::vec3 up = glm::normalize(glm::cross(right, VIEW_DIR));

  for (int x = 0; x < WINDOW_WIDTH; x++) {
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
      Ray ray = CreateRay(x, y, right, up);
      glm::vec3 color = TracePath(ray, mesh);
    }
  }
}
glm::vec3 RayTracer::TracePath(Ray &ray, BasicMesh &mesh) {
  glm::vec3 path_through_weight = glm::vec3(1.f);
  glm::vec3 color = glm::vec3(0.f);

  Ray current_ray = ray;
  for (int i = 0; i < MAX_BOUNCES; i++) {
    auto [found, intersection] = ClosestIntersection(current_ray, mesh);

    if (!found) {
      color += path_through_weight * BACKGROUND_COLOR;
    }

    current_ray.origin_ = intersection.position_;
    glm::vec3 new_dir;
    // TODO SampleBRDF
    //

    if (!IsVisible(new_dir, intersection.normal_)) {
      break;
    }

    // path_through_weight *= brdf_color * (cos_theta / pdf);

    if (!RussianRoulette(0.5f)) {
      break;
    }
  }
  return color;
}

std::pair<bool, Intersection> RayTracer::ClosestIntersection(Ray &ray,
                                                             BasicMesh &mesh) {
  Intersection closest;
  bool found = false;

  closest.t_ = std::numeric_limits<float>::max();

  for (int i = 0; i < mesh.meshes_.size(); ++i) {
    auto &triangle = mesh.meshes_[i];
    Intersection intersection;

    auto &v0 = mesh.vertices_[triangle.base_vertex_].position_;
    auto &v1 = mesh.vertices_[triangle.base_vertex_ + 1].position_;
    auto &v2 = mesh.vertices_[triangle.base_vertex_ + 2].position_;

    auto edge_1 = v1 - v0;
    auto edge_2 = v2 - v0;

    auto h = glm::cross(ray.direction_, edge_2);
    auto a = glm::dot(edge_1, h);

    if (fabs(a) < EPSILON) {
      continue;
    }

    auto f = 1.0f / a;
    auto s = ray.origin_ - v0;
    float u = f * dot(s, h);
    if (u < 0.f || u > 1.f) {
      continue;
    }

    auto q = glm::cross(s, edge_1);
    auto v = f * dot(ray.direction_, q);
    if (v < 0.f || u + v > 1.f) {
      continue;
    }

    auto t = f * glm::dot(edge_2, q);
    if (t < EPSILON) {
      continue;
    }

    intersection.distance_ =
        glm::length(ray.origin_ - (ray.origin_ + ray.direction_ * t));
    intersection.normal_ = glm::normalize(glm::cross(edge_1, edge_2));
    intersection.mesh_index_ = i;
    intersection.t_ = t;

    if (intersection.t_ < closest.t_) {
      closest = intersection;
    }

    found = true;
  }
  return std::make_pair(found, closest);
}
