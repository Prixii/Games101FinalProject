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
#pragma omp parallel num_threads(6)
#pragma omp for
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
      Ray ray = CreateRay(x, y, right, up);

      glm::vec3 color;

      color = TracePath(ray, mesh);

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
  glm::vec3 accumulated_radiance = glm::vec3(0.f); // 累积的辐射度
  glm::vec3 path_throughput = glm::vec3(1.f);      // 路径吞吐量

  Ray current_ray = ray;
  for (int bounce = 0; bounce < MAX_BOUNCES; bounce++) {
    auto [found, intersection] = ClosestIntersection(current_ray, mesh);

    if (!found) {
      // 如果没有找到交点，光线射向天空，根据背景色贡献
      // 对于间接光照，如果路径吞吐量还有效，则加上背景色。
      // 在这个简化例子中，我们假设直接光照处理了所有发射项。
      // 如果背景色是非零，可以这样处理：accumulated_radiance += path_throughput
      // * BACKGROUND_COLOR;
      accumulated_radiance += path_throughput * BACKGROUND_COLOR;
      break;
    }

    // 获取材质和BRDF
    auto &triangle = mesh.meshes_[intersection.mesh_index_];
    auto &brdf = mesh.brdfs_[triangle.material_idx_];
    auto &material = mesh.materials_[triangle.material_idx_];

    // 从交点看向相机/上一段光线的方向 (outgoing direction)
    // 这里的 wo 应该是从表面指向观测者（或上一个散射点）的方向
    glm::vec3 wo = -current_ray.direction_;

    // --- 直接光照采样（如果需要）---
    // 你可以考虑在这里添加一个显式的直接光照采样，例如采样光源。
    // 如果 CalcDirectLight 只是一个环境光或固定方向光，那么可以像以前一样处理。
    // 如果是显式光源采样，PDF 会很关键。

    // 在这里添加直接光照的贡献，一次性完成，而不是在循环外 todo add direct
    // light 假设 CalcDirectLight 返回的是该交点处接收到的直接光照强度
    // 注意：这里的 CalcDirectLight 似乎是一个简化的模型，
    // 它返回的值直接包含了颜色和Lambertian反射。
    glm::vec3 direct_light_contribution = CalcDirectLight(intersection, mesh);
    accumulated_radiance += path_throughput * direct_light_contribution;

    // --- 间接光照采样（BRDF 采样）---
    // 使用 BRDF 采样得到一个新的方向 wi
    BRDFSample brdf_sample =
        brdf.SampleBRDF(intersection.normal_, wo, SampleMethod::LAMBERT);

    // 检查新的采样方向是否有效，例如是否指向法线半球内
    // IsVisible 函数在这里可能不是判断方向是否有效的最佳名称，
    // 它应该检查 dot(new_dir, normal) 是否 > 0
    float cos_theta_sampled =
        glm::dot(brdf_sample.new_dir_, intersection.normal_);

    if (brdf_sample.pdf_ < EPSILON || cos_theta_sampled < EPSILON) {
      // PDF为0或采样方向指向下方，说明采样无效，终止路径
      break;
    }

    // 更新路径吞吐量
    // path_throughput = path_throughput * (BRDF_Value * cos_theta) / PDF
    // brdf_sample.brdf_color_ 已经是 albedo * INV_PI
    // cos_theta_sampled 是 dot(n, wi)
    path_throughput *=
        (brdf_sample.brdf_color_ * cos_theta_sampled) / brdf_sample.pdf_;

    // 接下来检查俄罗斯轮盘赌
    // 只有在路径吞吐量仍然足够大时才继续
    float p = glm::max(path_throughput.x,
                       glm::max(path_throughput.y, path_throughput.z));
    if (bounce >= MIN_BOUNCES_FOR_RR) {
      if (RussianRoulette(p)) {
        break; // 路径终止
      }
      path_throughput /= p; // 路径未终止，对吞吐量进行缩放
    }

    // 更新光线
    current_ray.direction_ = brdf_sample.new_dir_;
    // 沿着法线方向稍微偏移，避免自交
    current_ray.origin_ =
        intersection.position_ + EPSILON * intersection.normal_;
  }

  accumulated_radiance =
      glm::min(accumulated_radiance, glm::vec3(1.0f)); // Clamp to [0, 1] range
  return accumulated_radiance;
}
std::pair<bool, Intersection>
RayTracer::ClosestIntersection(Ray &ray, const BasicMesh &mesh) {
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
  // 这里假设LIGHT_POS是一个点光源，并且我们计算的是直接来自该光源的贡献。
  // 注意：这个直接光照的计算方式非常基础。
  // 对于真正的PBR，你还需要考虑光源的BRDF评估。
  // 当前返回的直接光照，它会与路径吞吐量相乘。

  glm::vec3 light_direction =
      glm::normalize(LIGHT_POS - intersection.position_);
  float light_distance = glm::distance(intersection.position_, LIGHT_POS);
  glm::vec3 light_color = glm::vec3(15.f); // 假设光源强度
  // 衰减因子：1 / (distance^2)
  float attenuation = 1.0f / (light_distance * light_distance +
                              EPSILON); // 加上EPSILON避免除以零

  glm::vec3 L_i = light_color * attenuation; // 入射光强度

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
    return glm::vec3(0.0f); // 被遮挡，没有直接光照贡献
  }

  // 最终直接光照贡献 = 入射光强度 * BRDF * cos_theta_i
  // 对于Lambertian，BRDF = albedo / PI
  glm::vec3 direct_light_contribution = L_i * (albedo * INV_PI) * cos_theta_i;

  return direct_light_contribution;
}
