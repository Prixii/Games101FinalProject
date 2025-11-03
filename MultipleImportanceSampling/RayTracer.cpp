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

  const int SPP = 16;

  const int update_interval = WINDOW_WIDTH / 100;
  for (int x = 0; x < WINDOW_WIDTH; x++) {
    if (x % update_interval == 0) {
      double percentage = x / (double)WINDOW_WIDTH * 100.f;
      printf("\rProgress: %.2f%%", percentage);
    }
#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
      glm::vec3 color(0.f);

      for (int s = 0; s < SPP; ++s) {
        float dx = GetRandomFloat() - 0.5f;
        float dy = GetRandomFloat() - 0.5f;

        Ray ray = CreateRay(x + dx, y + dy, right, up);
        color += TracePath(ray, mesh);
      }

      color /= SPP;

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
  glm::vec3 accumulated_radiance = glm::vec3(0.f);  // 累积的辐射度
  glm::vec3 path_throughput = glm::vec3(1.f);       // 路径吞吐量

  Ray current_ray = ray;
  for (int bounce = 0; bounce < MAX_BOUNCES; bounce++) {
    auto [found, intersection] = ClosestIntersection(current_ray, mesh);
    if (!found) {
      accumulated_radiance += path_throughput * BACKGROUND_COLOR;
      break;
    }

    // 获取材质和BRDF
    auto &triangle = mesh.meshes_[intersection.mesh_index_];
    auto &brdf = mesh.brdfs_[triangle.material_idx_];
    auto &material = mesh.materials_[triangle.material_idx_];

    if (material.IsLight()) {
      // 如果是光源，直接返回，不再进行后续的光线追踪
      accumulated_radiance += path_throughput * material.emissive_color_;
      break;
    }

    // --- 间接光照采样（BRDF 采样）---
    glm::vec3 wo = -current_ray.direction_;
    BRDFSample brdf_sample = brdf.SampleBRDF(intersection.normal_, wo,
                                             SampleMethod::IMPORTANCE_SAMPLING);

    // 检查新的采样方向是否有效，例如是否指向法线半球内
    // IsVisible 函数在这里可能不是判断方向是否有效的最佳名称，
    // 它应该检查 dot(new_dir, normal) 是否 > 0
    float cos_theta_sampled =
        glm::dot(brdf_sample.new_dir_, intersection.normal_);

    if (brdf_sample.pdf_ < EPSILON) {
      // PDF为0或采样方向指向下方，说明采样无效，终止路径
      break;
    }

    // 更新路径吞吐量
    path_throughput *=
        (brdf_sample.brdf_color_ * cos_theta_sampled) / brdf_sample.pdf_;

    // 只有在路径吞吐量仍然足够大时才继续
    float p = glm::max(path_throughput.x,
                       glm::max(path_throughput.y, path_throughput.z));
    if (bounce >= MIN_BOUNCES_FOR_RR) {
      if (RussianRoulette(p)) {
        break;
      }
      path_throughput /= p;
    }

    // 更新光线
    current_ray.direction_ = brdf_sample.new_dir_;
    // 沿着法线方向稍微偏移，避免自交
    current_ray.origin_ =
        intersection.position_ + EPSILON * intersection.normal_;
  }

  accumulated_radiance =
      glm::min(accumulated_radiance, glm::vec3(1.0f));  // Clamp to [0, 1] range
  return accumulated_radiance;
}

std::pair<bool, Intersection> RayTracer::ClosestIntersection(
    Ray &ray, const BasicMesh &mesh) {
  Intersection closest{};
  bool found = false;

  closest.t_ = std::numeric_limits<float>::max();

  for (int mesh_idx = 0; mesh_idx < mesh.meshes_.size(); mesh_idx++) {
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
  // 这里假设LIGHT_POS是一个点光源，并且我们计算的是直接来自该光源的贡献。
  // 注意：这个直接光照的计算方式非常基础。
  // 对于真正的PBR，你还需要考虑光源的BRDF评估。
  // 当前返回的直接光照，它会与路径吞吐量相乘。

  glm::vec3 light_direction =
      glm::normalize(LIGHT_POS - intersection.position_);
  float light_distance = glm::distance(intersection.position_, LIGHT_POS);
  glm::vec3 light_color = glm::vec3(15.f);  // 假设光源强度
  // 衰减因子：1 / (distance^2)
  float attenuation = 1.0f / (light_distance * light_distance +
                              EPSILON);  // 加上EPSILON避免除以零

  glm::vec3 L_i = light_color * attenuation;  // 入射光强度

  // 获取材质的反照率 (Lambertian diffuse)
  auto &sub_mesh_entry = mesh.meshes_[intersection.mesh_index_];
  auto &material = mesh.materials_[sub_mesh_entry.material_idx_];
  glm::vec3 albedo =
      glm::vec3(material.diffuse_color_.x, material.diffuse_color_.y,
                material.diffuse_color_.z);

  // Lambertian BRDF 因子: albedo / PI
  // cos_theta_i = dot(normal, light_direction)
  float cos_theta_i =
      glm::max(glm::dot(intersection.normal_, light_direction), 0.0f);

  // 检查是否有物体遮挡光线 (阴影检测)
  Ray shadow_ray{};
  shadow_ray.origin_ = intersection.position_ + EPSILON * intersection.normal_;
  shadow_ray.direction_ = light_direction;
  auto [shadow_hit, shadow_intersection] =
      ClosestIntersection(shadow_ray, mesh);

  if (shadow_hit && shadow_intersection.t_ < light_distance) {
    return glm::vec3(0.0f);  // 被遮挡，没有直接光照贡献
  }

  // 最终直接光照贡献 = 入射光强度 * BRDF * cos_theta_i
  // 对于Lambertian，BRDF = albedo / PI
  glm::vec3 direct_light_contribution = L_i * (albedo * INV_PI) * cos_theta_i;

  return direct_light_contribution;
}
