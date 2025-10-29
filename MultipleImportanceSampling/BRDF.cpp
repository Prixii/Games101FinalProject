#include "BRDF.h"
BRDFSample BRDF::SampleBRDF(glm::vec3 &n_, glm::vec3 &v_) {
  BRDFSample sample;
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
  sample.brdf_color_ = EvaluateBRDF(n, v, l);
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

  return (kd * albedo_) / PI + specular;
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
