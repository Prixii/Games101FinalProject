#include "BRDF.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
BRDFSample BRDF::SampleBRDF(glm::vec3 &n_, glm::vec3 &v_, SampleMethod method) {
  BRDFSample sample{};
  switch (method) {
  case SampleMethod::COMMON:
    sample = SampleBRDFCommon(n_, v_);
    break;
  case SampleMethod::IMPORTANCE_SAMPLING:
    sample = SampleBRDFImportanceSampling(n_, v_);
    break;
  default:
    PrintErr("Invalid Method\n");
  }
  return sample;
}

BRDFSample BRDF::SampleBRDFImportanceSampling(glm::vec3 &n_, glm::vec3 &v_) {
  BRDFSample sample{};
  auto n = glm::normalize(n_);
  auto v = glm::normalize(v_);

  // 1. TBN (与原代码类似，用于局部空间到世界空间的变换)
  auto w = n;
  // 使用更安全的向量来计算 u
  auto u = glm::normalize(glm::cross(
      abs(w.x) > 0.99f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0), w));
  glm::vec3 vv = glm::cross(w, u);
  auto TBN = glm::mat3(u, vv, w);

  // 2. 2D 随机数 (保持不变)
  auto xi_1 = GetRandomFloat();
  auto xi_2 = GetRandomFloat();

  // 3. GGX Importance Sampling: 采样微表面法线 h
  float a = roughness_ * roughness_; // alpha^2
  float a2 = a * a;

  // 采样 phi (方位角)
  float phi = 2.0f * PI * xi_1;

  // 采样 cos(theta_h) (极角)
  // 来自 p_theta(h) 的逆变换抽样
  float cos_theta_h = glm::sqrt((1.0f - xi_2) / (1.0f + (a2 - 1.0f) * xi_2));
  float sin_theta_h = glm::sqrt(1.0f - cos_theta_h * cos_theta_h);

  // 4. h 在局部空间 (Tangent Space)
  glm::vec3 h_local = glm::vec3(sin_theta_h * glm::cos(phi),
                                sin_theta_h * glm::sin(phi), cos_theta_h);

  // 5. h 变换到世界空间
  auto h = TBN * h_local;

  // 6. 根据 h 和 v 计算出射光方向 l
  // l = 2 * (v · h) * h - v
  float v_dot_h = glm::dot(v, h);
  auto l = 2.0f * v_dot_h * h - v;
  sample.new_dir_ = l;

  // 7. 有效性检查 (l 必须在法线半球上方, h 必须在 v 的半球上方)
  float n_dot_l = glm::dot(n, l);
  if (n_dot_l <= 1e-6f || v_dot_h <= 1e-6f) {
    sample.pdf_ = 0.f;
    sample.brdf_color_ = glm::vec3(0.f);
    return sample;
  }

  // 8. 计算 PDF (重要性采样的 PDF)
  // pdf(l) = D(h) * (n · h) / (4 * (v · h))
  float D = DistributionGGX(n, h, roughness_); // D(h)
  float n_dot_h = glm::dot(n, h);

  sample.pdf_ = (D * n_dot_h) / (4.0f * v_dot_h);

  // 9. 计算 BRDF 颜色 (蒙特卡洛积分项)
  // BRDF_Color = f_r * (n · l) / pdf(l)
  glm::vec3 fr = EvaluateBRDF(n, v, l);

  sample.brdf_color_ =
      (fr * n_dot_l) / (sample.pdf_ + 1e-6f); // 加上一个小的 eps 防止除零


    /// TEST
  sample.brdf_color_ = glm::vec3(1.f);
  sample.pdf_ = 1.f;

  return sample;
}
BRDFSample BRDF::SampleBRDFCommon(glm::vec3 &n_, glm::vec3 &v_) {
  BRDFSample sample{};
  auto n = glm::normalize(n_);
  auto v = glm::normalize(v_);

  // 1. TBN
  auto w = n;
  auto u = glm::normalize(
      glm::cross(abs(w.x) > 1.f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0), w));

  glm::vec3 vv = glm::cross(w, u);
  auto TBN = glm::mat3(u, vv, w);

  // 2. 2d random sampling
  auto xi_1 = GetRandomFloat();
  auto xi_2 = GetRandomFloat();

  // 3. random sampling on hemisphere
  auto cos_theta = xi_1;
  auto sin_theta = glm::sqrt(1.f - cos_theta * cos_theta);

  auto phi = 2.f * PI * xi_2;

  // 4. transform to local space
  auto l_local =
      glm::vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);

  // 5. transform to world space
  auto l = TBN * l_local;
  sample.new_dir_ = l;

  // 6. calc pdf
  sample.pdf_ = 1.f / (2.f * PI);

  // 7. calc brdf
  auto lambert = std::max(glm::dot(n, l), 0.f);
  sample.brdf_color_ = EvaluateBRDF(n, v, l) * lambert;

  auto nv = glm::dot(n, v);
  sample.brdf_color_ = glm::vec3(nv);
  return sample;
}
glm::vec3 BRDF::EvaluateBRDF(glm::vec3 n, glm::vec3 v, glm::vec3 l) {
  n = glm::normalize(n);
  v = glm::normalize(v);
  l = glm::normalize(l);

  auto h = glm::normalize(v + l);

  // Normal Distribution Function
  float D = DistributionGGX(n, h, roughness_);

  // Geometric Shadowing
  float G = GeometrySmith(n, v, l, roughness_);

  // Fresnel
  float l_dot_h = std::max(glm::dot(l, h), 0.f);
  auto F = FresnelSchlick(l_dot_h, f0_);

  // Specular
  float n_dot_v = std::max(glm::dot(n, v), 0.f);
  float n_dot_l = std::max(glm::dot(n, l), 0.f);
  float denom = 4.f * n_dot_l * n_dot_v + 1e-6f;

  auto specular = (D * G * F) / denom;

  // compose
  auto ks = F;
  auto kd = glm::vec3(1.f) - ks;
  kd *= (1.f - metallic_);
  auto diffuse = (kd * albedo_) / PI;

  return diffuse + specular;
}

glm::vec3 BRDF::FresnelSchlick(float cos_theta, glm::vec3 f_0) {
  return f_0 + (1.f - f_0) * pow(1.f - cos_theta, (float)FRESNEL_POWER);
}

float BRDF::DistributionGGX(glm::vec3 n, glm::vec3 h, float roughness) {
  auto a = roughness * roughness;
  auto a2 = a * a;
  auto n_dot_h = std::max(glm::dot(n, h), 0.f);
  auto n_dot_h2 = n_dot_h * n_dot_h;

  auto nom = a2;
  auto denom = n_dot_h2 * (a2 - 1.f) + 1.f;
  return a2 / (PI * denom * denom);
}

float BRDF::GeometrySchlickGGX(float n_dot_v, float roughness) {
  float r = roughness + 1.f;
  float k = (r * r) / 8.f;

  float nom = n_dot_v;
  float denom = n_dot_v * (1.f - k) + k;

  return nom / denom;
}

float BRDF::GeometrySmith(glm::vec3 n, glm::vec3 v, glm::vec3 l,
                          float roughness) {
  float n_dot_v = std::max(dot(n, v), 0.f);
  float n_dot_l = std::max(dot(n, l), 0.f);
  float ggx2 = GeometrySchlickGGX(n_dot_v, roughness);
  float ggx1 = GeometrySchlickGGX(n_dot_l, roughness);

  return ggx1 * ggx2;
}
