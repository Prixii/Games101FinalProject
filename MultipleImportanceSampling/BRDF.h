#pragma once
#include "../general/Tools.h"
#include "MISConfig.h"
#include "Structs.h"
#include "glm/exponential.hpp"
#include "glm/ext/matrix_float3x3.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"

#include <cmath>
#include <cstdlib>

struct BRDFSample {
  glm::vec3 new_dir_;
  float pdf_;
  glm::vec3 brdf_color_;
};

enum SampleMethod {
  COMMON,
  IMPORTANCE_SAMPLING,
  LAMBERT,
};

class BRDF {

  glm::vec3 f0_ = FRESNEL_F; // ks
  glm::vec3 albedo_;         // kd
  float roughness_;
  float metallic_;

public:
  BRDF() = default;
  BRDF(const glm::vec3 &diffuse_color, const glm::vec3 &specular_color,
       float roughness = 0.1f, float metallic = 0.f) {
    albedo_ = diffuse_color;
    roughness_ = roughness;
    metallic_ = metallic;

    f0_ = glm::mix(FRESNEL_F, albedo_, metallic_);
  }

  BRDFSample SampleBRDF(glm::vec3 &n, glm::vec3 &wi, glm::vec3 &wo,
                        SampleMethod method);

private:
  BRDFSample SampleBRDFCommon(glm::vec3 &n_, glm::vec3 &v_);
  BRDFSample SampleBRDFImportanceSampling(glm::vec3 &n_, glm::vec3 &v_);
  BRDFSample SampleLambertBRDF(glm::vec3 &n_, glm::vec3 &v_);

  glm::vec3 EvaluateBRDF(glm::vec3 n, glm::vec3 v, glm::vec3 l);

  glm::vec3 FresnelSchlick(float cos_theta, glm::vec3 f_0 = FRESNEL_F);

  float DistributionGGX(glm::vec3 n, glm::vec3 h, float roughness);

  float GeometrySchlickGGX(float n_dot_v, float roughness);

  float GeometrySmith(glm::vec3 n, glm::vec3 v, glm::vec3 l, float roughness);
};
