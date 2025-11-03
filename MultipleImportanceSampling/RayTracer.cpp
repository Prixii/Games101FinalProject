#include "RayTracer.h"

#include <vector>

#include "BRDF.h"
#include "MISConfig.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/geometric.hpp"

void RayTracer::Init() { pixels.reserve(WINDOW_HEIGHT * WINDOW_WIDTH); }

std::vector<glm::vec3> RayTracer::RayTracing(BasicMesh &mesh) {
  std::vector<glm::vec3> pixels(WINDOW_HEIGHT * WINDOW_WIDTH, glm::vec3(0.f));

  glm::vec3 right = glm::normalize(glm::cross(VIEW_DIR, CAMERA_UP));
  glm::vec3 up = glm::normalize(glm::cross(right, VIEW_DIR));

  const int update_interval = WINDOW_WIDTH / 100;
  for (int x = 0; x < WINDOW_WIDTH; x++) {
    if (x % update_interval == 0) {
      double percentage = x / (double)WINDOW_WIDTH * 100.f;
      printf("\rProgress: %.2f%%", percentage);
    }
    // #pragma omp parallel num_threads(6)
    // #pragma omp for
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
      Ray ray = CreateRay(x, y, right, up);

      glm::vec3 color;

      // color = TracePath(ray, mesh);
      auto [found, intersection] = ClosestIntersection(ray, mesh);
      if (!found) {
        color = BACKGROUND_COLOR;
      } else {
        auto &triangle = mesh.meshes_[intersection.mesh_index_];
        auto &brdf = mesh.brdfs_[triangle.material_idx_];
        auto &material = mesh.materials_[triangle.material_idx_];
        color = material.diffuse_color_;
      }

      pixels[GetIndex(x, y, WINDOW_WIDTH, WINDOW_HEIGHT)] = color;
    }
  }
  printf("\n");
  return pixels;
}

Ray RayTracer::CreateRay(int x, int y, glm::vec3 &right, glm::vec3 &up) {
  Ray ray{};

  ray.origin_ = CAMERA_POS;

  float aspect_ratio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
  float half_height = tan(FOV_RAD / 2.f);
  float half_width = half_height * aspect_ratio;

  float alpha = (2.f * (x + 0.5f)) / WINDOW_WIDTH - 1.f;
  float beta = (2.f * (y + 0.5f)) / WINDOW_HEIGHT - 1.f;

  glm::vec3 target =
      VIEW_DIR + alpha * half_width * right + beta * half_height * up;
  ray.direction_ = glm::normalize(target);

  return ray;
}

glm::vec3 RayTracer::TracePath(Ray &ray, BasicMesh &mesh) {
  glm::vec3 path_through_weight = glm::vec3(1.f);
  glm::vec3 color = glm::vec3(0.f, 0.f, 0.f);

  glm::vec3 dir_light_color = glm::vec3(0.f);
  bool calc_dir_light = false;
  Ray current_ray = ray;
  for (int bounce = 0; bounce < MAX_BOUNCES; bounce++) {
    auto [found, intersection] = ClosestIntersection(current_ray, mesh);

    if (!found && bounce == 0) {
      color = glm::vec3(0.1f, 0.f, 0.1f);
      break;
    }

    if (!calc_dir_light) {
      dir_light_color = CalcDirectLight(intersection, mesh);
    }

    auto &triangle = mesh.meshes_[intersection.mesh_index_];
    auto &brdf = mesh.brdfs_[triangle.material_idx_];
    auto &material = mesh.materials_[triangle.material_idx_];

    // HACK
    return dir_light_color + glm::vec3(0.5f);

    auto wi = ray.direction_;
    auto wo = intersection.position_ - CAMERA_POS;
    auto brdf_sample = brdf.SampleBRDF(intersection.normal_, wi, wo,
                                       SampleMethod::IMPORTANCE_SAMPLING);

    if (!IsVisible(brdf_sample.new_dir_, intersection.normal_) && bounce == 0) {
      break;
    }

    auto cos_theta = glm::dot(brdf_sample.new_dir_, intersection.normal_);
    path_through_weight *=
        brdf_sample.brdf_color_ * (cos_theta / brdf_sample.pdf_);

    color += path_through_weight * glm::vec3(material.diffuse_color_.x,
                                             material.diffuse_color_.y,
                                             material.diffuse_color_.z);

    current_ray.direction_ = brdf_sample.new_dir_;
    current_ray.origin_ =
        intersection.position_ + EPSILON * intersection.normal_;
    if (!RussianRoulette(0.5f)) {
      break;
    }
  }

  // todo add direct light
  return color;
}
std::pair<bool, Intersection> RayTracer::ClosestIntersection(Ray &ray,
                                                             BasicMesh &mesh) {
  Intersection closest{};
  bool found = false;

  closest.t_ = std::numeric_limits<float>::max();

  for (int mesh_idx = 0; mesh_idx < mesh.meshes_.size(); ++mesh_idx) {
    auto &sub_mesh_entry = mesh.meshes_[mesh_idx];
    for (int i = sub_mesh_entry.base_index;
         i < sub_mesh_entry.base_index + sub_mesh_entry.indices_count_;
         i += 3) {
      Intersection intersection{};

      uint32_t index0 = mesh.indices_[i];
      uint32_t index1 = mesh.indices_[i + 1];
      uint32_t index2 = mesh.indices_[i + 2];

      auto &v0 = mesh.vertices_[index0].position_;
      auto &v1 = mesh.vertices_[index1].position_;
      auto &v2 = mesh.vertices_[index2].position_;

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

      intersection.distance_ = t;
      intersection.normal_ = glm::normalize(glm::cross(edge_1, edge_2));
      intersection.mesh_index_ = mesh_idx;
      intersection.t_ = t;
      intersection.position_ = ray.origin_ + t * ray.direction_;

      if (intersection.t_ < closest.t_) {
        closest = intersection;
        found = true;
      }
    }
  }
  return std::make_pair(found, closest);
}

glm::vec3 RayTracer::CalcDirectLight(const Intersection &intersection,
                                     const BasicMesh &mesh) {
  glm::vec3 light_direction =
      glm::normalize(intersection.position_ - CAMERA_POS);
  float light_distance = glm::distance(intersection.position_, CAMERA_POS);
  glm::vec3 light_color = glm::vec3(1, 1, 1);
  glm::vec3 light_intensity = light_color / (light_distance * light_distance);
  glm::vec3 light_contribution =
      light_intensity *
      glm::max(glm::dot(-intersection.normal_, light_direction), 0.0f);
  return light_contribution;
}
